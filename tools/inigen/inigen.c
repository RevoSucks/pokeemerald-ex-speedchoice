#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <capstone/capstone.h>
#include "global.h"
#include "elf.h"
#include "util.h"

// Get constants from the repository
#include "constants/global.h"
#include "constants/species.h"
#include "constants/trainers.h"
#include "constants/items.h"
#include "constants/abilities.h"
#include "constants/moves.h"
#include "constants/pokemon.h"
#include "constants/event_objects.h"

/*
 * ---------------------------------------------------------
 * Type definitions
 * ---------------------------------------------------------
 */

#define len(arr) (sizeof(arr)/sizeof(*arr))

//struct TMText {
//    int tmno;
//    int mapgp;
//    int mapno;
//    int scriptno;
//    int offset;
//    const char * text;
//};

struct TMText {
    int tmno;
    const char * label;
    const char * text;
};

struct StaticPokemon {
    const char * label;
    int offset;
};

static csh sCapstone;

static Elf32_Shdr * sh_text;

/*
 * ---------------------------------------------------------
 * Data
 * ---------------------------------------------------------
 */

static const char * gTypeNames[] = {
    "Normal",
    "Fighting",
    "Flying",
    "Poison",
    "Ground",
    "Rock",
    "Grass", // "Bug",
    "Ghost",
    "Steel",
    "Ghost", // "Mystery",
    "Fire",
    "Water",
    "Grass",
    "Electric",
    "Psychic",
    "Ice",
    "Dragon",
    "Dark",
	"Fairy"
};

const struct StaticPokemon gStaticPokemon[][8] = {
    {{"RustboroCity_DevonCorp_2F_EventScript_LileepReady", 0x2}, {"RustboroCity_DevonCorp_2F_EventScript_ReceiveLileep", 0x3}, {"RustboroCity_DevonCorp_2F_EventScript_ReceiveLileep", 0x6}, {"RustboroCity_DevonCorp_2F_EventScript_ReceivedLileepFanfare", 0x2}, {"RustboroCity_DevonCorp_2F_EventScript_ReceivedLileepFanfare", 0x10}},
    {{"RustboroCity_DevonCorp_2F_EventScript_AnorithReady", 0x2}, {"RustboroCity_DevonCorp_2F_EventScript_ReceiveAnorith", 0x3}, {"RustboroCity_DevonCorp_2F_EventScript_ReceiveAnorith", 0x6}, {"RustboroCity_DevonCorp_2F_EventScript_ReceivedAnorithFanfare", 0x2}, {"RustboroCity_DevonCorp_2F_EventScript_ReceivedAnorithFanfare", 0x10}},
    {{"MarineCave_End_EventScript_Kyogre", 0x17}, {"MarineCave_End_EventScript_Kyogre", 0x25}, {"MarineCave_End_EventScript_RanFromKyogre", 0x3}},
    {{"TerraCave_End_EventScript_Groudon", 0x17}, {"TerraCave_End_EventScript_Groudon", 0x25}, {"TerraCave_End_EventScript_RanFromGroudon", 0x3}},
    {{"DesertRuins_EventScript_Regirock", 0x4}, {"DesertRuins_EventScript_Regirock", 0xD}, {"DesertRuins_EventScript_RanFromRegirock", 0x3}},
    {{"IslandCave_EventScript_Regice", 0x4}, {"IslandCave_EventScript_Regice", 0xD}, {"IslandCave_EventScript_RanFromRegice", 0x3}},
    {{"AncientTomb_EventScript_Registeel", 0x4}, {"AncientTomb_EventScript_Registeel", 0xD}, {"AncientTomb_EventScript_RanFromRegisteel", 0x3}},
    {{"SkyPillar_Top_EventScript_Rayquaza", 0x3}, {"SkyPillar_Top_EventScript_Rayquaza", 0xC}, {"SkyPillar_Top_EventScript_RanFromRayquaza", 0x3}},
    {{"EventScript_BattleKecleon", 0x1F}, {"EventScript_BattleKecleon", 0x28}},
    {{"Route120_EventScript_StevenBattleKecleon", 0x4B}, {"Route120_EventScript_StevenBattleKecleon", 0x54}},
    {{"NewMauville_Inside_EventScript_Voltorb1", 0x3}, {"NewMauville_Inside_EventScript_Voltorb1", 0xA}},
    {{"NewMauville_Inside_EventScript_Voltorb2", 0x3}, {"NewMauville_Inside_EventScript_Voltorb2", 0xA}},
    {{"NewMauville_Inside_EventScript_Voltorb3", 0x3}, {"NewMauville_Inside_EventScript_Voltorb3", 0xA}},
    {{"AquaHideout_B1F_EventScript_Electrode1", 0x3}, {"AquaHideout_B1F_EventScript_Electrode1", 0xA}},
    {{"AquaHideout_B1F_EventScript_Electrode2", 0x3}, {"AquaHideout_B1F_EventScript_Electrode2", 0xA}},
    {{"BattleFrontier_OutsideEast_EventScript_WaterSudowoodo", 0x1F}, {"BattleFrontier_OutsideEast_EventScript_WaterSudowoodo", 0x2D}},
    {{"SouthernIsland_Interior_EventScript_SetLatiosBattleVars", 0x3}},
    {{"SouthernIsland_Interior_EventScript_SetLatiasBattleVars", 0x3}},
    {{"BirthIsland_Exterior_EventScript_Deoxys", 0x26}, {"BirthIsland_Exterior_EventScript_Deoxys", 0x36}, {"BirthIsland_Exterior_EventScript_DefeatedDeoxys", 0x6}, {"BirthIsland_Exterior_EventScript_RanFromDeoxys", 0x3}},
    {{"FarawayIsland_Interior_EventScript_Mew", 0x1B}, {"FarawayIsland_Interior_EventScript_Mew", 0x55}, {"FarawayIsland_Interior_EventScript_MewDefeated", 0x6}, {"FarawayIsland_Interior_EventScript_PlayerOrMewRan", 0x3}},
    {{"NavelRock_Top_EventScript_HoOh", 0x30}, {"NavelRock_Top_EventScript_HoOh", 0x5F}, {"NavelRock_Top_EventScript_DefeatedHoOh", 0x6}, {"NavelRock_Top_EventScript_RanFromHoOh", 0x3}},
    {{"NavelRock_Bottom_EventScript_Lugia", 0x45}, {"NavelRock_Bottom_EventScript_Lugia", 0x50}, {"NavelRock_Bottom_EventScript_DefeatedLugia", 0x6}, {"NavelRock_Bottom_EventScript_RanFromLugia", 0x3}},
    {{"MossdeepCity_StevensHouse_EventScript_GiveBeldum", 0x3}, {"MossdeepCity_StevensHouse_EventScript_GiveBeldum", 0x6}, {"MossdeepCity_StevensHouse_EventScript_ReceivedBeldumFanfare", 0x2}, {"MossdeepCity_StevensHouse_EventScript_ReceivedBeldumFanfare", 0x13}},
    {{"Route119_WeatherInstitute_2F_EventScript_ReceiveCastform", 0xB}, {"Route119_WeatherInstitute_2F_EventScript_ReceiveCastform", 0xE}},
};

const struct TMText gTMTexts[] = {
    {3, "gTMText_1", "The TECHNICAL MACHINE I handed you contains [move].\\p… … … … … …"},
    {4, "gTMText_2", "TATE: That TM04 contains... LIZA: [move]!\\pTATE: It’s a move that’s perfect... LIZA: For any POKéMON!\\p… … … … … …"},
    {5, "gTMText_3", "All my POKéMON does is [move]... No one dares to come near me...\\pSigh... If you would, please take this TM away..."},
    {5, "gTMText_4", "TM05 contains [move]."},
    {8, "gTMText_5", "That TM08 contains [move].\\p… … … … … …"},
    {9, "gTMText_6", "I like filling my mouth with seeds, then spitting them out fast!\\pI like you, so you can have this!\\pUse it on a POKéMON, and it will learn [move].\\pWhat does that have to do with firing seeds? Well, nothing!"},
    {24, "gTMText_7", "WATTSON: Wahahahaha!\\pI knew it, \\v01\\v05! I knew I’d made the right choice asking you!\\pThis is my thanks - a TM containing [move]!\\pGo on, you’ve earned it!"},
    {31, "gTMText_8", "TM31 contains [move]! It’s a move so horrible that I can’t describe it."},
    {34, "gTMText_9", "That TM34 there contains [move]. You can count on it!\\p… … … … … …"},
    {39, "gTMText_10", "That TM39 contains [move].\\pIf you use a TM, it instantly teaches the move to a POKéMON.\\pRemember, a TM can be used only once, so think before you use it."},
    {40, "gTMText_11", "TM40 contains [move].\\p… … … … … …"},
    {41, "gTMText_12", "That’s, like, TM41, you know? Hey, it’s [move], you hearing me?\\pHey, now, you listen here, like, I’m not laying a torment on you!"},
    {42, "gTMText_13", "DAD: TM42 contains [move].\\pIt might be able to turn a bad situation into an advantage."},
    {47, "gTMText_14", "STEVEN: Okay, thank you.\\pYou went through all this trouble to deliver that. I need to thank you.\\pLet me see... I’ll give you this TM.\\pIt contains my favorite move, [move]."},
    {50, "gTMText_15", "That TM50 contains [move]."}
};

const struct TMText gMoveTutorTexts[] = {
    {4, "gMoveTutorText_1", "Sigh…\\pSOOTOPOLIS’s GYM LEADER is really lovably admirable.\\pBut that also means I have many rivals for his attention.\\pHe’s got appeal with a [move]. I couldn’t even catch his eye.\\pPlease, let me teach your POKéMON the move [move]!"},
    {4, "gMoveTutorText_2", "Okay, which POKéMON should I teach [move]?"},
    {15, "gMoveTutorText_3", "I can’t do this anymore!\\pIt’s utterly hopeless!\\pI’m a FIGHTING-type TRAINER, so I can’t win at the MOSSDEEP GYM no matter how hard I try!\\pArgh! Punch! Punch! Punch! Punch! Punch! Punch!\\pWhat, don’t look at me that way! I’m only hitting the ground!\\pOr do you want me to teach your POKéMON [move]?"},
    {15, "gMoveTutorText_4", "I want you to win at the MOSSDEEP GYM using that [move]!"},
    {12, "gMoveTutorText_5", "I don’t intend to be going nowhere fast in the sticks like this forever.\\pYou watch me, I’ll get out to the city and become a huge hit.\\pSeriously, I’m going to cause a huge explosion of popularity!\\pIf you overheard that, I’ll happily teach [move] to your POKéMON!"},
    {12, "gMoveTutorText_6", "Fine! [move] it is! Which POKéMON wants to learn it?"},
    {12, "gMoveTutorText_7", "For a long time, I’ve taught POKéMON how to use [move], but I’ve yet to ignite my own explosion…\\pMaybe it’s because deep down, I would rather stay here…"},
    {29, "gMoveTutorText_8", "There’s a move that is wickedly cool.\\pIt’s called [move].\\nWant me to teach it to a POKéMON?"},
    {8, "gMoveTutorText_9", "I want all sorts of things! But I used up my allowance…\\pWouldn’t it be nice if there were a spell that made money appear when you waggle a finger?\\pIf you want, I can teach your POKéMON the move [move].\\pMoney won’t appear, but your POKéMON will do well in battle. Yes?"},
    {8, "gMoveTutorText_10", "When a POKéMON uses [move], all sorts of nice things happen."},
    {7, "gMoveTutorText_11", "Ah, young one!\\pI am also a young one, but I mimic the styles and speech of the elderly folks of this town.\\pWhat do you say, young one? Would you agree to it if I were to offer to teach the move [move]?"},
    {7, "gMoveTutorText_12", "[move] is a move of great depth.\\pCould you execute it to perfection as well as me…?"},
    {7, "gMoveTutorText_13", "Oh, boo! I wanted to teach [move] to your POKéMON!"},
    {16, "gMoveTutorText_14", "Did you know that you can go from here a long way in that direction without changing direction?\\pI might even be able to roll that way.\\pDo you think your POKéMON will want to roll, too?\\pI can teach one the move [move] if you’d like."},
    {24, "gMoveTutorText_15", "Humph! My wife relies on HIDDEN POWER to stay awake.\\pShe should just take a nap like I do.\\pI can teach your POKéMON how to [move]. Interested?"},
    {24, "gMoveTutorText_16", "I’ve never once gotten my wife’s coin trick right.\\pI would be happy if I got it right even as I teach [move]…"},
    {14, "gMoveTutorText_17", "When I see the wide world from up here on the roof…\\pI think about how nice it would be if there were more than just one me so I could enjoy all sorts of lives.\\pOf course it’s not possible. Giggle…\\pI know! Would you be interested in having a POKéMON learn [move]?"},
    {14, "gMoveTutorText_18", "Giggle… Which POKéMON do you want me to teach [move]?"},
    {14, "gMoveTutorText_19", "Oh, no?\\pA POKéMON can do well in a battle using it, you know."},
    {25, "gMoveTutorText_20", "Heh! My POKéMON totally rules! It’s cooler than any POKéMON!\\pI was lipping off with a swagger in my step like that when the CHAIRMAN chewed me out.\\pThat took the swagger out of my step.\\pIf you’d like, I’ll teach the move [move] to a POKéMON of yours."},
    {25, "gMoveTutorText_21", "All right, which POKéMON wants to learn how to [move]?"},
    {25, "gMoveTutorText_22", "I’ll just praise my POKéMON from now on without the [move]."}
};

/*
 * ----------------------------------------------
 * Capstone callbacks
 * ----------------------------------------------
 */

static int IsLoadingStarterItems(const struct cs_insn * insn)
{
    static int to_return;
    Elf32_Sym *sym = GetSymbolByName("ScriptGiveMon");
    cs_arm_op * ops = insn->detail->arm.operands;
    // mov r2, #0
    if (insn->id == ARM_INS_MOV
     && ops[0].type == ARM_OP_REG
     && ops[0].reg == ARM_REG_R2
     && ops[1].type == ARM_OP_IMM
     && ops[1].imm == 0)
        to_return = insn->address;
    // bl ScriptGiveMon
    else if (insn->id == ARM_INS_BL)
    {
        uint32_t target = ops[0].imm;
        if (target == (sym->st_value & ~1))
            return to_return;
    }
    return -1;
}

static int IsIntroLotadForCry_1(const struct cs_insn * insn)
{
    static int to_return;
    static unsigned tmp_reg, tmp_reg2;
    cs_arm_op * ops = insn->detail->arm.operands;
    // mov rX, SPECIES_LOTAD / 2
    if (insn->id == ARM_INS_MOV
    && ops[0].type == ARM_OP_REG
    && ops[1].type == ARM_OP_IMM
    && ops[1].imm == SPECIES_LOTAD / 2)
    {
        to_return = insn->address;
        tmp_reg = ops[0].reg;
    }
    // lsl rX, rY, 1
    else if (insn->id == ARM_INS_LSL
    && ops[0].type == ARM_OP_REG
    && ops[1].type == ARM_OP_REG
    && ops[1].reg == tmp_reg
    && ops[2].type == ARM_OP_IMM
    && ops[2].imm == 1
    )
        tmp_reg2 = ops[0].reg;
    // str rX, [sp, 16]
    else if (insn->id == ARM_INS_STR
             && ops[0].type == ARM_OP_REG
             && ops[0].reg == tmp_reg2
             && ops[1].type == ARM_OP_MEM
             && !ops[1].subtracted
             && ops[1].mem.base == ARM_REG_SP
             && ops[1].mem.index == ARM_REG_INVALID
             && ops[1].mem.disp == 16)
        return to_return;

    return -1;
}

static int IsIntroLotadForCry_2(const struct cs_insn * insn)
{
    static int to_return;
    static unsigned tmp_reg;
    cs_arm_op * ops = insn->detail->arm.operands;
    // ldr rX, =SPECIES_LOTAD
    if (insn->id == ARM_INS_LDR
        && ops[0].type == ARM_OP_REG
        && ops[1].type == ARM_OP_MEM
        && !ops[1].subtracted
        && ops[1].mem.base == ARM_REG_PC
        && ops[1].mem.index == ARM_REG_INVALID)
    {
        to_return = (insn->address & ~3) + ops[1].mem.disp + 4;
        tmp_reg = ops[0].reg;
    }
    // str rX, [sp, #0x10]
    else if (insn->id == ARM_INS_STR
             && ops[0].type == ARM_OP_REG
             && ops[0].reg == tmp_reg
             && ops[1].type == ARM_OP_MEM
             && !ops[1].subtracted
             && ops[1].mem.base == ARM_REG_SP
             && ops[1].mem.index == ARM_REG_INVALID
             && ops[1].mem.disp == 16)
        return to_return;
    return -1;
}

static int IsIntroLotadForCry(const struct cs_insn * insn)
{
    int retval = IsIntroLotadForCry_1(insn);
    if (retval >= 0)
        return retval;
    return IsIntroLotadForCry_2(insn);
}

static int IsIntroLotadForPic_1(const struct cs_insn * insn)
{
    static int to_return;
    static unsigned tmp_reg;
    cs_arm_op * ops = insn->detail->arm.operands;
    // mov rX, SPECIES_LOTAD / 2
    if (insn->id == ARM_INS_MOV
    && ops[0].type == ARM_OP_REG
    && ops[1].type == ARM_OP_IMM
    && ops[1].imm == SPECIES_LOTAD / 2)
    {
        to_return = insn->address;
        tmp_reg = ops[0].reg;
    }
    // lsl rX, rY, 1
    else if (insn->id == ARM_INS_LSL
    && ops[0].type == ARM_OP_REG
    && ops[1].type == ARM_OP_REG
    && ops[1].reg == tmp_reg
    && ops[2].type == ARM_OP_IMM
    && ops[2].imm == 1
    )
        return to_return;

    return -1;
}

static int IsIntroLotadForPic_2(const struct cs_insn * insn)
{
    cs_arm_op * ops = insn->detail->arm.operands;
    // ldr rX, =SPECIES_LOTAD
    if (insn->id == ARM_INS_LDR
        && ops[0].type == ARM_OP_REG
        && ops[1].type == ARM_OP_MEM
        && !ops[1].subtracted
        && ops[1].mem.base == ARM_REG_PC
        && ops[1].mem.index == ARM_REG_INVALID)
        return (insn->address & ~3) + ops[1].mem.disp + 4;
    return -1;
}

static int IsIntroLotadForPic(const struct cs_insn * insn)
{
    int retval = IsIntroLotadForPic_1(insn);
    if (retval >= 0)
        return retval;
    return IsIntroLotadForPic_2(insn);
}

static int IsRunIndoorsTweakOffset(const struct cs_insn * insn)
{
    cs_arm_op * ops = insn->detail->arm.operands;
    if (insn->id == ARM_INS_AND
        && ops[0].type == ARM_OP_REG
        && ops[1].type == ARM_OP_REG
        && (insn - 1)->id == ARM_INS_MOV
        && (insn - 1)->detail->arm.operands[0].type == ARM_OP_REG
        && (insn - 1)->detail->arm.operands[1].type == ARM_OP_IMM
        && (insn - 1)->detail->arm.operands[0].reg == ops[0].reg
        && (insn - 1)->detail->arm.operands[1].imm == 4)
        return insn->address;
    return -1;
}

static int IsWallyZigzagoon_1(const struct cs_insn * insn)
{
    static int to_return;
    static unsigned tmp_reg;
    cs_arm_op * ops = insn->detail->arm.operands;
    // mov rX, SPECIES_ZIGZAGOON / 2
    if (insn->id == ARM_INS_MOV
        && ops[0].type == ARM_OP_REG
        && ops[1].type == ARM_OP_IMM
        && ops[1].imm == SPECIES_ZIGZAGOON / 2)
    {
        to_return = insn->address;
        tmp_reg = ops[0].reg;
    }
        // lsl rX, rY, 1
    else if (insn->id == ARM_INS_LSL
             && ops[0].type == ARM_OP_REG
             && ops[0].reg == ARM_REG_R1
             && ops[1].type == ARM_OP_REG
             && ops[1].reg == tmp_reg
             && ops[2].type == ARM_OP_IMM
             && ops[2].imm == 1
        )
        return to_return;

    return -1;
}

static int IsWallyZigzagoon_2(const struct cs_insn * insn)
{
    cs_arm_op * ops = insn->detail->arm.operands;
    // ldr rX, =SPECIES_ZIGZAGOON
    if (insn->id == ARM_INS_LDR
        && ops[0].type == ARM_OP_REG
        && ops[0].reg == ARM_REG_R1
        && ops[1].type == ARM_OP_MEM
        && !ops[1].subtracted
        && ops[1].mem.base == ARM_REG_PC
        && ops[1].mem.index == ARM_REG_INVALID)
        return (insn->address & ~3) + ops[1].mem.disp + 4;
    return -1;
}

static int IsWallyZigzagoon(const struct cs_insn * insn)
{
    int retval = IsWallyZigzagoon_1(insn);
    if (retval >= 0)
        return retval;
    return IsWallyZigzagoon_2(insn);
}

static int IsWallyRalts_1(const struct cs_insn * insn)
{
    static int to_return;
    static unsigned tmp_reg;
    cs_arm_op * ops = insn->detail->arm.operands;
    // mov rX, SPECIES_RALTS / 2
    if (insn->id == ARM_INS_MOV
        && ops[0].type == ARM_OP_REG
        && ops[1].type == ARM_OP_IMM
        && ops[1].imm == SPECIES_RALTS / 2)
    {
        to_return = insn->address;
        tmp_reg = ops[0].reg;
    }
        // lsl rX, rY, 1
    else if (insn->id == ARM_INS_LSL
             && ops[0].type == ARM_OP_REG
             && ops[0].reg == ARM_REG_R1
             && ops[1].type == ARM_OP_REG
             && ops[1].reg == tmp_reg
             && ops[2].type == ARM_OP_IMM
             && ops[2].imm == 1
        )
        return to_return;

    return -1;
}

static int IsWallyRalts_2(const struct cs_insn * insn)
{
    cs_arm_op * ops = insn->detail->arm.operands;
    // ldr rX, =SPECIES_RALTS
    if (insn->id == ARM_INS_LDR
        && ops[0].type == ARM_OP_REG
        && ops[0].reg == ARM_REG_R1
        && ops[1].type == ARM_OP_MEM
        && !ops[1].subtracted
        && ops[1].mem.base == ARM_REG_PC
        && ops[1].mem.index == ARM_REG_INVALID)
        return (insn->address & ~3) + ops[1].mem.disp + 4;
    return -1;
}

static int IsWallyRalts(const struct cs_insn * insn)
{
    int retval = IsWallyRalts_1(insn);
    if (retval >= 0)
        return retval;
    return IsWallyRalts_2(insn);
}

/*
 * ---------------------------------------------------------
 * get_instr_addr(
 *   FILE * elfFile,
 *   const char * symname,
 *   int (*callback)(const struct cs_insn *)
 * )
 *
 * Disassembles the function of the provided name until the
 * callback returns a positive integer, then returns that
 * integer. If the end of the function is reached and the
 * callback never returns positive, -1 is returned instead.
 * The callback takes a single argument, a pointer to a
 * disassembled instruction. It then runs internal logic to
 * determine whether the instruction or sequence of in-
 * structions is what is desired, then returns the address
 * of that instruction.
 * ---------------------------------------------------------
 */

static int get_instr_addr(FILE * elfFile, const char * symname, int (*callback)(const struct cs_insn *))
{
    int retval = -1;
    Elf32_Sym * sym = GetSymbolByName(symname);
    fseek(elfFile, (sym->st_value & ~1) - sh_text->sh_addr + sh_text->sh_offset, SEEK_SET);
    unsigned char * data = malloc(sym->st_size);
    if (fread(data, 1, sym->st_size, elfFile) != sym->st_size)
        FATAL_ERROR("fread");
    struct cs_insn *insn;
    int count = cs_disasm(sCapstone, data, sym->st_size, sym->st_value & ~1, 0, &insn);
    for (int i = 0; i < count; i++) {
        int to_return = callback(&insn[i]);
        if (to_return >= 0) {
            retval = to_return;
            break;
        }
    }
    cs_free(insn, count);
    free(data);
    return retval;
}

int main(int argc, char ** argv)
{
    const char * romName = "Emerald EX (U)";
    const char * romCode = "SPDX";
    FILE * elfFile = NULL;
    FILE * outFile = NULL;

    // Argument parser
    for (int i = 1; i < argc; i++) {
        char * arg = argv[i];
        if (strcmp(arg, "--name") == 0) {
            i++;
            if (i == argc) {
                FATAL_ERROR("missing argument to --name\n");
            }
            romName = argv[i];
        } else if (strcmp(arg, "--code") == 0) {
            i++;
            if (i == argc) {
                FATAL_ERROR("missing argument to --code\n");
            }
            romCode = argv[i];
        } else if (arg[0] == '-') {
            FATAL_ERROR("unrecognized option: \"%s\"\n", arg);
        } else if (elfFile == NULL) {
            elfFile = fopen(arg, "rb");
            if (elfFile == NULL) {
                FATAL_ERROR("unable to open file \"%s\" for reading\n", arg);
            }
        } else if (outFile == NULL) {
            outFile = fopen(arg, "w");
            if (outFile == NULL) {
                FATAL_ERROR("unable to open file \"%s\" for writing\n", arg);
            }
        } else {
            FATAL_ERROR("usage: %s ELF OUTPUT [--name NAME] [--code CODE]\n", argv[0]);
        }
    }

    if (outFile == NULL) {
        FATAL_ERROR("usage: %s ELF OUTPUT [--name NAME] [--code CODE]\n", argv[0]);
    }

    // Load the ELF metadata
    InitElf(elfFile);
#ifdef _MSC_VER
#define print(format, ...) (fprintf(outFile, format, __VA_ARGS__))
#else
#define print(format, ...) (fprintf(outFile, format, ##__VA_ARGS__))
#endif
#define config_set(name, value) (print("%s=0x%X\n", (name), (value)))
#define sym_get(name) (GetSymbolByName((name))->st_value)
#define config_sym(name, symname) (config_set((name), sym_get(symname) & 0xFFFFFF))

    // Initialize Capstone
    cs_open(CS_ARCH_ARM, CS_MODE_THUMB, &sCapstone);
    cs_option(sCapstone, CS_OPT_DETAIL, CS_OPT_ON);
    sh_text = GetSectionHeaderByName(".text");

    // Start writing the INI
    print("[%s]\n", romName);
    print("Game=%s\n", romCode);
    print("Version=0\n");
    print("TableFile=gba_english\n");

    // Find the first block after the ROM
    int shnum = GetSectionHeaderCount();
    uint32_t entry = GetEntryPoint();
    uint32_t end = entry;
    for (int i = 0; i < shnum; i++) {
        Elf32_Shdr * sec = GetSectionHeader(i);
        end = max(end, sec->sh_addr + sec->sh_size);
    }
    end -= entry;
    if (end & 0xFFFF) {
        end += 0x10000 - (end & 0xFFFF);
    }
    print("FreeSpace=0x%X\n", end);

    // Pokemon data
	config_sym("PokedexOrder", "gSpeciesToNationalPokedexNum");
    print("PokemonCount=%d\n", NUM_SPECIES - 1);
    print("PokemonNameLength=%d\n", POKEMON_NAME_LENGTH + 1);
    config_sym("PokemonMovesets", "gLevelUpLearnsets");
    config_sym("PokemonTMHMCompat", "gTMHMLearnsets");
    config_sym("PokemonEvolutions", "gEvolutionTable");
    config_sym("StarterPokemon", "sStarterMon");
    // This symbol is inside a C function, so we must take an assist from capstone.
    config_set("StarterItems", get_instr_addr(elfFile, "CB2_GiveStarter", IsLoadingStarterItems) & 0xFFFFFF);
    config_sym("TrainerData", "gTrainers");
    Elf32_Sym * Em_gTrainers = GetSymbolByName("gTrainers");
    config_sym("WildPokemon", "gWildMonHeaders");
    print("TrainerEntrySize=%d\n", Em_gTrainers->st_size / TRAINERS_COUNT);
    config_set("TrainerCount", TRAINERS_COUNT);
    config_sym("TrainerClassNames", "gTrainerClassNames");
    Elf32_Sym * Em_gTrainerClassNames = GetSymbolByName("gTrainerClassNames");
    print("TrainerClassCount=%d\n", TRAINER_CLASS_COUNT);
    print("TrainerClassNameLength=%d\n", Em_gTrainerClassNames->st_size / TRAINER_CLASS_COUNT);
    print("TrainerNameLength=%d\n", 12); // hardcoded for now
    print("DoublesTrainerClasses=[%d, %d, %d, %d, %d]\n", TRAINER_CLASS_SR_AND_JR, TRAINER_CLASS_TWINS, TRAINER_CLASS_YOUNG_COUPLE, TRAINER_CLASS_OLD_COUPLE, TRAINER_CLASS_SIS_AND_BRO); // hardcoded for now
    Elf32_Sym * Em_gItems = GetSymbolByName("gItems");
    print("ItemEntrySize=%d\n", Em_gItems->st_size / ITEMS_COUNT);
    print("ItemCount=%d\n", ITEMS_COUNT - 2); // NONE, DONE_BUTTON
    print("MoveCount=%d\n", MOVES_COUNT - 1);
    config_sym("MoveDescriptions", "gMoveDescriptionPointers");
    Elf32_Sym * Em_gMoveNames = GetSymbolByName("gMoveNames");
    print("MoveNameLength=%d\n", Em_gMoveNames->st_size / MOVES_COUNT);
    Elf32_Sym * Em_gAbilityNames = GetSymbolByName("gAbilityNames");
    print("AbilityNameLength=%d\n", Em_gAbilityNames->st_size / ABILITIES_COUNT);
    config_sym("TmMoves", "sTMHMMoves");
    config_sym("TmMovesDuplicate", "sUnused_StatStrings");
    config_sym("MoveTutorData", "gTutorMoves");
    Elf32_Sym* Em_gTutorMoves = GetSymbolByName("gTutorMoves");
    print("MoveTutorMoves=%d\n", Em_gTutorMoves->st_size / 2);
    config_sym("ItemImages", "gItemIconTable");

    print("TmPals=[");
    char buffer[64];
    for (int i = 0; i < len(gTypeNames); i++) {
        sprintf(buffer, "gItemIconPalette_%sTMHM", gTypeNames[i]);
        if (i != 0)
            print(",");
        print("0x%X", GetSymbolByName(buffer)->st_value & 0xFFFFFF);
    }
    print("]\n");

    config_set("IntroCryOffset", get_instr_addr(elfFile, "Task_NewGameBirchSpeechSub_InitPokeBall", IsIntroLotadForCry) & 0xFFFFFF);
    config_set("IntroSpriteOffset", get_instr_addr(elfFile, "NewGameBirchSpeech_CreateLotadSprite", IsIntroLotadForPic) & 0xFFFFFF);
    print("ItemBallPic=%d\n", OBJ_EVENT_GFX_ITEM_BALL);
    config_sym("TradeTableOffset", "sIngameTrades");
    Elf32_Sym * Em_gIngameTrades = GetSymbolByName("sIngameTrades");
    print("TradeTableSize=%d\n", Em_gIngameTrades->st_size / 60); // hardcoded for now
    print("TradesUnused=[]\n"); // so randomizer doesn't complain
    config_set("CatchingTutorialOpponentMonOffset", get_instr_addr(elfFile, "StartWallyTutorialBattle", IsWallyRalts) & 0xFFFFFF);
    config_set("CatchingTutorialPlayerMonOffset", get_instr_addr(elfFile, "PutZigzagoonInPlayerParty", IsWallyZigzagoon) & 0xFFFFFF);
    config_sym("PCPotionOffset", "gNewGamePCItems");

    // These may need some fixing to support dynamic offsets.
    print("StaticPokemonSupport=1\n");
    for (int i = 0; i < len(gStaticPokemon); i++) {
        print("StaticPokemon[]=[");
        for (int j = 0; j < 8; j++) {
            if (gStaticPokemon[i][j].label == NULL) break;
            if (j != 0)
                print(",");
            print("0x%X", (sym_get(gStaticPokemon[i][j].label) & 0xFFFFFF) + gStaticPokemon[i][j].offset);
        }
        print("]\n");
    }
    for (int i = 0; i < len(gTMTexts); i++) {
        Elf32_Sym * sym = GetSymbolByName(gTMTexts[i].label);
        print("TMTextSpdc[]=[%d,0x%X,%s]\n", gTMTexts[i].tmno, (sym->st_value + 2) & 0xFFFFFF, gTMTexts[i].text);
    }
    for (int i = 0; i < len(gMoveTutorTexts); i++) {
        Elf32_Sym * sym = GetSymbolByName(gMoveTutorTexts[i].label);
        print("MoveTutorTextSpdc[]=[%d,0x%X,%s]\n", gMoveTutorTexts[i].tmno, (sym->st_value + 2) & 0xFFFFFF, gMoveTutorTexts[i].text);
    }

    config_sym("PokedexOrder", "gSpeciesToNationalPokedexNum");
    config_sym("CheckValueOffset", "gRandomizerCheckValue");

    DestroyResources();
    fclose(outFile);
    fclose(elfFile);
    return 0;
}
