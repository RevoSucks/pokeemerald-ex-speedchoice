#include "global.h"
#include "malloc.h"
#include "berry_powder.h"
#include "item.h"
#include "load_save.h"
#include "main.h"
#include "overworld.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "save_location.h"
#include "trainer_hill.h"
#include "gba/flash_internal.h"
#include "decoration_inventory.h"
#include "agb_flash.h"
#include "done_button.h"
#include "sound.h"
#include "speedchoice.h"
#include "constants/sound.h"

static void ApplyNewEncryptionKeyToAllEncryptedData(u32 encryptionKey);

#define SAVEBLOCK_MOVE_RANGE    128

struct LoadedSaveData
{
 /*0x0000*/ struct ItemSlot items[BAG_ITEMS_COUNT];
 /*0x0078*/ struct ItemSlot keyItems[BAG_KEYITEMS_COUNT];
 /*0x00F0*/ struct ItemSlot pokeBalls[BAG_POKEBALLS_COUNT];
 /*0x0130*/ struct ItemSlot TMsHMs[BAG_TMHM_COUNT];
 /*0x0230*/ struct ItemSlot berries[BAG_BERRIES_COUNT];
 /*0x02E8*/ struct MailStruct mail[MAIL_COUNT];
};

// EWRAM DATA
EWRAM_DATA struct SaveBlock2 gSaveblock2 = {0};
EWRAM_DATA u8 gSaveblock2_DMA[SAVEBLOCK_MOVE_RANGE] = {0};

EWRAM_DATA struct SaveBlock1 gSaveblock1 = {0};
EWRAM_DATA u8 gSaveblock1_DMA[SAVEBLOCK_MOVE_RANGE] = {0};

EWRAM_DATA struct PokemonStorage gPokemonStorage = {0};
EWRAM_DATA u8 gSaveblock3_DMA[SAVEBLOCK_MOVE_RANGE] = {0};

EWRAM_DATA struct LoadedSaveData gLoadedSaveData = {0};
EWRAM_DATA u32 gLastEncryptionKey = 0;

// IWRAM common
bool32 gFlashMemoryPresent;
struct SaveBlock1 *gSaveBlock1Ptr;
struct SaveBlock2 *gSaveBlock2Ptr;
struct PokemonStorage *gPokemonStoragePtr;

// code
void CheckForFlashMemory(void)
{
    if (!IdentifyFlash())
    {
        gFlashMemoryPresent = TRUE;
        InitFlashTimer();
    }
    else
    {
        gFlashMemoryPresent = FALSE;
    }
}

void ClearSav2(void)
{
    CpuFill16(0, &gSaveblock2, sizeof(struct SaveBlock2) + sizeof(gSaveblock2_DMA));
}

void ClearSav1(void)
{
    CpuFill16(0, &gSaveblock1, sizeof(struct SaveBlock1) + sizeof(gSaveblock1_DMA));
}

void SetSaveBlocksPointers(u16 offset)
{
    struct SaveBlock1** sav1_LocalVar = &gSaveBlock1Ptr;

    // SPEEDCHOICE CHANGE: Disable the random Save Block DMAing crap.
    //offset = (offset + Random()) & (SAVEBLOCK_MOVE_RANGE - 4);
    offset = 0;

    gSaveBlock2Ptr = (void*)(&gSaveblock2) + offset;
    *sav1_LocalVar = (void*)(&gSaveblock1) + offset;
    gPokemonStoragePtr = (void*)(&gPokemonStorage) + offset;

    SetBagItemsPointers();
    SetDecorationInventoriesPointers();
}

void MoveSaveBlocks_ResetHeap(void)
{
    void *vblankCB, *hblankCB;
    u32 encryptionKey;
    struct SaveBlock2 *saveBlock2Copy;
    struct SaveBlock1 *saveBlock1Copy;
    struct PokemonStorage *pokemonStorageCopy;

    // save interrupt functions and turn them off
    vblankCB = gMain.vblankCallback;
    hblankCB = gMain.hblankCallback;
    gMain.vblankCallback = NULL;
    gMain.hblankCallback = NULL;
    gTrainerHillVBlankCounter = NULL;

    saveBlock2Copy = (struct SaveBlock2 *)(gHeap);
    saveBlock1Copy = (struct SaveBlock1 *)(gHeap + sizeof(struct SaveBlock2));
    pokemonStorageCopy = (struct PokemonStorage *)(gHeap + sizeof(struct SaveBlock2) + sizeof(struct SaveBlock1));

    // backup the saves.
    *saveBlock2Copy = *gSaveBlock2Ptr;
    *saveBlock1Copy = *gSaveBlock1Ptr;
    *pokemonStorageCopy = *gPokemonStoragePtr;

    // change saveblocks' pointers
    // argument is a sum of the individual trainerId bytes
    SetSaveBlocksPointers(
      saveBlock2Copy->playerTrainerId[0] +
      saveBlock2Copy->playerTrainerId[1] +
      saveBlock2Copy->playerTrainerId[2] +
      saveBlock2Copy->playerTrainerId[3]);

    // restore saveblock data since the pointers changed
    *gSaveBlock2Ptr = *saveBlock2Copy;
    *gSaveBlock1Ptr = *saveBlock1Copy;
    *gPokemonStoragePtr = *pokemonStorageCopy;

    // heap was destroyed in the copying process, so reset it
    InitHeap(gHeap, HEAP_SIZE);

    // restore interrupt functions
    gMain.hblankCallback = hblankCB;
    gMain.vblankCallback = vblankCB;

    // create a new encryption key
    encryptionKey = (Random() << 16) + (Random());
    ApplyNewEncryptionKeyToAllEncryptedData(encryptionKey);
    gSaveBlock2Ptr->encryptionKey = encryptionKey;
}

u32 UseContinueGameWarp(void)
{
    return gSaveBlock2Ptr->specialSaveWarpFlags & CONTINUE_GAME_WARP;
}

void ClearContinueGameWarpStatus(void)
{
    gSaveBlock2Ptr->specialSaveWarpFlags &= ~CONTINUE_GAME_WARP;
}

void SetContinueGameWarpStatus(void)
{
    gSaveBlock2Ptr->specialSaveWarpFlags |= CONTINUE_GAME_WARP;
}

void SetContinueGameWarpStatusToDynamicWarp(void)
{
    SetContinueGameWarpToDynamicWarp(0);
    gSaveBlock2Ptr->specialSaveWarpFlags |= CONTINUE_GAME_WARP;
}

void ClearContinueGameWarpStatus2(void)
{
    gSaveBlock2Ptr->specialSaveWarpFlags &= ~CONTINUE_GAME_WARP;
}

void SavePlayerParty(void)
{
    int i;

    gSaveBlock1Ptr->playerPartyCount = gPlayerPartyCount;

    for (i = 0; i < PARTY_SIZE; i++)
        gSaveBlock1Ptr->playerParty[i] = gPlayerParty[i];
}

void LoadPlayerParty(void)
{
    int i;

    gPlayerPartyCount = gSaveBlock1Ptr->playerPartyCount;

    for (i = 0; i < PARTY_SIZE; i++)
        gPlayerParty[i] = gSaveBlock1Ptr->playerParty[i];
}

extern int gShuffleMusic;

void SaveObjectEvents(void)
{
    int i;

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
        gSaveBlock1Ptr->objectEvents[i] = gObjectEvents[i];
}

#include "constants/songs.h"

extern u32 gRandomizerCheckValue;

extern int IsInFanfares(u16 songNum);

extern EWRAM_DATA u16 gShuffledMusic[(SONGS_END - SONGS_START + 2) - SFANFARES_COUNT][2]; // add 1 for VS Wild Night, 1 for terminator
extern EWRAM_DATA u16 gShuffledFanfares[SFANFARES_COUNT+1][2]; // add 1 for terminator

struct Fanfare
{
    u16 songNum;
    u16 duration;
};

extern const struct Fanfare sFanfares[];

// i could just do range checks to not have these lists, but fuck it, i want easy to edit lists.
static const u16 gExpansionMusicTracks[] = {
    // Gen 4 (D/P)
    MUS_DP_TWINLEAF_DAY               ,
    MUS_DP_SANDGEM_DAY                ,
    MUS_DP_FLOAROMA_DAY               ,
    MUS_DP_SOLACEON_DAY               ,
    MUS_DP_ROUTE225_DAY               ,
    MUS_DP_VALOR_LAKEFRONT_DAY        ,
    MUS_DP_JUBILIFE_DAY               ,
    MUS_DP_CANALAVE_DAY               ,
    MUS_DP_OREBURGH_DAY               ,
    MUS_DP_ETERNA_DAY                 ,
    MUS_DP_HEARTHOME_DAY              ,
    MUS_DP_VEILSTONE_DAY              ,
    MUS_DP_SUNYSHORE_DAY              ,
    MUS_DP_SNOWPOINT_DAY              ,
    MUS_DP_POKEMON_LEAGUE_DAY         ,
    MUS_DP_FIGHT_AREA_DAY             ,
    MUS_DP_ROUTE201_DAY               ,
    MUS_DP_ROUTE203_DAY               ,
    MUS_DP_ROUTE205_DAY               ,
    MUS_DP_ROUTE206_DAY               ,
    MUS_DP_ROUTE209_DAY               ,
    MUS_DP_ROUTE210_DAY               ,
    MUS_DP_ROUTE216_DAY               ,
    MUS_DP_ROUTE228_DAY               ,
    MUS_DP_ROWAN                      ,
    MUS_DP_TV_BROADCAST               ,
    MUS_DP_TWINLEAF_NIGHT             ,
    MUS_DP_SANDGEM_NIGHT              ,
    MUS_DP_FLOAROMA_NIGHT             ,
    MUS_DP_SOLACEON_NIGHT             ,
    MUS_DP_ROUTE225_NIGHT             ,
    MUS_DP_VALOR_LAKEFRONT_NIGHT      ,
    MUS_DP_JUBILIFE_NIGHT             ,
    MUS_DP_CANALAVE_NIGHT             ,
    MUS_DP_OREBURGH_NIGHT             ,
    MUS_DP_ETERNA_NIGHT               ,
    MUS_DP_HEARTHOME_NIGHT            ,
    MUS_DP_VEILSTONE_NIGHT            ,
    MUS_DP_SUNYSHORE_NIGHT            ,
    MUS_DP_SNOWPOINT_NIGHT            ,
    MUS_DP_POKEMON_LEAGUE_NIGHT       ,
    MUS_DP_FIGHT_AREA_NIGHT           ,
    MUS_DP_ROUTE201_NIGHT             ,
    MUS_DP_ROUTE203_NIGHT             ,
    MUS_DP_ROUTE205_NIGHT             ,
    MUS_DP_ROUTE206_NIGHT             ,
    MUS_DP_ROUTE209_NIGHT             ,
    MUS_DP_ROUTE210_NIGHT             ,
    MUS_DP_ROUTE216_NIGHT             ,
    MUS_DP_ROUTE228_NIGHT             ,
    MUS_DP_UNDERGROUND                ,
    MUS_DP_FLAG_CAPTURED              ,
    MUS_DP_VICTORY_ROAD               ,
    MUS_DP_ETERNA_FOREST              ,
    MUS_DP_OLD_CHATEAU                ,
    MUS_DP_LAKE_CAVERNS               ,
    MUS_DP_AMITY_SQUARE               ,
    MUS_DP_GALACTIC_HQ                ,
    MUS_DP_GALACTIC_ETERNA_BUILDING   ,
    MUS_DP_GREAT_MARSH                ,
    MUS_DP_LAKE                       ,
    MUS_DP_MT_CORONET                 ,
    MUS_DP_SPEAR_PILLAR               ,
    MUS_DP_STARK_MOUNTAIN             ,
    MUS_DP_OREBURGH_GATE              ,
    MUS_DP_OREBURGH_MINE              ,
    MUS_DP_INSIDE_POKEMON_LEAGUE      ,
    MUS_DP_HALL_OF_FAME_ROOM          ,
    MUS_DP_POKE_CENTER_DAY            ,
    MUS_DP_POKE_CENTER_NIGHT          ,
    MUS_DP_GYM                        ,
    MUS_DP_ROWAN_LAB                  ,
    MUS_DP_CONTEST_LOBBY              ,
    MUS_DP_POKE_MART                  ,
    MUS_DP_GAME_CORNER                ,
    MUS_DP_B_TOWER                    ,
    MUS_DP_TV_STATION                 ,
    MUS_DP_GALACTIC_HQ_BASEMENT       ,
    MUS_DP_AZURE_FLUTE                ,
    MUS_DP_HALL_OF_ORIGIN             ,
    MUS_DP_GTS                        ,
    MUS_DP_ENCOUNTER_BOY              ,
    MUS_DP_ENCOUNTER_TWINS            ,
    MUS_DP_ENCOUNTER_INTENSE          ,
    MUS_DP_ENCOUNTER_GALACTIC         ,
    MUS_DP_ENCOUNTER_LADY             ,
    MUS_DP_ENCOUNTER_HIKER            ,
    MUS_DP_ENCOUNTER_RICH             ,
    MUS_DP_ENCOUNTER_SAILOR           ,
    MUS_DP_ENCOUNTER_SUSPICIOUS       ,
    MUS_DP_ENCOUNTER_ACE_TRAINER      ,
    MUS_DP_ENCOUNTER_GIRL             ,
    MUS_DP_ENCOUNTER_CYCLIST          ,
    MUS_DP_ENCOUNTER_ARTIST           ,
    MUS_DP_ENCOUNTER_ELITE_FOUR       ,
    MUS_DP_ENCOUNTER_CHAMPION         ,
    MUS_DP_VS_WILD                    ,
    MUS_DP_VS_GYM_LEADER              ,
    MUS_DP_VS_UXIE_MESPRIT_AZELF      ,
    MUS_DP_VS_TRAINER                 ,
    MUS_DP_VS_GALACTIC_BOSS           ,
    MUS_DP_VS_DIALGA_PALKIA           ,
    MUS_DP_VS_CHAMPION                ,
    MUS_DP_VS_GALACTIC                ,
    MUS_DP_VS_RIVAL                   ,
    MUS_DP_VS_ARCEUS                  ,
    MUS_DP_VS_LEGEND                  ,
    MUS_DP_VICTORY_WILD               ,
    MUS_DP_VICTORY_TRAINER            ,
    MUS_DP_VICTORY_GYM_LEADER         ,
    MUS_DP_VICTORY_CHAMPION           ,
    MUS_DP_VICTORY_GALACTIC           ,
    MUS_DP_VICTORY_ELITE_FOUR         ,
    MUS_DP_VS_GALACTIC_COMMANDER      ,
    MUS_DP_CONTEST                    ,
    MUS_DP_VS_ELITE_FOUR              ,
    MUS_DP_FOLLOW_ME                  ,
    MUS_DP_RIVAL                      ,
    MUS_DP_LAKE_EVENT                 ,
    MUS_DP_EVOLUTION                  ,
    MUS_DP_LUCAS                      ,
    MUS_DP_DAWN                       ,
    MUS_DP_LEGEND_APPEARS             ,
    MUS_DP_CATASTROPHE                ,
    MUS_DP_POKE_RADAR                 ,
    MUS_DP_SURF                       ,
    MUS_DP_CYCLING                    ,
    MUS_DP_LETS_GO_TOGETHER           ,
    MUS_DP_TV_END                     ,
    MUS_DP_LEVEL_UP                   ,
    MUS_DP_EVOLVED                    ,
    MUS_DP_OBTAIN_KEY_ITEM            ,
    MUS_DP_OBTAIN_ITEM                ,
    MUS_DP_CAUGHT_INTRO               ,
    MUS_DP_DEX_RATING                 ,
    MUS_DP_OBTAIN_BADGE               ,
    MUS_DP_POKETCH                    ,
    MUS_DP_OBTAIN_TMHM                ,
    MUS_DP_OBTAIN_ACCESSORY           ,
    MUS_DP_MOVE_DELETED               ,
    MUS_DP_HEAL                       ,
    MUS_DP_OBTAIN_BERRY               ,
    MUS_DP_CONTEST_DRESS_UP           ,
    MUS_DP_HALL_OF_FAME               ,
    MUS_DP_INTRO                      ,
    MUS_DP_TITLE                      ,
    MUS_DP_MYSTERY_GIFT               ,
    MUS_DP_WFC                        ,
    MUS_DP_DANCE_EASY                 ,
    MUS_DP_DANCE_DIFFICULT            ,
    MUS_DP_CONTEST_RESULTS            ,
    MUS_DP_CONTEST_WINNER             ,
    MUS_DP_POFFINS                    ,
    MUS_DP_SLOTS_WIN                  ,
    MUS_DP_SLOTS_JACKPOT              ,
    MUS_DP_CREDITS                    ,
    MUS_DP_SLOTS_UNUSED               ,
    MUS_PL_FIGHT_AREA_DAY             ,
    MUS_PL_TV_BROADCAST               ,
    MUS_PL_TV_END                     ,
    MUS_PL_INTRO                      ,
    MUS_PL_TITLE                      ,
    MUS_PL_DISTORTION_WORLD           ,
    MUS_PL_B_ARCADE                   ,
    MUS_PL_B_HALL                     ,
    MUS_PL_B_CASTLE                   ,
    MUS_PL_B_FACTORY                  ,
    MUS_PL_GLOBAL_TERMINAL            ,
    MUS_PL_LILYCOVE_BOSSA_NOVA        ,
    MUS_PL_LOOKER                     ,
    MUS_PL_VS_GIRATINA                ,
    MUS_PL_VS_FRONTIER_BRAIN          ,
    MUS_PL_VICTORY_FRONTIER_BRAIN     ,
    MUS_PL_VS_REGI                    ,
    MUS_PL_CONTEST_COOL               ,
    MUS_PL_CONTEST_SMART              ,
    MUS_PL_CONTEST_CUTE               ,
    MUS_PL_CONTEST_TOUGH              ,
    MUS_PL_CONTEST_BEAUTY             ,
    MUS_PL_SPIN_TRADE                 ,
    MUS_PL_WIFI_MINIGAMES             ,
    MUS_PL_WIFI_PLAZA                 ,
    MUS_PL_WIFI_PARADE                ,
    MUS_PL_GIRATINA_APPEARS_1         ,
    MUS_PL_GIRATINA_APPEARS_2         ,
    MUS_PL_MYSTERY_GIFT               ,
    MUS_PL_TWINLEAF_MUSIC_BOX         ,
    MUS_PL_OBTAIN_ARCADE_POINTS       ,
    MUS_PL_OBTAIN_CASTLE_POINTS       ,
    MUS_PL_OBTAIN_B_POINTS            ,
    MUS_PL_WIN_MINIGAME               ,
    MUS_HG_INTRO                      ,
    MUS_HG_TITLE                      ,
    MUS_HG_NEW_GAME                   ,
    MUS_HG_EVOLUTION                  ,
    MUS_HG_EVOLUTION_NO_INTRO         ,
    MUS_HG_CYCLING                    ,
    MUS_HG_SURF                       ,
    MUS_HG_E_DENDOURIRI               ,
    MUS_HG_CREDITS                    ,
    MUS_HG_END                        ,
    MUS_HG_NEW_BARK                   ,
    MUS_HG_CHERRYGROVE                ,
    MUS_HG_VIOLET                     ,
    MUS_HG_AZALEA                     ,
    MUS_HG_GOLDENROD                  ,
    MUS_HG_ECRUTEAK                   ,
    MUS_HG_CIANWOOD                   ,
    MUS_HG_ROUTE29                    ,
    MUS_HG_ROUTE30                    ,
    MUS_HG_ROUTE34                    ,
    MUS_HG_ROUTE38                    ,
    MUS_HG_ROUTE42                    ,
    MUS_HG_VERMILION                  ,
    MUS_HG_PEWTER                     ,
    MUS_HG_CERULEAN                   ,
    MUS_HG_LAVENDER                   ,
    MUS_HG_CELADON                    ,
    MUS_HG_PALLET                     ,
    MUS_HG_CINNABAR                   ,
    MUS_HG_ROUTE1                     ,
    MUS_HG_ROUTE3                     ,
    MUS_HG_ROUTE11                    ,
    MUS_HG_ROUTE24                    ,
    MUS_HG_ROUTE26                    ,
    MUS_HG_POKE_CENTER                ,
    MUS_HG_POKE_MART                  ,
    MUS_HG_GYM                        ,
    MUS_HG_ELM_LAB                    ,
    MUS_HG_OAK                        ,
    MUS_HG_DANCE_THEATER              ,
    MUS_HG_GAME_CORNER                ,
    MUS_HG_B_TOWER                    ,
    MUS_HG_B_TOWER_RECEPTION          ,
    MUS_HG_SPROUT_TOWER               ,
    MUS_HG_UNION_CAVE                 ,
    MUS_HG_RUINS_OF_ALPH              ,
    MUS_HG_NATIONAL_PARK              ,
    MUS_HG_BURNED_TOWER               ,
    MUS_HG_BELL_TOWER                 ,
    MUS_HG_LIGHTHOUSE                 ,
    MUS_HG_TEAM_ROCKET_HQ             ,
    MUS_HG_ICE_PATH                   ,
    MUS_HG_DRAGONS_DEN                ,
    MUS_HG_ROCK_TUNNEL                ,
    MUS_HG_VIRIDIAN_FOREST            ,
    MUS_HG_VICTORY_ROAD               ,
    MUS_HG_POKEMON_LEAGUE             ,
    MUS_HG_FOLLOW_ME_1                ,
    MUS_HG_FOLLOW_ME_2                ,
    MUS_HG_ENCOUNTER_RIVAL            ,
    MUS_HG_RIVAL_EXIT                 ,
    MUS_HG_BUG_CONTEST_PREP           ,
    MUS_HG_BUG_CATCHING_CONTEST       ,
    MUS_HG_RADIO_ROCKET               ,
    MUS_HG_ROCKET_TAKEOVER            ,
    MUS_HG_MAGNET_TRAIN               ,
    MUS_HG_SS_AQUA                    ,
    MUS_HG_MT_MOON_SQUARE             ,
    MUS_HG_RADIO_JINGLE               ,
    MUS_HG_RADIO_LULLABY              ,
    MUS_HG_RADIO_MARCH                ,
    MUS_HG_RADIO_UNOWN                ,
    MUS_HG_RADIO_POKE_FLUTE           ,
    MUS_HG_RADIO_OAK                  ,
    MUS_HG_RADIO_BUENA                ,
    MUS_HG_EUSINE                     ,
    MUS_HG_CLAIR                      ,
    MUS_HG_ENCOUNTER_GIRL_1           ,
    MUS_HG_ENCOUNTER_BOY_1            ,
    MUS_HG_ENCOUNTER_SUSPICIOUS_1     ,
    MUS_HG_ENCOUNTER_SAGE             ,
    MUS_HG_ENCOUNTER_KIMONO_GIRL      ,
    MUS_HG_ENCOUNTER_ROCKET           ,
    MUS_HG_ENCOUNTER_GIRL_2           ,
    MUS_HG_ENCOUNTER_BOY_2            ,
    MUS_HG_ENCOUNTER_SUSPICIOUS_2     ,
    MUS_HG_VS_WILD                    ,
    MUS_HG_VS_TRAINER                 ,
    MUS_HG_VS_GYM_LEADER              ,
    MUS_HG_VS_RIVAL                   ,
    MUS_HG_VS_ROCKET                  ,
    MUS_HG_VS_SUICUNE                 ,
    MUS_HG_VS_ENTEI                   ,
    MUS_HG_VS_RAIKOU                  ,
    MUS_HG_VS_CHAMPION                ,
    MUS_HG_VS_WILD_KANTO              ,
    MUS_HG_VS_TRAINER_KANTO           ,
    MUS_HG_VS_GYM_LEADER_KANTO        ,
    MUS_HG_VICTORY_TRAINER            ,
    MUS_HG_VICTORY_WILD               ,
    MUS_HG_CAUGHT                     ,
    MUS_HG_VICTORY_GYM_LEADER         ,
    MUS_HG_VS_HO_OH                   ,
    MUS_HG_VS_LUGIA                   ,
    MUS_HG_POKEATHLON_LOBBY           ,
    MUS_HG_POKEATHLON_START           ,
    MUS_HG_POKEATHLON_BEFORE          ,
    MUS_HG_POKEATHLON_EVENT           ,
    MUS_HG_POKEATHLON_FINALS          ,
    MUS_HG_POKEATHLON_RESULTS         ,
    MUS_HG_POKEATHLON_END             ,
    MUS_HG_POKEATHLON_WINNER          ,
    MUS_HG_B_FACTORY                  ,
    MUS_HG_B_HALL                     ,
    MUS_HG_B_ARCADE                   ,
    MUS_HG_B_CASTLE                   ,
    MUS_HG_VS_FRONTIER_BRAIN          ,
    MUS_HG_VICTORY_FRONTIER_BRAIN     ,
    MUS_HG_WFC                        ,
    MUS_HG_MYSTERY_GIFT               ,
    MUS_HG_WIFI_PLAZA                 ,
    MUS_HG_WIFI_MINIGAMES             ,
    MUS_HG_WIFI_PARADE                ,
    MUS_HG_GLOBAL_TERMINAL            ,
    MUS_HG_SPIN_TRADE                 ,
    MUS_HG_GTS                        ,
    MUS_HG_ROUTE47                    ,
    MUS_HG_SAFARI_ZONE_GATE           ,
    MUS_HG_SAFARI_ZONE                ,
    MUS_HG_ETHAN                      ,
    MUS_HG_LYRA                       ,
    MUS_HG_GAME_CORNER_WIN            ,
    MUS_HG_KIMONO_GIRL_DANCE          ,
    MUS_HG_KIMONO_GIRL                ,
    MUS_HG_HO_OH_APPEARS              ,
    MUS_HG_LUGIA_APPEARS              ,
    MUS_HG_SPIKY_EARED_PICHU          ,
    MUS_HG_SINJOU_RUINS               ,
    MUS_HG_RADIO_ROUTE101             ,
    MUS_HG_RADIO_ROUTE201             ,
    MUS_HG_RADIO_TRAINER              ,
    MUS_HG_RADIO_VARIETY              ,
    MUS_HG_VS_KYOGRE_GROUDON          ,
    MUS_HG_POKEWALKER                 ,
    MUS_HG_VS_ARCEUS                  ,
    MUS_HG_HEAL                       ,
    MUS_HG_LEVEL_UP                   ,
    MUS_HG_OBTAIN_ITEM                ,
    MUS_HG_OBTAIN_KEY_ITEM            ,
    MUS_HG_EVOLVED                    ,
    MUS_HG_OBTAIN_BADGE               ,
    MUS_HG_OBTAIN_TMHM                ,
    MUS_HG_OBTAIN_ACCESSORY           ,
    MUS_HG_MOVE_DELETED               ,
    MUS_HG_OBTAIN_BERRY               ,
    MUS_HG_DEX_RATING_1               ,
    MUS_HG_DEX_RATING_2               ,
    MUS_HG_DEX_RATING_3               ,
    MUS_HG_DEX_RATING_4               ,
    MUS_HG_DEX_RATING_5               ,
    MUS_HG_DEX_RATING_6               ,
    MUS_HG_OBTAIN_EGG                 ,
    MUS_HG_BUG_CONTEST_1ST_PLACE      ,
    MUS_HG_BUG_CONTEST_2ND_PLACE      ,
    MUS_HG_BUG_CONTEST_3RD_PLACE      ,
    MUS_HG_CARD_FLIP                  ,
    MUS_HG_CARD_FLIP_GAME_OVER        ,
    MUS_HG_POKEGEAR_REGISTERED        ,
    MUS_HG_LETS_GO_TOGETHER           ,
    MUS_HG_POKEATHLON_READY           ,
    MUS_HG_POKEATHLON_1ST_PLACE       ,
    MUS_HG_RECEIVE_POKEMON            ,
    MUS_HG_OBTAIN_ARCADE_POINTS       ,
    MUS_HG_OBTAIN_CASTLE_POINTS       ,
    MUS_HG_OBTAIN_B_POINTS            ,
    MUS_HG_WIN_MINIGAME
};

static const u16 gExpansionFanfareTracks[] = {
    FANFARE_DP_TV_END                 ,
    FANFARE_DP_OBTAIN_ITEM            ,
    FANFARE_DP_HEAL                   ,
    FANFARE_DP_OBTAIN_KEY_ITEM        ,
    FANFARE_DP_OBTAIN_TMHM            ,
    FANFARE_DP_OBTAIN_BADGE           ,
    FANFARE_DP_LEVEL_UP               ,
    FANFARE_DP_OBTAIN_BERRY           ,
    FANFARE_DP_PARTNER                ,
    FANFARE_DP_EVOLVED                ,
    FANFARE_DP_POKETCH                ,
    FANFARE_DP_MOVE_DELETED           ,
    FANFARE_DP_ACCESSORY              ,
    FANFARE_PL_TV_END                 ,
    FANFARE_PL_CLEAR_MINIGAME         ,
    FANFARE_PL_OBTAIN_ARCADE_POINTS   ,
    FANFARE_PL_OBTAIN_CASTLE_POINTS   ,
    FANFARE_PL_OBTAIN_B_POINTS        ,
    FANFARE_HG_OBTAIN_KEY_ITEM        ,
    FANFARE_HG_LEVEL_UP               ,
    FANFARE_HG_HEAL                   ,
    FANFARE_HG_DEX_RATING_1           ,
    FANFARE_HG_DEX_RATING_2           ,
    FANFARE_HG_DEX_RATING_3           ,
    FANFARE_HG_DEX_RATING_4           ,
    FANFARE_HG_DEX_RATING_5           ,
    FANFARE_HG_DEX_RATING_6           ,
    FANFARE_HG_RECEIVE_EGG            ,
    FANFARE_HG_OBTAIN_ITEM            ,
    FANFARE_HG_EVOLVED                ,
    FANFARE_HG_OBTAIN_BADGE           ,
    FANFARE_HG_OBTAIN_TMHM            ,
    FANFARE_HG_VOLTORB_FLIP_1         ,
    FANFARE_HG_VOLTORB_FLIP_2         ,
    FANFARE_HG_ACCESSORY              ,
    FANFARE_HG_REGISTER_POKEGEAR      ,
    FANFARE_HG_OBTAIN_BERRY           ,
    FANFARE_HG_RECEIVE_POKEMON        ,
    FANFARE_HG_MOVE_DELETED           ,
    FANFARE_HG_THIRD_PLACE            ,
    FANFARE_HG_SECOND_PLACE           ,
    FANFARE_HG_FIRST_PLACE            ,
    FANFARE_HG_POKEATHLON_NEW         ,
    FANFARE_HG_WINNING_POKEATHLON     ,
    FANFARE_HG_OBTAIN_B_POINTS        ,
    FANFARE_HG_OBTAIN_ARCADE_POINTS   ,
    FANFARE_HG_OBTAIN_CASTLE_POINTS   ,
    FANFARE_HG_CLEAR_MINIGAME         ,
    FANFARE_HG_PARTNER               
};

// lazy duplication. i do not care.
int IsExpansionMusic(u16 songNum) {
    int i;
    for(i = 0; i < ARRAY_COUNT(gExpansionMusicTracks); i++) {
        if (gExpansionMusicTracks[i] == songNum)
            return 1;
    }
    return 0;
}

int IsExpansionFanfare(u16 songNum) {
    int i;
    for(i = 0; i < ARRAY_COUNT(gExpansionFanfareTracks); i++) {
        if (gExpansionFanfareTracks[i] == songNum)
            return 1;
    }
    return 0;
}

void SetShuffledMusicSEArrays(void) {
    int i, j, musicCount, fanfareCount;

    // only shuffle music if shuffling music is NOT disabled (on or expanded).
    if(CheckSpeedchoiceOption(SHUFFLE_MUSIC, SHUFFLE_MUSIC_OFF) == FALSE) {
        // We need the same array every time, or else we get different arrays resetting; use the ROM's
        // check value as set by the randomizer.
        SeedRng((u16)gRandomizerCheckValue);

        // initialize the gShuffledMusic array to shuffle.
        for(i = 0, j = 0; i < (SONGS_END - SONGS_START); i++) {
            // only add it to the array if its not in sFanfares; IF we are using the "ON" option and not
            // expanded, if it is NOT in the list of expansion tracks.
            if(!IsInFanfares(SONGS_START + i)) {
                if(CheckSpeedchoiceOption(SHUFFLE_MUSIC, SHUFFLE_MUSIC_ON) == TRUE && IsExpansionMusic(SONGS_START + i))
                    continue; // we ignore expansion music. continue the loop.
                gShuffledMusic[j][0] = SONGS_START + i;   // we are adding the song ID, so we need to use i.
                gShuffledMusic[j++][1] = SONGS_START + i; // same here.
            }
        }

        // add VS Wild Night
        gShuffledMusic[j][0] = MUS_VS_WILD_NIGHT;
        gShuffledMusic[j][1] = MUS_VS_WILD_NIGHT;
        
        // the final j count is the number of music tracks added to the array, plus 1 to account
        // for VS Wild Night.
        musicCount = j+1;
        
        // add terminator
        gShuffledMusic[j+1][0] = 0xFFFF;
        gShuffledMusic[j+1][1] = 0xFFFF;

        // initialize gShuffledFanfares too.
        for(i = 0, j = 0; i < SFANFARES_COUNT; i++) {
            // also check if its in the list if expansion tracks.
            if(CheckSpeedchoiceOption(SHUFFLE_MUSIC, SHUFFLE_MUSIC_ON) == TRUE && IsExpansionFanfare(i))
                continue; // we ignore expansion fanfare too. continue the loop.
            gShuffledFanfares[j][0] = sFanfares[j].songNum;
            gShuffledFanfares[j][1] = sFanfares[j].songNum;
            j++;
        }
        
        // same as before, but for Fanfare count.
        fanfareCount = j;
        
        // add terminator here too.
        gShuffledFanfares[j+1][0] = 0xFFFF;
        gShuffledFanfares[j+1][1] = 0xFFFF;

        // do the first shuffle for gShuffledMusic, but count up to musicCount, because we ONLY want to shuffle
        // pre-terminator elements.
        for(i = 0; i < musicCount; i++) {
            int j = Random() % musicCount; // pick a random element to swap with on the [1] element.

            // perform the swap.
            int swap = gShuffledMusic[j][1];
            gShuffledMusic[j][1] = gShuffledMusic[i][1];
            gShuffledMusic[i][1] = swap;
        }

        // now for gShuffledFanfares, and ignore the terminator too, so dont add +1.
        for(i = 0; i < fanfareCount; i++) {
            int j = Random() % fanfareCount;

            // perform the swap.
            int swap = gShuffledFanfares[j][1];
            gShuffledFanfares[j][1] = gShuffledFanfares[i][1];
            gShuffledFanfares[i][1] = swap;
        }
    }
}

void LoadObjectEvents(void)
{
    int i, j;

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
        gObjectEvents[i] = gSaveBlock1Ptr->objectEvents[i];
}

void SaveSerializedGame(void)
{
    SavePlayerParty();
    SaveObjectEvents();
    
    // ---------------------
    // SPEEDCHOICE CHANGE
    // ---------------------
    // Save the timers here.
    gSaveBlock1Ptr->doneButtonStats.frameCount = gFrameTimers.frameCount;
    gSaveBlock1Ptr->doneButtonStats.owFrameCount = gFrameTimers.owFrameCount;
    gSaveBlock1Ptr->doneButtonStats.battleFrameCount = gFrameTimers.battleFrameCount;
    gSaveBlock1Ptr->doneButtonStats.menuFrameCount = gFrameTimers.menuFrameCount;
    gSaveBlock1Ptr->doneButtonStats.introsFrameCount = gFrameTimers.introsFrameCount;
}

void LoadSerializedGame(void)
{
    LoadPlayerParty();
    LoadObjectEvents();

    // ---------------------
    // SPEEDCHOICE CHANGE
    // ---------------------
    // Load the timers here. Note we don't overwrite them: we add the counts to the total
    // timers since we want to include the intro counts it took to reach the load.
    gFrameTimers.frameCount += gSaveBlock1Ptr->doneButtonStats.frameCount;
    gFrameTimers.owFrameCount += gSaveBlock1Ptr->doneButtonStats.owFrameCount;
    gFrameTimers.battleFrameCount += gSaveBlock1Ptr->doneButtonStats.battleFrameCount;
    gFrameTimers.menuFrameCount += gSaveBlock1Ptr->doneButtonStats.menuFrameCount;
    gFrameTimers.introsFrameCount += gSaveBlock1Ptr->doneButtonStats.introsFrameCount;
}

void LoadPlayerBag(void)
{
    int i;

    // load player items.
    for (i = 0; i < BAG_ITEMS_COUNT; i++)
        gLoadedSaveData.items[i] = gSaveBlock1Ptr->bagPocket_Items[i];

    // load player key items.
    for (i = 0; i < BAG_KEYITEMS_COUNT; i++)
        gLoadedSaveData.keyItems[i] = gSaveBlock1Ptr->bagPocket_KeyItems[i];

    // load player pokeballs.
    for (i = 0; i < BAG_POKEBALLS_COUNT; i++)
        gLoadedSaveData.pokeBalls[i] = gSaveBlock1Ptr->bagPocket_PokeBalls[i];

    // load player TMs and HMs.
    for (i = 0; i < BAG_TMHM_COUNT; i++)
        gLoadedSaveData.TMsHMs[i] = gSaveBlock1Ptr->bagPocket_TMHM[i];

    // load player berries.
    for (i = 0; i < BAG_BERRIES_COUNT; i++)
        gLoadedSaveData.berries[i] = gSaveBlock1Ptr->bagPocket_Berries[i];

    // load mail.
    for (i = 0; i < MAIL_COUNT; i++)
        gLoadedSaveData.mail[i] = gSaveBlock1Ptr->mail[i];

    gLastEncryptionKey = gSaveBlock2Ptr->encryptionKey;
}

void SavePlayerBag(void)
{
    int i;
    u32 encryptionKeyBackup;

    // save player items.
    for (i = 0; i < BAG_ITEMS_COUNT; i++)
        gSaveBlock1Ptr->bagPocket_Items[i] = gLoadedSaveData.items[i];

    // save player key items.
    for (i = 0; i < BAG_KEYITEMS_COUNT; i++)
        gSaveBlock1Ptr->bagPocket_KeyItems[i] = gLoadedSaveData.keyItems[i];

    // save player pokeballs.
    for (i = 0; i < BAG_POKEBALLS_COUNT; i++)
        gSaveBlock1Ptr->bagPocket_PokeBalls[i] = gLoadedSaveData.pokeBalls[i];

    // save player TMs and HMs.
    for (i = 0; i < BAG_TMHM_COUNT; i++)
        gSaveBlock1Ptr->bagPocket_TMHM[i] = gLoadedSaveData.TMsHMs[i];

    // save player berries.
    for (i = 0; i < BAG_BERRIES_COUNT; i++)
        gSaveBlock1Ptr->bagPocket_Berries[i] = gLoadedSaveData.berries[i];

    // save mail.
    for (i = 0; i < MAIL_COUNT; i++)
        gSaveBlock1Ptr->mail[i] = gLoadedSaveData.mail[i];

    encryptionKeyBackup = gSaveBlock2Ptr->encryptionKey;
    gSaveBlock2Ptr->encryptionKey = gLastEncryptionKey;
    ApplyNewEncryptionKeyToBagItems(encryptionKeyBackup);
    gSaveBlock2Ptr->encryptionKey = encryptionKeyBackup; // updated twice?
}

void ApplyNewEncryptionKeyToHword(u16 *hWord, u32 newKey)
{
    *hWord ^= gSaveBlock2Ptr->encryptionKey;
    *hWord ^= newKey;
}

void ApplyNewEncryptionKeyToWord(u32 *word, u32 newKey)
{
    *word ^= gSaveBlock2Ptr->encryptionKey;
    *word ^= newKey;
}

static void ApplyNewEncryptionKeyToAllEncryptedData(u32 encryptionKey)
{
    ApplyNewEncryptionKeyToGameStats(encryptionKey);
    ApplyNewEncryptionKeyToBagItems_(encryptionKey);
    ApplyNewEncryptionKeyToBerryPowder(encryptionKey);
    ApplyNewEncryptionKeyToWord(&gSaveBlock1Ptr->money, encryptionKey);
    ApplyNewEncryptionKeyToHword(&gSaveBlock1Ptr->coins, encryptionKey);
}
