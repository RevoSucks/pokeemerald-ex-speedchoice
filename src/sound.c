#include "global.h"
#include "gba/m4a_internal.h"
#include "sound.h"
#include "battle.h"
#include "m4a.h"
#include "main.h"
#include "pokemon.h"
#include "constants/songs.h"
#include "constants/sound.h"
#include "task.h"

struct Fanfare
{
    u16 songNum;
    u16 duration;
};

// ewram
EWRAM_DATA struct MusicPlayerInfo* gMPlay_PokemonCry = NULL;
EWRAM_DATA u8 gPokemonCryBGMDuckingCounter = 0;

// iwram bss
static u16 sCurrentMapMusic;
static u16 sNextMapMusic;
static u8 sMapMusicState;
static u8 sMapMusicFadeInSpeed;
static u16 sFanfareCounter;

// iwram common
bool8 gDisableMusic;

extern struct MusicPlayerInfo gMPlayInfo_BGM;
extern struct MusicPlayerInfo gMPlayInfo_SE1;
extern struct MusicPlayerInfo gMPlayInfo_SE2;
extern struct MusicPlayerInfo gMPlayInfo_SE3;
extern struct ToneData gCryTable[];
extern struct ToneData gCryTable_Reverse[];

static void Task_Fanfare(u8 taskId);
static void CreateFanfareTask(void);
static void Task_DuckBGMForPokemonCry(u8 taskId);
static void RestoreBGMVolumeAfterPokemonCry(void);

const struct Fanfare sFanfares[] = {
    [FANFARE_LEVEL_UP]                 = { MUS_LEVEL_UP                ,  80 },
    [FANFARE_HEAL]                     = { MUS_HEAL                    , 160 },
    [FANFARE_OBTAIN_BADGE]             = { MUS_OBTAIN_BADGE            , 340 },
    [FANFARE_OBTAIN_ITEM]              = { MUS_OBTAIN_ITEM             , 160 },
    [FANFARE_EVOLVED]                  = { MUS_EVOLVED                 , 220 },
    [FANFARE_OBTAIN_TMHM]              = { MUS_OBTAIN_TMHM             , 220 },
    [FANFARE_EVOLUTION_INTRO]          = { MUS_EVOLUTION_INTRO         ,  60 },
    [FANFARE_MOVE_DELETED]             = { MUS_MOVE_DELETED            , 180 },
    [FANFARE_OBTAIN_BERRY]             = { MUS_OBTAIN_BERRY            , 120 },
    [FANFARE_AWAKEN_LEGEND]            = { MUS_AWAKEN_LEGEND           , 710 },
    [FANFARE_SLOTS_JACKPOT]            = { MUS_SLOTS_JACKPOT           , 250 },
    [FANFARE_SLOTS_WIN]                = { MUS_SLOTS_WIN               , 150 },
    [FANFARE_TOO_BAD]                  = { MUS_TOO_BAD                 , 160 },
    [FANFARE_RG_JIGGLYPUFF]            = { MUS_RG_JIGGLYPUFF           , 400 },
    [FANFARE_RG_DEX_RATING]            = { MUS_RG_DEX_RATING           , 196 },
    [FANFARE_RG_OBTAIN_KEY_ITEM]       = { MUS_RG_OBTAIN_KEY_ITEM      , 170 },
    [FANFARE_RG_CAUGHT_INTRO]          = { MUS_RG_CAUGHT_INTRO         , 231 },
    [FANFARE_RG_PHOTO]                 = { MUS_RG_PHOTO                ,  90 },
    [FANFARE_RG_POKE_FLUTE]            = { MUS_RG_POKE_FLUTE           , 450 },
    [FANFARE_OBTAIN_B_POINTS]          = { MUS_OBTAIN_B_POINTS         , 313 },
    [FANFARE_REGISTER_MATCH_CALL]      = { MUS_REGISTER_MATCH_CALL     , 135 },
    [FANFARE_OBTAIN_SYMBOL]            = { MUS_OBTAIN_SYMBOL           , 318 },
    [FANFARE_DP_TV_END]                = { MUS_DP_TV_END               , 244 },
    [FANFARE_DP_OBTAIN_ITEM]           = { MUS_DP_OBTAIN_ITEM          , 160 },
    [FANFARE_DP_HEAL]                  = { MUS_DP_HEAL                 , 160 },
    [FANFARE_DP_OBTAIN_KEY_ITEM]       = { MUS_DP_OBTAIN_KEY_ITEM      , 170 },
    [FANFARE_DP_OBTAIN_TMHM]           = { MUS_DP_OBTAIN_TMHM          , 220 },
    [FANFARE_DP_OBTAIN_BADGE]          = { MUS_DP_OBTAIN_BADGE         , 340 },
    [FANFARE_DP_LEVEL_UP]              = { MUS_DP_LEVEL_UP             ,  80 },
    [FANFARE_DP_OBTAIN_BERRY]          = { MUS_DP_OBTAIN_BERRY         , 120 },
    [FANFARE_DP_PARTNER]               = { MUS_DP_LETS_GO_TOGETHER     , 180 },
    [FANFARE_DP_EVOLVED]               = { MUS_DP_EVOLVED              , 252 },
    [FANFARE_DP_POKETCH]               = { MUS_DP_POKETCH              , 120 },
    [FANFARE_DP_MOVE_DELETED]          = { MUS_DP_MOVE_DELETED         , 180 },
    [FANFARE_DP_ACCESSORY]             = { MUS_DP_OBTAIN_ACCESSORY     , 160 },
    [FANFARE_PL_TV_END]                = { MUS_PL_TV_END               , 230 },
    [FANFARE_PL_CLEAR_MINIGAME]        = { MUS_PL_WIN_MINIGAME         , 230 },
    [FANFARE_PL_OBTAIN_ARCADE_POINTS]  = { MUS_PL_OBTAIN_ARCADE_POINTS , 175 },
    [FANFARE_PL_OBTAIN_CASTLE_POINTS]  = { MUS_PL_OBTAIN_CASTLE_POINTS , 200 },
    [FANFARE_PL_OBTAIN_B_POINTS]       = { MUS_PL_OBTAIN_B_POINTS      , 264 },
    [FANFARE_HG_OBTAIN_KEY_ITEM]       = { MUS_HG_OBTAIN_KEY_ITEM      , 170 },
    [FANFARE_HG_LEVEL_UP]              = { MUS_HG_LEVEL_UP             ,  80 },
    [FANFARE_HG_HEAL]                  = { MUS_HG_HEAL                 , 160 },
    [FANFARE_HG_DEX_RATING_1]          = { MUS_HG_DEX_RATING_1         , 200 },
    [FANFARE_HG_DEX_RATING_2]          = { MUS_HG_DEX_RATING_2         , 180 },
    [FANFARE_HG_DEX_RATING_3]          = { MUS_HG_DEX_RATING_3         , 220 },
    [FANFARE_HG_DEX_RATING_4]          = { MUS_HG_DEX_RATING_4         , 210 },
    [FANFARE_HG_DEX_RATING_5]          = { MUS_HG_DEX_RATING_5         , 210 },
    [FANFARE_HG_DEX_RATING_6]          = { MUS_HG_DEX_RATING_6         , 370 },
    [FANFARE_HG_RECEIVE_EGG]           = { MUS_HG_OBTAIN_EGG           , 155 },
    [FANFARE_HG_OBTAIN_ITEM]           = { MUS_HG_OBTAIN_ITEM          , 160 },
    [FANFARE_HG_EVOLVED]               = { MUS_HG_EVOLVED              , 240 },
    [FANFARE_HG_OBTAIN_BADGE]          = { MUS_HG_OBTAIN_BADGE         , 340 },
    [FANFARE_HG_OBTAIN_TMHM]           = { MUS_HG_OBTAIN_TMHM          , 220 },
    [FANFARE_HG_VOLTORB_FLIP_1]        = { MUS_HG_CARD_FLIP            , 195 },
    [FANFARE_HG_VOLTORB_FLIP_2]        = { MUS_HG_CARD_FLIP_GAME_OVER  , 240 },
    [FANFARE_HG_ACCESSORY]             = { MUS_HG_OBTAIN_ACCESSORY     , 160 },
    [FANFARE_HG_REGISTER_POKEGEAR]     = { MUS_HG_POKEGEAR_REGISTERED  , 185 },
    [FANFARE_HG_OBTAIN_BERRY]          = { MUS_HG_OBTAIN_BERRY         , 120 },
    [FANFARE_HG_RECEIVE_POKEMON]       = { MUS_HG_RECEIVE_POKEMON      , 150 },
    [FANFARE_HG_MOVE_DELETED]          = { MUS_HG_MOVE_DELETED         , 180 },
    [FANFARE_HG_THIRD_PLACE]           = { MUS_HG_BUG_CONTEST_3RD_PLACE, 130 },
    [FANFARE_HG_SECOND_PLACE]          = { MUS_HG_BUG_CONTEST_2ND_PLACE, 225 },
    [FANFARE_HG_FIRST_PLACE]           = { MUS_HG_BUG_CONTEST_1ST_PLACE, 250 },
    [FANFARE_HG_POKEATHLON_NEW]        = { MUS_HG_POKEATHLON_READY     , 110 },
    [FANFARE_HG_WINNING_POKEATHLON]    = { MUS_HG_POKEATHLON_1ST_PLACE , 144 },
    [FANFARE_HG_OBTAIN_B_POINTS]       = { MUS_HG_OBTAIN_B_POINTS      , 264 },
    [FANFARE_HG_OBTAIN_ARCADE_POINTS]  = { MUS_HG_OBTAIN_ARCADE_POINTS , 175 },
    [FANFARE_HG_OBTAIN_CASTLE_POINTS]  = { MUS_HG_OBTAIN_CASTLE_POINTS , 200 },
    [FANFARE_HG_CLEAR_MINIGAME]        = { MUS_HG_WIN_MINIGAME         , 230 },
    [FANFARE_HG_PARTNER]               = { MUS_HG_LETS_GO_TOGETHER     , 180 },
    // Adding cable car fanfare and FR/LG beta heal music for rando purposes.
    [FANFARE_MISC_CABLE_CAR]           = { MUS_CABLE_CAR               , 450 },
    [FANFARE_MISC_RG_HEAL]             = { MUS_RG_HEAL                 , 160 },
    [FANFARE_DP_CAUGHT_INTRO]          = { MUS_DP_CAUGHT_INTRO         , 231 },
    [FANFARE_DP_DEX_RATING]            = { MUS_DP_DEX_RATING           , 185 },
};

#define CRY_VOLUME  120 // was 125 in R/S

int IsInFanfares(u16 songNum) {
    int i;
    
    for(i = 0; i < ARRAY_COUNT(sFanfares); i++) {
        if (sFanfares[i].songNum == songNum)
            return TRUE;
    }
    return FALSE;
}

void InitMapMusic(void)
{
    gDisableMusic = FALSE;
    ResetMapMusic();
}

void MapMusicMain(void)
{
    switch (sMapMusicState)
    {
    case 0:
        break;
    case 1:
        sMapMusicState = 2;
        PlayBGM(sCurrentMapMusic);
        break;
    case 2:
    case 3:
    case 4:
        break;
    case 5:
        if (IsBGMStopped())
        {
            sNextMapMusic = 0;
            sMapMusicState = 0;
        }
        break;
    case 6:
        if (IsBGMStopped() && IsFanfareTaskInactive())
        {
            sCurrentMapMusic = sNextMapMusic;
            sNextMapMusic = 0;
            sMapMusicState = 2;
            PlayBGM(sCurrentMapMusic);
        }
        break;
    case 7:
        if (IsBGMStopped() && IsFanfareTaskInactive())
        {
            FadeInNewBGM(sNextMapMusic, sMapMusicFadeInSpeed);
            sCurrentMapMusic = sNextMapMusic;
            sNextMapMusic = 0;
            sMapMusicState = 2;
            sMapMusicFadeInSpeed = 0;
        }
        break;
    }
}

void ResetMapMusic(void)
{
    sCurrentMapMusic = 0;
    sNextMapMusic = 0;
    sMapMusicState = 0;
    sMapMusicFadeInSpeed = 0;
}

u16 GetCurrentMapMusic(void)
{
    return sCurrentMapMusic;
}

void PlayNewMapMusic(u16 songNum)
{
    sCurrentMapMusic = songNum;
    sNextMapMusic = 0;
    sMapMusicState = 1;
}

void StopMapMusic(void)
{
    sCurrentMapMusic = 0;
    sNextMapMusic = 0;
    sMapMusicState = 1;
}

void FadeOutMapMusic(u8 speed)
{
    if (IsNotWaitingForBGMStop())
        FadeOutBGM(speed);
    sCurrentMapMusic = 0;
    sNextMapMusic = 0;
    sMapMusicState = 5;
}

void FadeOutAndPlayNewMapMusic(u16 songNum, u8 speed)
{
    FadeOutMapMusic(speed);
    sCurrentMapMusic = 0;
    sNextMapMusic = songNum;
    sMapMusicState = 6;
}

void FadeOutAndFadeInNewMapMusic(u16 songNum, u8 fadeOutSpeed, u8 fadeInSpeed)
{
    FadeOutMapMusic(fadeOutSpeed);
    sCurrentMapMusic = 0;
    sNextMapMusic = songNum;
    sMapMusicState = 7;
    sMapMusicFadeInSpeed = fadeInSpeed;
}

void FadeInNewMapMusic(u16 songNum, u8 speed)
{
    FadeInNewBGM(songNum, speed);
    sCurrentMapMusic = songNum;
    sNextMapMusic = 0;
    sMapMusicState = 2;
    sMapMusicFadeInSpeed = 0;
}

bool8 IsNotWaitingForBGMStop(void)
{
    if (sMapMusicState == 6)
        return FALSE;
    if (sMapMusicState == 5)
        return FALSE;
    if (sMapMusicState == 7)
        return FALSE;
    return TRUE;
}

extern EWRAM_DATA u16 gShuffledFanfares[][2];

// take the unrandomized song num fanfare, find the new fanfare, and then
// get the duration.
u16 GetNewFanfareDuration(u16 songNum) {
    int i = 0;
    // find the entry of the fanfare via song ID in the shuffled array.
    while (gShuffledFanfares[i][0] != 0xFFFF) {
        // if there is a match, then take the randomized value at [1] and do
        // another lookup via sFanfares.
        if(songNum == gShuffledFanfares[i][0]) {
            int j;
            // found a match, look it up in the fanfare array.
            for(j = 0; j < ARRAY_COUNT(sFanfares); j++) {
                if(gShuffledFanfares[i][1] == sFanfares[j].songNum) {
                    return sFanfares[j].duration;
                }
            }
        }
        i++;
    }
    return 100; // not found. uhhh.. default to 100 duration? Reaching this shouldn't be possible.
}

void PlayFanfareByFanfareNum(u8 fanfareNum)
{
    u16 songNum;
    m4aMPlayStop(&gMPlayInfo_BGM);
    songNum = sFanfares[fanfareNum].songNum;
    sFanfareCounter = GetNewFanfareDuration(songNum);
    sFanfareCounter = (sFanfareCounter < 400) ? sFanfareCounter : 400;
    //sFanfareCounter = sFanfares[fanfareNum].duration;
    m4aSongNumStart(songNum);
}

bool8 WaitFanfare(bool8 stop)
{
    if (sFanfareCounter)
    {
        sFanfareCounter--;
        return FALSE;
    }
    else
    {
        if (!stop)
            m4aMPlayContinue(&gMPlayInfo_BGM);
        else
            m4aSongNumStart(MUS_DUMMY);

        return TRUE;
    }
}

// Unused
void StopFanfareByFanfareNum(u8 fanfareNum)
{
    m4aSongNumStop(sFanfares[fanfareNum].songNum);
}

void PlayFanfare(u16 songNum)
{
    s32 i;
    for (i = 0; (u32)i < ARRAY_COUNT(sFanfares); i++)
    {
        if (sFanfares[i].songNum == songNum)
        {
            PlayFanfareByFanfareNum(i);
            CreateFanfareTask();
            return;
        }
    }

    // songNum is not in sFanfares
    // Play first fanfare in table instead
    PlayFanfareByFanfareNum(0);
    CreateFanfareTask();
}

bool8 IsFanfareTaskInactive(void)
{
    if (FuncIsActiveTask(Task_Fanfare) == TRUE)
        return FALSE;
    return TRUE;
}

static void Task_Fanfare(u8 taskId)
{
    if (sFanfareCounter)
    {
        sFanfareCounter--;
    }
    else
    {
        m4aMPlayContinue(&gMPlayInfo_BGM);
        DestroyTask(taskId);
    }
}

static void CreateFanfareTask(void)
{
    if (FuncIsActiveTask(Task_Fanfare) != TRUE)
        CreateTask(Task_Fanfare, 80);
}

void FadeInNewBGM(u16 songNum, u8 speed)
{
    if (gDisableMusic)
        songNum = 0;
    if (songNum == MUS_NONE)
        songNum = 0;
    m4aSongNumStart(songNum);
    m4aMPlayImmInit(&gMPlayInfo_BGM);
    m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 0);
    m4aSongNumStop(songNum);
    m4aMPlayFadeIn(&gMPlayInfo_BGM, speed);
}

void FadeOutBGMTemporarily(u8 speed)
{
    m4aMPlayFadeOutTemporarily(&gMPlayInfo_BGM, speed);
}

bool8 IsBGMPausedOrStopped(void)
{
    if (gMPlayInfo_BGM.status & MUSICPLAYER_STATUS_PAUSE)
        return TRUE;
    if (!(gMPlayInfo_BGM.status & MUSICPLAYER_STATUS_TRACK))
        return TRUE;
    return FALSE;
}

void FadeInBGM(u8 speed)
{
    m4aMPlayFadeIn(&gMPlayInfo_BGM, speed);
}

void FadeOutBGM(u8 speed)
{
    m4aMPlayFadeOut(&gMPlayInfo_BGM, speed);
}

bool8 IsBGMStopped(void)
{
    if (!(gMPlayInfo_BGM.status & MUSICPLAYER_STATUS_TRACK))
        return TRUE;
    return FALSE;
}

void PlayCry1(u16 species, s8 pan)
{
    m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 85);
    PlayCryInternal(species, pan, CRY_VOLUME, 10, 0);
    gPokemonCryBGMDuckingCounter = 2;
    RestoreBGMVolumeAfterPokemonCry();
}

void PlayCry2(u16 species, s8 pan, s8 volume, u8 priority)
{
    PlayCryInternal(species, pan, volume, priority, 0);
}

void PlayCry3(u16 species, s8 pan, u8 mode)
{
    if (mode == 1)
    {
        PlayCryInternal(species, pan, CRY_VOLUME, 10, 1);
    }
    else
    {
        m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 85);
        PlayCryInternal(species, pan, CRY_VOLUME, 10, mode);
        gPokemonCryBGMDuckingCounter = 2;
        RestoreBGMVolumeAfterPokemonCry();
    }
}

void PlayCry4(u16 species, s8 pan, u8 mode)
{
    if (mode == 1)
    {
        PlayCryInternal(species, pan, CRY_VOLUME, 10, 1);
    }
    else
    {
        if (!(gBattleTypeFlags & BATTLE_TYPE_MULTI))
            m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 85);
        PlayCryInternal(species, pan, CRY_VOLUME, 10, mode);
    }
}

void PlayCry6(u16 species, s8 pan, u8 mode) // not present in R/S
{
    if (mode == 1)
    {
        PlayCryInternal(species, pan, CRY_VOLUME, 10, 1);
    }
    else
    {
        m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 85);
        PlayCryInternal(species, pan, CRY_VOLUME, 10, mode);
        gPokemonCryBGMDuckingCounter = 2;
    }
}

void PlayCry5(u16 species, u8 mode)
{
    m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 85);
    PlayCryInternal(species, 0, CRY_VOLUME, 10, mode);
    gPokemonCryBGMDuckingCounter = 2;
    RestoreBGMVolumeAfterPokemonCry();
}

void PlayCryInternal(u16 species, s8 pan, s8 volume, u8 priority, u8 mode)
{
    bool32 v0;
    u32 release;
    u32 length;
    u32 pitch;
    u32 chorus;

    length = 140;
    v0 = FALSE;
    release = 0;
    pitch = 15360;
    chorus = 0;

    switch (mode)
    {
    case 0:
        break;
    case 1:
        length = 20;
        release = 225;
        break;
    case 2:
        release = 225;
        pitch = 15600;
        chorus = 20;
        volume = 90;
        break;
    case 3:
        length = 50;
        release = 200;
        pitch = 15800;
        chorus = 20;
        volume = 90;
        break;
    case 4:
        length = 25;
        v0 = TRUE;
        release = 100;
        pitch = 15600;
        chorus = 192;
        volume = 90;
        break;
    case 5:
        release = 200;
        pitch = 14440;
        break;
    case 6:
        release = 220;
        pitch = 15555;
        chorus = 192;
        volume = 70;
        break;
    case 7:
        length = 10;
        release = 100;
        pitch = 14848;
        break;
    case 8:
        length = 60;
        release = 225;
        pitch = 15616;
        break;
    case 9:
        length = 15;
        v0 = TRUE;
        release = 125;
        pitch = 15200;
        break;
    case 10:
        length = 100;
        release = 225;
        pitch = 15200;
        break;
    case 12:
        length = 20;
        release = 225;
    case 11:
        pitch = 15000;
        break;
    }

    SetPokemonCryVolume(volume);
    SetPokemonCryPanpot(pan);
    SetPokemonCryPitch(pitch);
    SetPokemonCryLength(length);
    SetPokemonCryProgress(0);
    SetPokemonCryRelease(release);
    SetPokemonCryChorus(chorus);
    SetPokemonCryPriority(priority);

    species--;
    gMPlay_PokemonCry = SetPokemonCryTone(v0 ? &gCryTable_Reverse[species] : &gCryTable[species]);
}

bool8 IsCryFinished(void)
{
    if (FuncIsActiveTask(Task_DuckBGMForPokemonCry) == TRUE)
    {
        return FALSE;
    }
    else
    {
        ClearPokemonCrySongs();
        return TRUE;
    }
}

void StopCryAndClearCrySongs(void)
{
    m4aMPlayStop(gMPlay_PokemonCry);
    ClearPokemonCrySongs();
}

void StopCry(void)
{
    m4aMPlayStop(gMPlay_PokemonCry);
}

bool8 IsCryPlayingOrClearCrySongs(void)
{
    if (IsPokemonCryPlaying(gMPlay_PokemonCry))
    {
        return TRUE;
    }
    else
    {
        ClearPokemonCrySongs();
        return FALSE;
    }
}

bool8 IsCryPlaying(void)
{
    if (IsPokemonCryPlaying(gMPlay_PokemonCry))
        return TRUE;
    else
        return FALSE;
}

static void Task_DuckBGMForPokemonCry(u8 taskId)
{
    if (gPokemonCryBGMDuckingCounter)
    {
        gPokemonCryBGMDuckingCounter--;
        return;
    }

    if (!IsPokemonCryPlaying(gMPlay_PokemonCry))
    {
        m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 256);
        DestroyTask(taskId);
    }
}

static void RestoreBGMVolumeAfterPokemonCry(void)
{
    if (FuncIsActiveTask(Task_DuckBGMForPokemonCry) != TRUE)
        CreateTask(Task_DuckBGMForPokemonCry, 80);
}

void PlayBGM(u16 songNum)
{
    if (gDisableMusic)
        songNum = 0;
    if (songNum == MUS_NONE)
        songNum = 0;
    m4aSongNumStart(songNum);
}

void PlaySE(u16 songNum)
{
    m4aSongNumStart(songNum);
}

void PlaySE12WithPanning(u16 songNum, s8 pan)
{
    m4aSongNumStart(songNum);
    m4aMPlayImmInit(&gMPlayInfo_SE1);
    m4aMPlayImmInit(&gMPlayInfo_SE2);
    m4aMPlayPanpotControl(&gMPlayInfo_SE1, 0xFFFF, pan);
    m4aMPlayPanpotControl(&gMPlayInfo_SE2, 0xFFFF, pan);
}

void PlaySE1WithPanning(u16 songNum, s8 pan)
{
    m4aSongNumStart(songNum);
    m4aMPlayImmInit(&gMPlayInfo_SE1);
    m4aMPlayPanpotControl(&gMPlayInfo_SE1, 0xFFFF, pan);
}

void PlaySE2WithPanning(u16 songNum, s8 pan)
{
    m4aSongNumStart(songNum);
    m4aMPlayImmInit(&gMPlayInfo_SE2);
    m4aMPlayPanpotControl(&gMPlayInfo_SE2, 0xFFFF, pan);
}

void SE12PanpotControl(s8 pan)
{
    m4aMPlayPanpotControl(&gMPlayInfo_SE1, 0xFFFF, pan);
    m4aMPlayPanpotControl(&gMPlayInfo_SE2, 0xFFFF, pan);
}

bool8 IsSEPlaying(void)
{
    if ((gMPlayInfo_SE1.status & MUSICPLAYER_STATUS_PAUSE) && (gMPlayInfo_SE2.status & MUSICPLAYER_STATUS_PAUSE))
        return FALSE;
    if (!(gMPlayInfo_SE1.status & MUSICPLAYER_STATUS_TRACK) && !(gMPlayInfo_SE2.status & MUSICPLAYER_STATUS_TRACK))
        return FALSE;
    return TRUE;
}

bool8 IsBGMPlaying(void)
{
    if (gMPlayInfo_BGM.status & MUSICPLAYER_STATUS_PAUSE)
        return FALSE;
    if (!(gMPlayInfo_BGM.status & MUSICPLAYER_STATUS_TRACK))
        return FALSE;
    return TRUE;
}

bool8 IsSpecialSEPlaying(void)
{
    if (gMPlayInfo_SE3.status & MUSICPLAYER_STATUS_PAUSE)
        return FALSE;
    if (!(gMPlayInfo_SE3.status & MUSICPLAYER_STATUS_TRACK))
        return FALSE;
    return TRUE;
}
