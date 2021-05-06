#include "global.h"
#include "main.h"
#include "menu.h"
#include "palette.h"
#include "sprite.h"
#include "task.h"
#include "sound.h"
#include "constants/songs.h"
#include "string_util.h"
#include "text.h"
#include "speedchoice.h"
#include "gpu_regs.h"
#include "bg.h"
#include "scanline_effect.h"
#include "text_window.h"
#include "field_message_box.h"
#include "text_window.h"
#include "done_button.h"
#include "naming_screen.h"
#include "random.h"
#include "constants/rgb.h"

// A macro was defined here to simplify the row used in Palette calls, but I haven't
// used this yet.
#define PLTTROW(row) ((row) * (0x20))

// We reuse the option menu and main menu palette data to simplfy things.
extern u16 sOptionMenuText_Pal[16];
extern u16 sOptionMenuBg_Pal[1];
extern u16 sMainMenuTextPal[16];

// ----------------------------------------------
// Window IDs enumeration for the defined
// Speedchoice menu templates.
// ----------------------------------------------
enum
{
    SPD_WIN_TEXT_OPTION,
    SPD_WIN_OPTIONS,
    SPD_WIN_TOOLTIP,
    SPD_WIN_YESNO,
};

// ----------------------------------------------
// BG TEMPLATES
// ----------------------------------------------
const struct BgTemplate sSpeedchoiceMenuBgTemplates[] =
{
   {
       .bg = 1,
       .charBaseIndex = 1,
       .mapBaseIndex = 30,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 1,
       .baseTile = 0
   },
   {
       .bg = 0,
       .charBaseIndex = 1,
       .mapBaseIndex = 31,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 2,
       .baseTile = 0
   },
   // 0x000001ec (I forget why this was put here)
   {
       .bg = 2,
       .charBaseIndex = 1,
       .mapBaseIndex = 29,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 0,
       .baseTile = 0
   }
};

const struct WindowTemplate sSpeedchoiceMenuWinTemplates[] =
{
    {1, 2, 1, 0x1A, 2, 1, 2},
    {0, 2, 5, 0x1A, 14, 1, 0x36},
    {2, 2, 15, 0x1A, 4, 15, 427},
    {2, 23, 9, 4, 4, 15, 531}, // YES/NO
    DUMMY_WIN_TEMPLATE
};

// Deprecated ID. This may have been a leftover copy paste from the option menu.
enum
{
    // DEPRECATED
    WIN_TEXT_OPTION,
    WIN_OPTIONS
};


/* ----------------------------------------------- */
/* SPEEDCHOICE MENU TEXT (System Text)             */
/* ----------------------------------------------- */
const u8 gSystemText_TerminatorS[] = _("{COLOR RED}$");

/* ----------------------------------------------- */
/* SPEEDCHOICE MENU TEXT (Header Text)             */
/* ----------------------------------------------- */
const u8 gSpeedchoiceTextHeader[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}EX SPEEDCHOICE MENU");
const u8 gSpeedchoiceCurrentVersion[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}v0.2.1");

/* ----------------------------------------------- */
/* SPEEDCHOICE MENU TEXT (Option Choices)          */
/* ----------------------------------------------- */
const u8 gSpeedchoiceTextYes[]    = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}YES");
const u8 gSpeedchoiceTextNo[]     = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}NO");

const u8 gSpeedchoiceTextOn[]     = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}ON");
const u8 gSpeedchoiceTextOff[]    = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}OFF");

const u8 gSpeedchoiceTextNerf[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}PURGE");
const u8 gSpeedchoiceTextKeep[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}KEEP");
const u8 gSpeedchoiceTextHell[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}HELL");
const u8 gSpeedchoiceTextWhy[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}WHY");

const u8 gSpeedchoiceTextSemi[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}SEMI");
const u8 gSpeedchoiceTextFull[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}FULL");

const u8 gSpeedchoiceTextStatic[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}SAME");
const u8 gSpeedchoiceTextRand[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}RAND");
const u8 gSpeedchoiceTextSane[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}SANE");

const u8 gSpeedchoiceTextBW[]     = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}BW");
const u8 gSpeedchoiceTextNone[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}NONE");

const u8 gSpeedchoiceTextTutor[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}TUTOR");
const u8 gSpeedchoiceTextHM05[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}HM05");

/* ----------------------------------------------- */
/* SPEEDCHOICE MENU TEXT (Option Names)            */
/* ----------------------------------------------- */

// PAGE 1
const u8 gSpeedchoiceOptionPreset[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}PRESET");
const u8 gSpeedchoiceOptionName[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}NAME");
const u8 gSpeedchoiceOptionEXP[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}EXP");
const u8 gSpeedchoiceOptionPlotless[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}PLOTLESS");
const u8 gSpeedchoiceOptionInstantText[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}INSTANT TEXT");
const u8 gSpeedchoiceOptionSpinners[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}SPINNERS");

// PAGE 2
const u8 gSpeedchoiceOptionMaxVision[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}MAX VISION");
const u8 gSpeedchoiceOptionNerfRoxanne[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}NERF ROXANNE");
const u8 gSpeedchoiceOptionSuperBike[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}SUPER BIKE");
const u8 gSpeedchoiceOptionNewWildEnc[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}NEW WILD ENC.");
const u8 gSpeedchoiceOptionEarlyFly[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}EARLY FLY");

// PAGE 3
const u8 gSpeedchoiceOptionRunEverywhere[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}RUN EVERYWHERE");
const u8 gSpeedchoiceOptionMemeIsland[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}MEME ISLAND");
const u8 gSpeedchoiceOptionBetterMarts[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}BETTER MARTS");
const u8 gSpeedchoiceOptionGoodEarlyWilds[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}GOOD EARLY WILDS");
const u8 gSpeedchoiceOptionEarlySurf[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}EARLY SURF");

// PAGE 4
const u8 gSpeedchoiceOptionNiceMenuOrder[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}NICE MENU ORDER");
const u8 gSpeedchoiceOptionEasyFalseSwipe[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}EASY FALSE SWIPE");
const u8 gSpeedchoiceOptionFastCatch[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}FAST CATCH");
const u8 gSpeedchoiceOptionEarlyBike[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}EARLY BIKE");
const u8 gSpeedchoiceOptionFastEggHatch[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}FAST EGG HATCH");

// PAGE 5
const u8 gSpeedchoiceOptionGen7XItems[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}GEN 7 X ITEMS");
const u8 gSpeedchoiceOptionEvoEveryLv[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}EVO EVERY LV");

// CONSTANT OPTIONS
const u8 gSpeedchoiceOptionPage[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}PAGE");
const u8 gSpeedchoiceOptionStartGame[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}START GAME");

// ARROWS
const u8 gSpeedchoiceOptionLeftArrow[] = _("{COLOR RED}{SHADOW LIGHT_RED}{LEFT_ARROW}");
const u8 gSpeedchoiceOptionRightArrow[] = _("{COLOR RED}{SHADOW LIGHT_RED}{RIGHT_ARROW}");

/* ----------------------------------------------- */
/* SPEEDCHOICE MENU TEXT (Tooltip Text)            */
/* ----------------------------------------------- */

// INTRODUCTION
const u8 gSpeedchoiceTooltipExplanation[] = _("This is the Speedchoice menu where\nvarious options can be selected.\pTo get an explanation of each option,\npress SELECT when over the option.");

// TOOLTIPS
const u8 gSpeedchoiceTooltipPreset[] = _("Sets of predetermined options\nthat help speedrunners quickly select\pa set of options for a type of\nspeedrunning category.");
const u8 gSpeedchoiceTooltipName[] = _("Set your trainer name here.");
const u8 gSpeedchoiceTooltipEXP[] = _("Modifies the experience system\nto the desired input.\pCan be Gen 5, or\nno exp at all.");
const u8 gSpeedchoiceTooltipPlotless[] = _("SEMI: Stops villain team events\nafter Mt. Chimney.\pFULL: Skips all the villain team\nevents. Also allows Dive to be used\pwithout Gym 7's badge.");
const u8 gSpeedchoiceTooltipInstantText[] = _("Self-explanatory.\nHold A or B to mash.");
const u8 gSpeedchoiceTooltipSpinners[] = _("PURGE: Makes spinners on a static\nspinning pattern at a fixed rate.\pHELL: Rapidly spins\nevery spinner every frame.\pWHY: Same as HELL but 4 frames\ninstead of 16.\pHELL and WHY also fix bag\nmanip.");
const u8 gSpeedchoiceTooltipMaxVision[] = _("SANE: Will extend trainer vision\nto 8, but prevent trainers from\pwalking through walls or solid\nobjects.\pHELL: No collision or\nelevation detection.");
const u8 gSpeedchoiceTooltipNerfRoxanne[] = _("Nerfs Gym Leader Roxanne.\nRemoves both of her Potions.");
const u8 gSpeedchoiceTooltipSuperBike[] = _("While riding the bicycle, you\ncan switch between bikes with\pthe R button while you are\nstanding still.");
const u8 gSpeedchoiceTooltipNewWildEnc[] = _("Intended to be used with\nthe randomizer.\pTreats grass and fishing slots\nas 6 and 2 slots respectively.");
const u8 gSpeedchoiceTooltipEarlyFly[] = _("Recieve HM02 instead of\nItemfinder at Rival 2.\pAllows use of Fly without use of\nGym 6's badge.");
const u8 gSpeedchoiceTooltipRunEverywhere[] = _("Allows the Running Shoes to\nbe used anywhere, including\pinside of houses.\pAlso allows bike in most places.");
const u8 gSpeedchoiceTooltipMemeIsland[] = _("Mirage Island always appears.");
const u8 gSpeedchoiceTooltipBetterMarts[] = _("Improves the item selections\nof many {PKMN} marts.\pAdds repels to Oldale Mart.\pAdds repels and super repels to\nMauville Mart.\pAdds X Specials to Rustboro\nMart.");
const u8 gSpeedchoiceTooltipGoodEarlyWilds[] = _("SAME: Depending on the\nrandomizer check value, wild\pencounters in the grass for\n{PKMN} below lv 10 will have\ptheir final evolution.\pRAND: If they have a branching\nevolution, it will be randomly\pgenerated instead of being static.");
const u8 gSpeedchoiceTooltipEarlySurf[] = _("Switches the locations of\nHM04 and HM03.\pUse of Surf requirement only needs\nWattson's Gym Badge.\pIn addition, Steven post-Fortree\nwill not spawn until after GYM 5.");
const u8 gSpeedchoiceTooltipNiceMenuOrder[] = _("Swaps the location of SUMMARY and\nfield moves in party menu.\palso swaps CHECK TAG and USE\nin the berry bag.\pON: Fields moves and berry USE are first.\nOFF: SUMMARY and CHECK TAG are first.");
const u8 gSpeedchoiceTooltipEasyFalseSwipe[] = _("Makes FALSE SWIPE guaranteed\nOFF: Vanilla game behavior\pTUTOR: The tutor in Slateport teaches\nFALSE SWIPE.\pHM05: Replaces HM05 FLASH with\nHM05 FALSE SWIPE.");
const u8 gSpeedchoiceTooltipFastCatch[] = _("All Pokeballs are guaranteed to catch.");
const u8 gSpeedchoiceTooltipEarlyBike[] = _("Start game with Bicycle.");
const u8 gSpeedchoiceTooltipFastEggHatch[] = _("Makes eggs hatch quickly.");
const u8 gSpeedchoiceTooltipGen7XItems[] = _("Stat boost +2 instead of +1.");
const u8 gSpeedchoiceTooltipEvoEveryLv[] = _("{PKMN} evolve into a random\nbut set species every lv.");

// START GAME
const u8 gSpeedchoiceStartGameText[] = _("CV: {STR_VAR_1}\nStart the game?");

// MISC STRINGS
const u8 gSpeedchoiceEscapeText[] = _("ESCAPE");

/* ----------------- */
/* ---- PRESETS ---- */
/* ----------------- */
const u8 gPresetNames[][20] = {
    _("{COLOR GREEN}{SHADOW LIGHT_GREEN}VANILLA"),
    _("{COLOR GREEN}{SHADOW LIGHT_GREEN}BINGO"),
    _("{COLOR GREEN}{SHADOW LIGHT_GREEN}CEA"),
    _("{COLOR GREEN}{SHADOW LIGHT_GREEN}RACE"),
    _("{COLOR GREEN}{SHADOW LIGHT_GREEN}MEME"),
};

// use local config optionConfig[0] for preset!

// -------------------------
// Enumeration for Preset.
// -------------------------
enum {
    PRESET_VANILLA,
    PRESET_BINGO,
    PRESET_CEA,
    PRESET_RACE,
    PRESET_MEME
};

// When creating the task that manages the speedchoice menu, store the task ID in RAM to
// be used later when destroying the task.
static EWRAM_DATA int gSpeedchoiceTaskId = 0;

// ----------------------------------------------------------------
// Preset Data for Speedchoice Menu
// ----------------------------------------------------------------
static const u8 gPresetVanilla[CURRENT_OPTIONS_NUM] = {
    PRESET_VANILLA,         // PRESET
    0xFF,                   // (Reserved for trainer name)
    EXP_KEEP,               // EXP
    PLOT_KEEP,              // PLOTLESS
    SPIN_KEEP,              // SPINNERS
    MAX_OFF,                // MAXVISION
    FLY_NO,                 // EARLYFLY
    GOOD_OFF,               // GOOD_EARLY_WILDS
    SURF_OFF,               // EARLYSURF
    NICE_MENU_ORDER_OFF,    // NICE_MENU_ORDER
    EASY_FALSE_SWIPE_OFF,   // EASY_FALSE_SWIPE
    FAST_CATCH_OFF,         // FAST_CATCH
    EARLY_BIKE_NO,          // EARLY_BIKE
    EVO_EV_OFF              // EVO_EVERY_LEVEL
};

static const u8 gPresetBingo[CURRENT_OPTIONS_NUM] = {
    PRESET_BINGO,           // PRESET
    0xFF,                   // (Reserved for trainer name)
    EXP_BW,                 // EXP
    PLOT_FULL,              // PLOTLESS
    SPIN_KEEP,              // SPINNERS
    MAX_OFF,                // MAXVISION
    FLY_YES,                // EARLYFLY
    GOOD_OFF,               // GOOD_EARLY_WILDS
    SURF_ON,                // EARLYSURF
    NICE_MENU_ORDER_ON,     // NICE_MENU_ORDER
    EASY_FALSE_SWIPE_TUTOR, // EASY_FALSE_SWIPE
    FAST_CATCH_OFF,         // FAST_CATCH
    GEN_7_X_ITEMS_ON,       // GEN_7_X_ITEMS
    EVO_EV_OFF              // EVO_EVERY_LEVEL
};

static const u8 gPresetCEA[CURRENT_OPTIONS_NUM] = {
    PRESET_CEA,             // PRESET
    0xFF,                   // (Reserved for trainer name)
    EXP_BW,                 // EXP
    PLOT_FULL,              // PLOTLESS
    SPIN_NERF,              // SPINNERS
    MAX_OFF,                // MAXVISION
    FLY_YES,                // EARLYFLY
    GOOD_OFF,               // GOOD_EARLY_WILDS
    SURF_ON,                // EARLYSURF
    NICE_MENU_ORDER_ON,     // NICE_MENU_ORDER
    EASY_FALSE_SWIPE_TUTOR, // EASY_FALSE_SWIPE
    FAST_CATCH_ON,          // FAST_CATCH
    GEN_7_X_ITEMS_ON,       // GEN_7_X_ITEMS
    EVO_EV_OFF              // EVO_EVERY_LEVEL
};

static const u8 gPresetRace[CURRENT_OPTIONS_NUM] = {
    PRESET_RACE,            // PRESET
    0xFF,                   // (Reserved for trainer name)
    EXP_BW,                 // EXP
    PLOT_FULL,              // PLOTLESS
    SPIN_NERF,              // SPINNERS
    MAX_OFF,                // MAXVISION
    FLY_YES,                // EARLYFLY
    GOOD_STATIC,            // GOOD_EARLY_WILDS
    SURF_ON,                // EARLYSURF
    NICE_MENU_ORDER_ON,     // NICE_MENU_ORDER
    EASY_FALSE_SWIPE_TUTOR, // EASY_FALSE_SWIPE
    FAST_CATCH_OFF,         // FAST_CATCH
    GEN_7_X_ITEMS_ON,       // GEN_7_X_ITEMS
    EVO_EV_OFF              // EVO_EVERY_LEVEL
};

static const u8 gPresetMeme[CURRENT_OPTIONS_NUM] = {
    PRESET_RACE,            // PRESET
    0xFF,                   // (Reserved for trainer name)
    EXP_NONE,               // EXP
    PLOT_FULL,              // PLOTLESS
    SPIN_WHY,               // SPINNERS
    MAX_HELL,               // MAXVISION
    FLY_NO,                 // EARLYFLY
    GOOD_RAND,              // GOOD_EARLY_WILDS
    SURF_OFF,               // EARLYSURF
    NICE_MENU_ORDER_OFF,    // NICE_MENU_ORDER
    EASY_FALSE_SWIPE_OFF,   // EASY_FALSE_SWIPE
    FAST_CATCH_OFF,         // FAST_CATCH
    GEN_7_X_ITEMS_OFF,      // GEN_7_X_ITEMS
    EVO_EV_ON               // EVO_EVERY_LEVEL
};

/*
 * Fetch the Preset array from the set Preset ID.
 */
const u8 *GetPresetPtr(int presetID) {
    switch(presetID) {
        case PRESET_VANILLA:
        default:
            return gPresetVanilla;
        case PRESET_BINGO:
            return gPresetBingo;
        case PRESET_CEA:
            return gPresetCEA;
        case PRESET_RACE:
            return gPresetRace;
        case PRESET_MEME:
            return gPresetMeme;
    }
}

// ---------------------------------------
// Speedchoice menu Option Config data
// ---------------------------------------
const struct OptionChoiceConfig OptionChoiceConfigYesNo[MAX_CHOICES] = 
{
    { 120, (u8 *)&gSpeedchoiceTextYes },
    { 150, (u8 *)&gSpeedchoiceTextNo  },
    { -1, NULL },
    { -1, NULL },
    { -1, NULL },
    { -1, NULL }
};

const struct OptionChoiceConfig OptionChoiceConfigOnOff[MAX_CHOICES] = 
{
    { 120, (u8 *)&gSpeedchoiceTextOn  },
    { 150, (u8 *)&gSpeedchoiceTextOff },
    { -1, NULL },
    { -1, NULL },
    { -1, NULL },
    { -1, NULL }
};

const struct OptionChoiceConfig OptionChoiceConfigNerfKeep[MAX_CHOICES] = 
{
    { 85, (u8 *)&gSpeedchoiceTextNerf },
    { 120, (u8 *)&gSpeedchoiceTextKeep },
    { 150, (u8 *)&gSpeedchoiceTextHell },
    { 180, (u8 *)&gSpeedchoiceTextWhy },
    { -1, NULL },
    { -1, NULL }
};

const struct OptionChoiceConfig OptionChoiceConfigSemiFull[MAX_CHOICES] = 
{
    { 120, (u8 *)&gSpeedchoiceTextSemi },
    { 150, (u8 *)&gSpeedchoiceTextKeep },
    { 180, (u8 *)&gSpeedchoiceTextFull },
    { -1, NULL },
    { -1, NULL },
    { -1, NULL }
};

const struct OptionChoiceConfig OptionChoiceConfigOffRand[MAX_CHOICES] = 
{
    { 120, (u8 *)&gSpeedchoiceTextOff    },
    { 150, (u8 *)&gSpeedchoiceTextStatic },
    { 180, (u8 *)&gSpeedchoiceTextRand   },
    { -1, NULL },
    { -1, NULL },
    { -1, NULL }
};

const struct OptionChoiceConfig OptionChoiceConfigOffTutorHM[MAX_CHOICES] = 
{
    { 120, (u8 *)&gSpeedchoiceTextOff   },
    { 145, (u8 *)&gSpeedchoiceTextTutor },
    { 180, (u8 *)&gSpeedchoiceTextHM05  },
    { -1, NULL },
    { -1, NULL },
    { -1, NULL }
};

const struct OptionChoiceConfig OptionChoiceConfigSaneHell[MAX_CHOICES] = 
{
    { 120, (u8 *)&gSpeedchoiceTextOff  },
    { 150, (u8 *)&gSpeedchoiceTextSane },
    { 180, (u8 *)&gSpeedchoiceTextHell },
    { -1, NULL },
    { -1, NULL },
    { -1, NULL }
};

const struct OptionChoiceConfig OptionChoiceConfigKeepNone[MAX_CHOICES] = 
{
    { 120, (u8 *)&gSpeedchoiceTextKeep },
    { 150, (u8 *)&gSpeedchoiceTextBW   },
    { 180, (u8 *)&gSpeedchoiceTextNone },
    { -1, NULL },
    { -1, NULL },
    { -1, NULL }
};

/*
 * In order to use ProcessGeneralInput, a struct is needed for page, so, I opt to have a
 * dummy struct which only has the number of choices relevent to the calculation of the
 * selection.
 */
const struct OptionChoiceConfig OptionChoiceConfigPage[MAX_CHOICES] = 
{
    { -1, NULL },
    { -1, NULL },
    { -1, NULL },
    { -1, NULL },
    { -1, NULL },
    { -1, NULL }
};

// player name needs its own config. Leave NULL there.
const struct OptionChoiceConfig OptionChoiceConfigPlayerName[MAX_CHOICES] = 
{
    { 120, NULL }, // we still have a coord so it's handled properly.
    { -1, NULL },
    { -1, NULL },
    { -1, NULL },
    { -1, NULL },
    { -1, NULL }
};

// not a normal config struct, but used for the arrows for multi choice.
const struct OptionChoiceConfig Arrows[MAX_CHOICES] = 
{
    { 120, (u8 *)&gSpeedchoiceOptionLeftArrow },
    { 195, (u8 *)&gSpeedchoiceOptionRightArrow },
    { -1, NULL },
    { -1, NULL },
    { -1, NULL },
    { -1, NULL }
};

// ---------------------------------------
// Speedchoice menu Option data
// ---------------------------------------
const struct SpeedchoiceOption SpeedchoiceOptions[CURRENT_OPTIONS_NUM + 1] = // plus one for page
{
    // ----------------------------------
    // PRESET OPTION
    // ----------------------------------
    { 
        /* Option Count   */ 5,
        /* Option Type    */ ARROW,
        /* Option Preset  */ gSpeedchoiceOptionPreset,
        /* Option Text    */ Arrows,
        /* Option Tooltip */ gSpeedchoiceTooltipPreset,
        /* Option Usable  */ TRUE
    },
    // ----------------------------------
    // PLAYER NAME
    // ----------------------------------
    { 
        /* Option Count   */ 1,
        /* Option Type    */ PLAYER_NAME,
        /* Option Preset  */ gSpeedchoiceOptionName,
        /* Option Text    */ OptionChoiceConfigPlayerName,
        /* Option Tooltip */ gSpeedchoiceTooltipName,
        /* Option Usable  */ TRUE
    },
    // ----------------------------------
    // EXP OPTION
    // ----------------------------------
    { 
        /* Option Count   */ 3,
        /* Option Type    */ NORMAL,
        /* Option Preset  */ gSpeedchoiceOptionEXP,
        /* Option Text    */ OptionChoiceConfigKeepNone,
        /* Option Tooltip */ gSpeedchoiceTooltipEXP,            
        /* Option Usable  */ TRUE  
    },
    // ----------------------------------
    // PLOTLESS OPTION
    // ----------------------------------
    { 
        /* Option Count   */ 3,
        /* Option Type    */ NORMAL,
        /* Option Preset  */ gSpeedchoiceOptionPlotless,
        /* Option Text    */ OptionChoiceConfigSemiFull,
        /* Option Tooltip */ gSpeedchoiceTooltipPlotless,
        /* Option Usable  */ TRUE 
    },
    // ----------------------------------
    // SPINNERS OPTION
    // ----------------------------------
    { 
        /* Option Count   */ 4,
        /* Option Type    */ NORMAL,
        /* Option Preset  */ gSpeedchoiceOptionSpinners,
        /* Option Text    */ OptionChoiceConfigNerfKeep,
        /* Option Tooltip */ gSpeedchoiceTooltipSpinners,
        /* Option Usable  */ TRUE 
    },
    // ----------------------------------
    // MAX VISION OPTION
    // ----------------------------------
    { 
        /* Option Count   */ 3,
        /* Option Type    */ NORMAL,
        /* Option Preset  */ gSpeedchoiceOptionMaxVision,
        /* Option Text    */ OptionChoiceConfigSaneHell,
        /* Option Tooltip */ gSpeedchoiceTooltipMaxVision,
        /* Option Usable  */ TRUE
    },
    // ----------------------------------
    // EARLY FLY OPTION
    // ----------------------------------
    { 
        /* Option Count   */ 2,
        /* Option Type    */ NORMAL,
        /* Option Preset  */ gSpeedchoiceOptionEarlyFly,
        /* Option Text    */ OptionChoiceConfigYesNo,
        /* Option Tooltip */ gSpeedchoiceTooltipEarlyFly,
        /* Option Usable  */ TRUE
    },
    // ----------------------------------
    // GOOD EARLY WILDS OPTION
    // ----------------------------------
    { 
        /* Option Count   */ 3,
        /* Option Type    */ NORMAL,
        /* Option Preset  */ gSpeedchoiceOptionGoodEarlyWilds,
        /* Option Text    */ OptionChoiceConfigOffRand,
        /* Option Tooltip */ gSpeedchoiceTooltipGoodEarlyWilds,
        /* Option Usable  */ TRUE
    },
    // ----------------------------------
    // EARLY SURF OPTION
    // ----------------------------------
    { 
        /* Option Count   */ 2,
        /* Option Type    */ NORMAL,
        /* Option Preset  */ gSpeedchoiceOptionEarlySurf,
        /* Option Text    */ OptionChoiceConfigOnOff,
        /* Option Tooltip */ gSpeedchoiceTooltipEarlySurf,
        /* Option Usable  */ TRUE
    },
    // ----------------------------------
    // PARTY MENU OPTION
    // ----------------------------------
    { 
        /* Option Count   */ 2,
        /* Option Type    */ NORMAL,
        /* Option Preset  */ gSpeedchoiceOptionNiceMenuOrder,
        /* Option Text    */ OptionChoiceConfigOnOff,
        /* Option Tooltip */ gSpeedchoiceTooltipNiceMenuOrder,
        /* Option Usable  */ TRUE
    },
    // ----------------------------------
    // EASY FALSE SWIPE OPTION
    // ----------------------------------
    { 
        /* Option Count   */ 3,
        /* Option Type    */ NORMAL,
        /* Option Preset  */ gSpeedchoiceOptionEasyFalseSwipe,
        /* Option Text    */ OptionChoiceConfigOffTutorHM,
        /* Option Tooltip */ gSpeedchoiceTooltipEasyFalseSwipe,
        /* Option Usable  */ TRUE
    },
    // ----------------------------------
    // FAST CATCH OPTION
    // ----------------------------------
    { 
        /* Option Count   */ 2,
        /* Option Type    */ NORMAL,
        /* Option Preset  */ gSpeedchoiceOptionFastCatch,
        /* Option Text    */ OptionChoiceConfigOnOff,
        /* Option Tooltip */ gSpeedchoiceTooltipFastCatch,
        /* Option Usable  */ TRUE
    },
    // ----------------------------------
    // GEN 7 X ITEMS OPTION
    // ----------------------------------
    { 
        /* Option Count   */ 2,
        /* Option Type    */ NORMAL,
        /* Option Preset  */ gSpeedchoiceOptionGen7XItems,
        /* Option Text    */ OptionChoiceConfigOnOff,
        /* Option Tooltip */ gSpeedchoiceTooltipGen7XItems,
        /* Option Usable  */ TRUE
    },
    // ----------------------------------
    // EVO EVERY LEVEL OPTION
    // ----------------------------------
    { 
        /* Option Count   */ 2,
        /* Option Type    */ NORMAL,
        /* Option Preset  */ gSpeedchoiceOptionEvoEveryLv,
        /* Option Text    */ OptionChoiceConfigOnOff,
        /* Option Tooltip */ gSpeedchoiceTooltipEvoEveryLv,
        /* Option Usable  */ TRUE
    },
    // ----------------------------------
    // PAGE STATIC OPTION
    // ----------------------------------
    { 
        /* Option Count   */ MAX_PAGES,
        /* Option Type    */ NORMAL,
        /* Option Preset  */ gSpeedchoiceOptionPage,
        /* Option Text    */ OptionChoiceConfigPage,
        /* Option Tooltip */ NULL,
        /* Option Usable  */ TRUE
    }
};

// To avoid redrawing the page info every frame, we store the active page number
// and compare it to the task's page number. We only redraw if they are different
// and update accordingly.
EWRAM_DATA u8 gStoredPageNum = 0;

// See SpeedchoiceConfigStruct documentation in speedchoice.h.
EWRAM_DATA struct SpeedchoiceConfigStruct gLocalSpeedchoiceConfig = {0};

// See MapObjectTimerBackup documentation in speedchoice.h.
EWRAM_DATA struct MapObjectTimerBackup gMapObjectTimerBackup[MAX_SPRITES] = {0};

// Deprecated. Was going to be used for the timer backup hack, but a better
// implementation was found.
EWRAM_DATA bool8 gLastMenuWasSubmenu = {0}; // DEPRECATED

// Deprecated. Was going to be used for the Pokedex area screen beep change,
// but a better implementation was found.
EWRAM_DATA bool8 gPokedexAreaScreenFlag = {0}; // DEPRECATED

// Used to signal that the page must redraw in cases where it is needed such as
// updating the Preset.
EWRAM_DATA bool8 gForceUpdate = FALSE;

// Stores the written player name in the options menu until it is flushed to the
// Save Block.
EWRAM_DATA u8 gTempPlayerName[PLAYER_NAME_LENGTH + 1] = {0};

// -------------------------------------
// PROTOTYPES
// -------------------------------------
u8 GetPageDrawCount(u8 page);
void SetPageIndexFromTrueIndex(u8 taskId, s16 index);

/*
 * When initializing the menu for the first time or setting a preset, this function
 * is invoked to set the local config and save block data from the preset or default
 * preset.
 */
void SetOptionChoicesAndConfigFromPreset(const u8 *preset)
{
    u8 i;

    // set the local config for the current menu. Do NOT overwrite the preset!
    for(i = 1; i < CURRENT_OPTIONS_NUM; i++)
        gLocalSpeedchoiceConfig.optionConfig[i] = preset[i];

    // this would be a for loop, but i want to use the fewest bits possible to
    // avoid shifting RAM too much: hence the ugly per-option saving.
    gSaveBlock2Ptr->speedchoiceConfig.expsystem = preset[EXPMATH];
    gSaveBlock2Ptr->speedchoiceConfig.plotless = preset[PLOTLESS];
    gSaveBlock2Ptr->speedchoiceConfig.spinners = preset[SPINNERS];
    gSaveBlock2Ptr->speedchoiceConfig.maxVision = preset[MAXVISION];
    gSaveBlock2Ptr->speedchoiceConfig.earlyfly = preset[EARLYFLY];
    gSaveBlock2Ptr->speedchoiceConfig.goodEarlyWilds = preset[GOOD_EARLY_WILDS];
    gSaveBlock2Ptr->speedchoiceConfig.earlysurf = preset[EARLYSURF];
    gSaveBlock2Ptr->speedchoiceConfig.niceMenuOrder = preset[NICE_MENU_ORDER];
    gSaveBlock2Ptr->speedchoiceConfig.easyFalseSwipe = preset[EASY_FALSE_SWIPE];
    gSaveBlock2Ptr->speedchoiceConfig.fastCatch = preset[FAST_CATCH];
    gSaveBlock2Ptr->speedchoiceConfig.gen7XItems = preset[GEN_7_X_ITEMS];
    gSaveBlock2Ptr->speedchoiceConfig.evoEveryLevel = preset[EVO_EVERY_LEVEL];
}

/*
 * This option is used to check the currently set option data and is used when doing
 * speedchoice checks. (Is the EXP option set to BWEXP, etc)
 *
 * See the speedchoice.h enums for the values to pass here.
 */
bool8 CheckSpeedchoiceOption(u8 option, u8 selection)
{
    switch(option)
    {
        case EXPMATH:
            return gSaveBlock2Ptr->speedchoiceConfig.expsystem == selection;
        case PLOTLESS:
            return gSaveBlock2Ptr->speedchoiceConfig.plotless == selection;
        case SPINNERS:
            return gSaveBlock2Ptr->speedchoiceConfig.spinners == selection;
        case MAXVISION:
            return gSaveBlock2Ptr->speedchoiceConfig.maxVision == selection;
        case EARLYFLY:
            return gSaveBlock2Ptr->speedchoiceConfig.earlyfly == selection;
        case GOOD_EARLY_WILDS:
            return gSaveBlock2Ptr->speedchoiceConfig.goodEarlyWilds == selection;
        case EARLYSURF:
            return gSaveBlock2Ptr->speedchoiceConfig.earlysurf == selection;
        case NICE_MENU_ORDER:
            return gSaveBlock2Ptr->speedchoiceConfig.niceMenuOrder == selection;
        case EASY_FALSE_SWIPE:
            return gSaveBlock2Ptr->speedchoiceConfig.easyFalseSwipe == selection;
        case FAST_CATCH:
            return gSaveBlock2Ptr->speedchoiceConfig.fastCatch == selection;
        case GEN_7_X_ITEMS:
            return gSaveBlock2Ptr->speedchoiceConfig.gen7XItems == selection;
        case EVO_EVERY_LEVEL:
            return gSaveBlock2Ptr->speedchoiceConfig.evoEveryLevel == selection;
        default:
            return FALSE;
    }
}

/*
 * Invoked to locally format the passed text and add it to the text printer to render it.
 */
static void DrawOptionMenuChoice(const u8 *text, u8 x, u8 y, u8 style)
{
    u8 dst[16];
    u16 i;

    for (i = 0; *text != EOS && i <= 14; i++)
        dst[i] = *(text++);

    if (style != 0)
    {
        dst[2] = 4;
        dst[5] = 5;
    }

    dst[i] = EOS;
    AddTextPrinterParameterized(SPD_WIN_OPTIONS, SPEEDCHOICE_FONT_ID, dst, x, y, TEXT_SPEED_FF, NULL);
}

void DrawPageOptions(u8);

/*
 * This is used to handle the inputs per option, not the Speedchoice menu inputs overall.
 * See Task_SpeedchoiceMenuProcessInput for that.
 */
static u8 ProcessGeneralInput(struct SpeedchoiceOption *option, u8 selection, bool8 indexedToOne) // if indexedToOne is true (1), i can conveniently use it as the selection anchor.
{
    if(gMain.newKeys & DPAD_RIGHT)
    {
        if(option->enabled == FALSE)
        {
            PlaySE(SE_FAILURE);
            return selection;
        }
        if(selection == (option->optionCount - (!indexedToOne))) // i invert the option because when indexedToOne is set to true (or 1) it means i do not subtract 1 since it's not 0 indexed, so invert the boolean in both cases to get the correct modifier.
            selection = indexedToOne;
        else
            selection++;
        if(option->string != gSpeedchoiceOptionPage)
            PlaySE(SE_SELECT);
        else
            PlaySE(SE_WIN_OPEN); // page scrolling.
    }
    // i dont return immediately because emulators could hold both right and left down.
    if(gMain.newKeys & DPAD_LEFT)
    {
        if(option->enabled == FALSE)
        {
            PlaySE(SE_FAILURE);
            return selection;
        }
        if(selection == indexedToOne)
            selection = (option->optionCount - (!indexedToOne));
        else
            selection--;
        if(option->string != gSpeedchoiceOptionPage)
            PlaySE(SE_SELECT);
        else
            PlaySE(SE_WIN_OPEN); // page scrolling.
    }
    return selection;
}

// --------------------------------------------
// Standard callbacks for Main and VBlankCB.
// --------------------------------------------
static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

// We dim the whole menu except the header and currently selected option. This is invoked when the menu
// is initialized or re-initialized.
void HighlightHeaderBox(void)
{
    SetGpuReg(REG_OFFSET_WIN1H, WIN_RANGE(17, 223));
    SetGpuReg(REG_OFFSET_WIN1V, WIN_RANGE(1, 31));
}

//This version uses addition '+' instead of OR '|'.
#define WIN_RANGE_(a, b) (((a) << 8) + (b))

// Same as above, but for the option instead.
static void HighlightOptionMenuItem(u8 index)
{
    SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(16, 224));
    SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE_(index * 16 + 40, index * 16 + 56));
}

void CB2_InitSpeedchoice(void);

// The Options menu relies on a CB2 init while New Game relied on a task switchover. As a hack, I
// used this to transition it over. It's a bit messy but whatever.
void Task_InitSpeedchoiceMenu(u8 taskId)
{
    SetMainCallback2(CB2_InitSpeedchoice);
    DestroyTask(taskId);
}

static void DrawHeaderWindow(void);
static void Task_SpeedchoiceMenuFadeIn(u8 taskId);
static void sub_80BB154(void);

extern const u8 *const gFemalePresetNames[];

/*
 * Initialize the default player name similar to how Birch intro does it.
 */
void FormatInitialTempName(u8 nameId)
{
    const u8* name;
    u8 i;

    // who needs male anyway
    name = gFemalePresetNames[nameId];
    for (i = 0; i < 7; i++)
        gTempPlayerName[i] = name[i];
    gTempPlayerName[7] = 0xFF;
}

// Used to signal to avoid redrawing specific stuff. This is used for the naming screen switchover
// and the first time tooltip.
EWRAM_DATA bool32 gAlreadyLoaded = FALSE;

/*
 * Initialize the CB2 Speedchoice Menu.
 */
void CB2_InitSpeedchoice(void)
{
    switch (gMain.state)
    {
    default:
    case 0:
        SetVBlankCallback(NULL);
        gSpeedchoiceTaskId = -1;
        gMain.state++;
        break;
    case 1:
    {
        u8 *addr;
        u32 size;

        addr = (u8 *)VRAM;
        size = 0x18000;
        while (1)
        {
            DmaFill16(3, 0, addr, 0x1000);
            addr += 0x1000;
            size -= 0x1000;
            if (size <= 0x1000)
            {
                DmaFill16(3, 0, addr, size);
                break;
            }
        }
        DmaClear32(3, OAM, OAM_SIZE);
        DmaClear16(3, PLTT, PLTT_SIZE);
        // dont forget to clear BG PLTT RAM you dingus.
        CpuFill16(0, (void*)(BG_PLTT), BG_PLTT_SIZE);
        FillPalette(RGB_BLACK, 0, PLTT_SIZE);
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sSpeedchoiceMenuBgTemplates, ARRAY_COUNT(sSpeedchoiceMenuBgTemplates));
        ChangeBgX(0, 0, 0);
        ChangeBgY(0, 0, 0);
        ChangeBgX(1, 0, 0);
        ChangeBgY(1, 0, 0);
        ChangeBgX(2, 0, 0);
        ChangeBgY(2, 0, 0);
        ChangeBgX(3, 0, 0);
        ChangeBgY(3, 0, 0);
        InitWindows(sSpeedchoiceMenuWinTemplates);
        DeactivateAllTextPrinters();
        SetGpuReg(REG_OFFSET_WIN0H, 0);
        SetGpuReg(REG_OFFSET_WIN0V, 0);
        SetGpuReg(REG_OFFSET_WININ, 5);
        SetGpuReg(REG_OFFSET_WINOUT, 39);
        SetGpuReg(REG_OFFSET_BLDCNT, 193);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 4);
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON | DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
        ShowBg(0);
        ShowBg(1);
        ShowBg(2);
        gMain.state++;
    }
        break;
    case 2:
        ResetPaletteFade();
        ScanlineEffect_Stop();
        ResetTasks();
        ResetSpriteData();
        gMain.state++;
        break;
    case 3:
        LoadBgTiles(1, GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->tiles, 0x120, 0x1A2);
        gMain.state++;
        break;
    case 4:
        LoadPalette(sOptionMenuBg_Pal, 0, sizeof(sOptionMenuBg_Pal));
        LoadPalette(GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->pal, 0x70, 0x20);
        LoadPalette(GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->pal, 0x20, 0x20); // again for the hardcoded border
        gMain.state++;
        break;
    case 5:
        LoadPalette(sOptionMenuText_Pal, 0x10, sizeof(sOptionMenuText_Pal));
        LoadPalette(sMainMenuTextPal, 0xF0, sizeof(sMainMenuTextPal));
        gMain.state++;
        break;
    case 6:
        PutWindowTilemap(0);
        DrawHeaderWindow();
        gMain.state++;
        break;
    case 7:
        gMain.state++;
        break;
    case 8:
        PutWindowTilemap(1);
        //DrawOptionMenuTexts();
        gMain.state++;
    case 9:
        sub_80BB154();
        gMain.state++;
        break;
    case 10:
    {
        gSpeedchoiceTaskId = CreateTask(Task_SpeedchoiceMenuFadeIn, 0);

        gStoredPageNum = 1;

        if(!gAlreadyLoaded)
        {
            StartTimer1();
            gLocalSpeedchoiceConfig.trueIndex = 0;
            gLocalSpeedchoiceConfig.pageIndex = 0;
            gLocalSpeedchoiceConfig.pageNum = 1;
    
            SetOptionChoicesAndConfigFromPreset(gPresetVanilla);
    
            FormatInitialTempName(Random() % 20);
        }
        DrawHeaderWindow();
        DrawPageOptions(gLocalSpeedchoiceConfig.pageNum);

        /*TextSpeed_DrawChoices(gTasks[taskId].data[TD_TEXTSPEED]);
        BattleScene_DrawChoices(gTasks[taskId].data[TD_BATTLESCENE]);
        BattleStyle_DrawChoices(gTasks[taskId].data[TD_BATTLESTYLE]);
        Sound_DrawChoices(gTasks[taskId].data[TD_SOUND]);
        ButtonMode_DrawChoices(gTasks[taskId].data[TD_BUTTONMODE]);
        FrameType_DrawChoices(gTasks[taskId].data[TD_FRAMETYPE]);
        HighlightOptionMenuItem(gTasks[taskId].data[TD_MENUSELECTION]);*/

        HighlightHeaderBox();
        HighlightOptionMenuItem(gLocalSpeedchoiceConfig.pageIndex);
        if(!gAlreadyLoaded)
            PlayBGM(MUS_CONTEST_LOBBY);
        gMain.state++;
        break;
    }
    case 11:
        BeginNormalPaletteFade(-1, 0, 0x10, 0, 0);
        SetVBlankCallback(VBlankCB);
        SetMainCallback2(MainCB2);
        return;
    }
}

// from charmap.txt
#define CHAR_0 0xA1
#define CHAR_PERCENT 0x5B

// Replaces MENUOPTIONCOORDS in speedchoice.h.
#define NEWMENUOPTIONCOORDS(i)  ((i * 16) + 2)

// Deprecated function. Originally was going to be used to select % of experience, I think.
static u8 *FormatPercentNumber(char *text, u8 selection, u8 x, u8 y, u8 style) // style is unused
{
    memcpy(text, gSystemText_TerminatorS, 3); // copy the palette control code.

    // there are no more than 10 pages, so format it as a single digit.
    text[3] = (selection / 100) + CHAR_0; // hundreds
    text[4] = ((selection % 100) / 10) + CHAR_0;    // tens
    text[5] = ((selection % 100) % 10) + CHAR_0; // ones
    text[6] = CHAR_PERCENT;
    text[7] = EOS;
}

// Render the page number.
static void DrawPageChoice(u8 selection)
{
    u8 text[5];

    memcpy(text, gSystemText_TerminatorS, 3);

    text[3] = selection + CHAR_0;
    text[4] = EOS;

    AddTextPrinterParameterized(SPD_WIN_OPTIONS, SPEEDCHOICE_FONT_ID, text, 40, NEWMENUOPTIONCOORDS(5), TEXT_SPEED_FF, NULL);
}

// Render the text for the choices for each option.
static void DrawGeneralChoices(struct SpeedchoiceOption *option, u8 selection, u8 row, bool8 isPercent)
{
    u8 styles[MAX_CHOICES];
    u8 numChoices = option->optionCount;
    u8 i;

    // Format the array.
    if(numChoices < MAX_CHOICES)
    {
        for(i = 0; i < numChoices; i++)
            styles[i] = 0;

        styles[selection] = 1;
    }

    // Arrows needs special handling for positioning the arrows and text.
    if(option->optionType == ARROW)
    {
        u8 text[8];
        s16 x_left = Arrows[0].x;
        s16 x_right = Arrows[1].x;
        s16 y = NEWMENUOPTIONCOORDS(row);
        // perform centering, add 4 pixels for the 8x8 arrow centering
        s16 x_preset = 4 + x_left + (x_right - x_left - GetStringWidth(SPEEDCHOICE_FONT_ID, gPresetNames[gLocalSpeedchoiceConfig.optionConfig[0]], 0)) / 2;

        DrawOptionMenuChoice(Arrows[0].string, x_left, y, 0); // left arrow
        DrawOptionMenuChoice(Arrows[1].string, x_right, y, 0); // right arrow
        DrawOptionMenuChoice(gPresetNames[gLocalSpeedchoiceConfig.optionConfig[0]], x_preset, y, 0);
    }
    // Player name needs special handling as well.
    else if(option->optionType == PLAYER_NAME)
    {
        u8 i;
        u8 bufferedName[6 + PLAYER_NAME_LENGTH + 1]; // account for the color strings
        s16 y = NEWMENUOPTIONCOORDS(row);
        s16 x_left = OptionChoiceConfigPlayerName[0].x;
        s16 x_right = 195; // from Arrows[1].x dont mind me just borrowing
        s16 length = GetStringWidth(SPEEDCHOICE_FONT_ID, gTempPlayerName, 0);
        s16 x_preset = 4 + x_left + (x_right - x_left - length) / 2;
        
        // format the name with {COLOR GREEN}{SHADOW LIGHT_GREEN} first.
        bufferedName[0] = 0xFC;
        bufferedName[1] = 0x01;
        bufferedName[2] = 0x06;
        bufferedName[3] = 0xFC;
        bufferedName[4] = 0x03;
        bufferedName[5] = 0x07;
        // copy the name.
        for(i = 0; i < PLAYER_NAME_LENGTH; i++) {
            bufferedName[i + 6] = gTempPlayerName[i];
        }
        // add terminator.
        bufferedName[13] = EOS;
        DrawOptionMenuChoice(bufferedName, x_preset, y, 0);
    }
    // Assume everything else is a normal option render.
    else
    {
        for(i = 0; i < numChoices; i++)
        {
            s16 x = option->options[i].x;
            s16 y = NEWMENUOPTIONCOORDS(row);
            u8 *string = option->options[i].string;

            DrawOptionMenuChoice(string, x, y, styles[i]);
        }
    }
}

static void Task_SpeedchoiceMenuProcessInput(u8);
static void DrawTooltip(u8 taskId, const u8 *str, int speed, bool32 isYesNo);

/*
 * Fade in until it's time to start processing inputs.
 */
static void Task_SpeedchoiceMenuFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gTasks[taskId].func = Task_SpeedchoiceMenuProcessInput;
    }
}

/*
 * Get the true index of the first or last page option depending on the enum passed.
 */
u8 GetPageOptionTrueIndex(bool8 lastOrFirst, u8 page)
{
    if(lastOrFirst == LAST)
        return (OPTIONS_PER_PAGE * (page - 1)) + GetPageDrawCount(page) - 1;
    else
        return (OPTIONS_PER_PAGE * (page - 1));
}

/*
 * Same as above, but return the page index.
 */
u8 GetPageOptionPageIndex(bool8 lastOrFirst, u8 page)
{
    return (lastOrFirst) ? GetPageDrawCount(page) : 1;
}

extern void ClearMainMenuWindowTilemap(const struct WindowTemplate *template);

/*
 * Finish rendering the tooltip by holding until it has completed rendering.
 */
static void Task_WaitForTooltip(u8 taskId)
{
    RunTextPrinters();

    if(!IsTextPrinterActive(SPD_WIN_TOOLTIP))
    {
        if(gMain.newKeys & A_BUTTON)
        {
            ClearWindowTilemap(SPD_WIN_TOOLTIP);
            ClearMainMenuWindowTilemap((struct WindowTemplate *)&sSpeedchoiceMenuWinTemplates[SPD_WIN_TOOLTIP]);
            DrawPageOptions(gLocalSpeedchoiceConfig.pageNum);
            gTasks[taskId].func = Task_SpeedchoiceMenuProcessInput;
        }
    }
}

extern void DrawMainMenuWindowBorder(const struct WindowTemplate*, u16);

/*
 * Initiate the rendering for the tooltip.
 */
static void DrawTooltip(u8 taskId, const u8 *str, int speed, bool32 isYesNo)
{
    FillWindowPixelBuffer(SPD_WIN_TOOLTIP, 0x11);
    AddTextPrinterParameterized(SPD_WIN_TOOLTIP, SPEEDCHOICE_FONT_ID, str, 0, 1, speed, NULL);
    //sub_8098858(SPD_WIN_TOOLTIP, 0x1D5, 0);
    DrawMainMenuWindowBorder((struct WindowTemplate *)&sSpeedchoiceMenuWinTemplates[SPD_WIN_TOOLTIP], 418);
    PutWindowTilemap(SPD_WIN_TOOLTIP);
    CopyWindowToVram(SPD_WIN_TOOLTIP, 3);
    if(isYesNo)
    {
        FillWindowPixelBuffer(3, 0x11);
        DrawMainMenuWindowBorder((struct WindowTemplate *)&sSpeedchoiceMenuWinTemplates[SPD_WIN_YESNO], 418);
        PutWindowTilemap(3);
        CopyWindowToVram(3, 3);
    }
    SetGpuReg(REG_OFFSET_WIN1H, WIN_RANGE(1, 241));
    SetGpuReg(REG_OFFSET_WIN1V, WIN_RANGE_(114, 160));
    if(!isYesNo)
        gTasks[taskId].func = Task_WaitForTooltip;
}

// Count leading zeroes. Self-explanatory.
u32 CountLeadingZeros(u32 value)
{
    u32 result = 0;

    if (!value)
        return 32;

    while (value < 0x80000000)
    {
        result ++;
        value <<= 1;
    }

    return result;
}

// Convert number of options to bits that are used. Used to assist calculating CV.
u8 GetNumBitsUsed(u8 numOptions)
{
    if(numOptions < 2) { return 1; }
    return 32 - CountLeadingZeros(numOptions - 1);
}

// Calculate the Check Value given a given option configuration. Used for verifying ROM
// check value + option config before starting a run/race.
u32 CalculateCheckValue(u8 taskId)
{
    u32 checkValue;
    u8 i; // current option
    u8 totalBitsUsed;

    // do checkvalue increment for 32-bit value.
    for(checkValue = 0, i = 0, totalBitsUsed = 0; i < CURRENT_OPTIONS_NUM; i++)
    {
        checkValue += gLocalSpeedchoiceConfig.optionConfig[i] << totalBitsUsed;
        totalBitsUsed += GetNumBitsUsed(SpeedchoiceOptions[i].optionCount);
    }

    // seed RNG with checkValue for more hash-like number.
    checkValue = 0x41c64e6d * checkValue + 0x00006073;

    // xor with randomizer value, if one is present.
    checkValue = checkValue ^ gRandomizerCheckValue;

    // get rid of sign extension.
    checkValue = abs(checkValue);

    return checkValue;
}

// Flush the settings to the Save Block.
static void SaveSpeedchoiceOptions(u8 taskId)
{
    u8 i;

    // once again i would prefer to use an extensible for loop here, but the options
    // being bitfields means that it cannot currently be done.
    gSaveBlock2Ptr->speedchoiceConfig.expsystem = gLocalSpeedchoiceConfig.optionConfig[EXPMATH];
    gSaveBlock2Ptr->speedchoiceConfig.plotless = gLocalSpeedchoiceConfig.optionConfig[PLOTLESS];
    gSaveBlock2Ptr->speedchoiceConfig.spinners = gLocalSpeedchoiceConfig.optionConfig[SPINNERS];
    gSaveBlock2Ptr->speedchoiceConfig.maxVision = gLocalSpeedchoiceConfig.optionConfig[MAXVISION];
    gSaveBlock2Ptr->speedchoiceConfig.earlyfly = gLocalSpeedchoiceConfig.optionConfig[EARLYFLY];
    gSaveBlock2Ptr->speedchoiceConfig.goodEarlyWilds = gLocalSpeedchoiceConfig.optionConfig[GOOD_EARLY_WILDS];
    gSaveBlock2Ptr->speedchoiceConfig.earlysurf = gLocalSpeedchoiceConfig.optionConfig[EARLYSURF];
    gSaveBlock2Ptr->speedchoiceConfig.niceMenuOrder = gLocalSpeedchoiceConfig.optionConfig[NICE_MENU_ORDER];
    gSaveBlock2Ptr->speedchoiceConfig.easyFalseSwipe = gLocalSpeedchoiceConfig.optionConfig[EASY_FALSE_SWIPE];
    gSaveBlock2Ptr->speedchoiceConfig.fastCatch = gLocalSpeedchoiceConfig.optionConfig[FAST_CATCH];
    gSaveBlock2Ptr->speedchoiceConfig.gen7XItems = gLocalSpeedchoiceConfig.optionConfig[GEN_7_X_ITEMS];
    gSaveBlock2Ptr->speedchoiceConfig.evoEveryLevel = gLocalSpeedchoiceConfig.optionConfig[EVO_EVERY_LEVEL];

    // write the playername.
    for (i = 0; i < PLAYER_NAME_LENGTH; i++) {
        gSaveBlock2Ptr->playerName[i] = gTempPlayerName[i];
    }
    gSaveBlock2Ptr->playerName[7] = 0xFF;
}

extern const struct BgTemplate sMainMenuBgTemplates[];
extern void Task_NewGameBirchSpeech_Init(u8 taskId);

/*
 * Complete the fade out of the speedchoice menu and then clear the menu data and
 * jump to Birch Intro.
 */
static void Task_SpeedchoiceMenuFadeOut(u8 taskId)
{
    if(!gPaletteFade.active)
    {
        u8 *addr;
        u32 size;

        addr = (u8 *)VRAM;
        size = 0x18000;
        while (1)
        {
            DmaFill16(3, 0, addr, 0x1000);
            addr += 0x1000;
            size -= 0x1000;
            if (size <= 0x1000)
            {
                DmaFill16(3, 0, addr, size);
                break;
            }
        }
        DmaClear32(3, OAM, OAM_SIZE);
        DmaClear16(3, PLTT, PLTT_SIZE);
        gPlttBufferUnfaded[0] = 0;
        gPlttBufferFaded[0] = 0;
        /*ClearWindowTilemap(SPD_WIN_TEXT_OPTION);
        ClearWindowTilemap(SPD_WIN_OPTIONS);
        ClearWindowTilemap(SPD_WIN_TOOLTIP);
        ClearWindowTilemap(3);
        sub_8032250(&sSpeedchoiceMenuWinTemplates[SPD_WIN_TEXT_OPTION]);
        sub_8032250(&sSpeedchoiceMenuWinTemplates[SPD_WIN_OPTIONS]);
        sub_8032250(&sSpeedchoiceMenuWinTemplates[SPD_WIN_TOOLTIP]);
        sub_8032250(&sSpeedchoiceMenuWinTemplates[SPD_WIN_YESNO]);*/
        FreeAllWindowBuffers();
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        SetGpuReg(REG_OFFSET_BG2CNT, 0);
        SetGpuReg(REG_OFFSET_BG1CNT, 0);
        SetGpuReg(REG_OFFSET_BG0CNT, 0);
        SetGpuReg(REG_OFFSET_BG2HOFS, 0);
        SetGpuReg(REG_OFFSET_BG2VOFS, 0);
        SetGpuReg(REG_OFFSET_BG1HOFS, 0);
        SetGpuReg(REG_OFFSET_BG1VOFS, 0);
        SetGpuReg(REG_OFFSET_BG0HOFS, 0);
        SetGpuReg(REG_OFFSET_BG0VOFS, 0);
        ResetBgs();
        InitBgsFromTemplates(0, sMainMenuBgTemplates, 2);
        sInIntro = TRUE;
        sInSubMenu = FALSE;
        sInBattle = FALSE;
        sInField = FALSE;
        gTasks[taskId].func = Task_NewGameBirchSpeech_Init;
    }
}

/*
 * Prompt the Yes/No menu choice to determine if the run/race is started.
 */
static void Task_AskToStartGame(u8 taskId)
{
    switch (Menu_ProcessInputNoWrapClearOnChoose())
    {
    case 0:  // YES
        SeedRngAndSetTrainerId(); // fix Trainer ID potentially being skipped
        PlayBGM(MUS_DUMMY);
        PlaySE(SE_SELECT);
        SaveSpeedchoiceOptions(taskId);
        BeginNormalPaletteFade(-1, 0, 0, 0x10, 0);
        gTasks[taskId].func = Task_SpeedchoiceMenuFadeOut;
        break;
    case 1:  // NO
    case -1: // B button
        PlaySE(SE_SELECT);
        ClearWindowTilemap(SPD_WIN_TOOLTIP);
        ClearWindowTilemap(3);
        ClearMainMenuWindowTilemap((struct WindowTemplate *)&sSpeedchoiceMenuWinTemplates[SPD_WIN_TOOLTIP]);
        ClearMainMenuWindowTilemap((struct WindowTemplate *)&sSpeedchoiceMenuWinTemplates[SPD_WIN_YESNO]);
        DrawPageOptions(gLocalSpeedchoiceConfig.pageNum);
        gTasks[taskId].func = Task_SpeedchoiceMenuProcessInput;
        break;
    }
}

/*
 * This might be incorrectly named, but this is the function to initialize the Yes/No for start game.
 * The flushing of the options to the Save Block may have been done here originally.
 */
static void Task_SpeedchoiceMenuSave(u8 taskId)
{
    ConvertIntToHexStringN(gStringVar1, CalculateCheckValue(taskId), STR_CONV_MODE_LEADING_ZEROS, 8);
    StringExpandPlaceholders(gStringVar4, gSpeedchoiceStartGameText);
    DrawTooltip(taskId, gStringVar4, TEXT_SPEED_FF, TRUE); // a bit of a hack, but whatever.
    CreateYesNoMenu(&sSpeedchoiceMenuWinTemplates[SPD_WIN_YESNO], 418, 2, 0);

    gTasks[taskId].func = Task_AskToStartGame;
}

/*
 * Fade out Task function to initialize naming screen.
 */
void Task_SpeedchoiceMenuFadeOutToNamingScreen(u8 taskId)
{
    if(!gPaletteFade.active)
    {
        FreeAllWindowBuffers();
        DestroyTask(gSpeedchoiceTaskId);
        DoNamingScreen(NAMING_SCREEN_PLAYER, gTempPlayerName, 1, 0, 0, CB2_InitSpeedchoice);
    }
}

/*
 * General processor for the input on the speedchoice menu.
 */
static void Task_SpeedchoiceMenuProcessInput(u8 taskId)
{
	if(!gAlreadyLoaded)
	{
		DrawTooltip(taskId, gSpeedchoiceTooltipExplanation, GetPlayerTextSpeed(), FALSE);
		gAlreadyLoaded = TRUE;
	}
    else if (gMain.newKeys & A_BUTTON)
    {
        if (gLocalSpeedchoiceConfig.trueIndex == START_GAME)
            gTasks[taskId].func = Task_SpeedchoiceMenuSave;
        else if (gLocalSpeedchoiceConfig.trueIndex == PRESET) {
            SetOptionChoicesAndConfigFromPreset(GetPresetPtr(gLocalSpeedchoiceConfig.optionConfig[PRESET]));
            PlaySE(SE_SELECT); // page scrolling.
            gForceUpdate = TRUE;
        }
        else if (gLocalSpeedchoiceConfig.trueIndex == PLAYER_NAME_SET) {
            BeginNormalPaletteFade(-1, 0, 0, 0x10, 0);
            gTasks[taskId].func = Task_SpeedchoiceMenuFadeOutToNamingScreen;
            PlaySE(SE_SELECT); // page scrolling.
        }
    }
    else if (gMain.newKeys & SELECT_BUTTON) // do tooltip.
    {
        if(gLocalSpeedchoiceConfig.trueIndex <= CURRENT_OPTIONS_NUM && SpeedchoiceOptions[gLocalSpeedchoiceConfig.trueIndex].tooltip != NULL)
            DrawTooltip(taskId, SpeedchoiceOptions[gLocalSpeedchoiceConfig.trueIndex].tooltip, GetPlayerTextSpeed(), FALSE);
    }
    else if (gMain.newKeys & DPAD_UP)
    {
        if(gLocalSpeedchoiceConfig.trueIndex == PAGE)
            gLocalSpeedchoiceConfig.trueIndex = GetPageOptionTrueIndex(LAST, gLocalSpeedchoiceConfig.pageNum); // set the entry to the last available option.
        else if(gLocalSpeedchoiceConfig.trueIndex > GetPageOptionTrueIndex(FIRST, gLocalSpeedchoiceConfig.pageNum))
            gLocalSpeedchoiceConfig.trueIndex--;
        else
            gLocalSpeedchoiceConfig.trueIndex = START_GAME;

        SetPageIndexFromTrueIndex(taskId, gLocalSpeedchoiceConfig.trueIndex);
        HighlightOptionMenuItem(gLocalSpeedchoiceConfig.pageIndex);
    }
    else if (gMain.newKeys & DPAD_DOWN)
    {
        if(gLocalSpeedchoiceConfig.trueIndex == GetPageOptionTrueIndex(LAST, gLocalSpeedchoiceConfig.pageNum))
            gLocalSpeedchoiceConfig.trueIndex = PAGE; // you are at the last option when you press down, go to page index.
        else if(gLocalSpeedchoiceConfig.trueIndex == START_GAME)
            gLocalSpeedchoiceConfig.trueIndex = GetPageOptionTrueIndex(FIRST, gLocalSpeedchoiceConfig.pageNum);
        else
            gLocalSpeedchoiceConfig.trueIndex++;

        SetPageIndexFromTrueIndex(taskId, gLocalSpeedchoiceConfig.trueIndex);
        HighlightOptionMenuItem(gLocalSpeedchoiceConfig.pageIndex);
    }
    else
    {
        u8 trueIndex = gLocalSpeedchoiceConfig.trueIndex;
        u8 selection = gLocalSpeedchoiceConfig.optionConfig[trueIndex];
        switch (trueIndex)
        {
            default:
                if(trueIndex < CURRENT_OPTIONS_NUM)
                {
                    // lol. I don't know why I hardcoded this in Sapphire.
                    //if(trueIndex == NERFROXANNE)
                    //    gLocalSpeedchoiceConfig.optionConfig[trueIndex] = ProcessGeneralInput((struct SpeedchoiceOption *)&SpeedchoiceOptions[trueIndex], selection, TRUE);
                    //else
                    u8 oldSelection = gLocalSpeedchoiceConfig.optionConfig[trueIndex];
                    gLocalSpeedchoiceConfig.optionConfig[trueIndex] = ProcessGeneralInput((struct SpeedchoiceOption *)&SpeedchoiceOptions[trueIndex], selection, FALSE);
                    DrawGeneralChoices((struct SpeedchoiceOption *)&SpeedchoiceOptions[trueIndex], gLocalSpeedchoiceConfig.optionConfig[trueIndex], gLocalSpeedchoiceConfig.pageIndex, FALSE);
                    if(oldSelection != gLocalSpeedchoiceConfig.optionConfig[trueIndex] || gForceUpdate) {
                        DrawPageOptions(gLocalSpeedchoiceConfig.pageNum); // HACK!!! The page has to redraw. But only redraw it if the selection changed, otherwise it lags.
                        gForceUpdate = FALSE;
                    }
                }
                break;
            case PAGE:
                gLocalSpeedchoiceConfig.pageNum = ProcessGeneralInput((struct SpeedchoiceOption *)&SpeedchoiceOptions[CURRENT_OPTIONS_NUM], gLocalSpeedchoiceConfig.pageNum, TRUE);
                //DrawPageChoice(gLocalSpeedchoiceConfig.pageNum); Deprecated.
                if(gLocalSpeedchoiceConfig.pageNum != gStoredPageNum) // only redraw if the page updates!
                {
                    DrawPageOptions(gLocalSpeedchoiceConfig.pageNum);
                    gStoredPageNum = gLocalSpeedchoiceConfig.pageNum; // update the page.
                }
                break;
            case START_GAME:
                break;
        }
    }
    gTasks[taskId].data[0]++; // arrow timer
}

// Self-explanatory. Draws the header window.
static void DrawHeaderWindow(void)
{
    s32 width;
    
    FillWindowPixelBuffer(SPD_WIN_TEXT_OPTION, 0x11);
    AddTextPrinterParameterized(SPD_WIN_TEXT_OPTION, SPEEDCHOICE_FONT_ID, gSpeedchoiceTextHeader, 4, 1, TEXT_SPEED_FF, NULL);
    width = GetStringWidth(SPEEDCHOICE_FONT_ID, gSpeedchoiceCurrentVersion, GetFontAttribute(SPEEDCHOICE_FONT_ID, FONTATTR_LETTER_SPACING));
    AddTextPrinterParameterized(SPD_WIN_TEXT_OPTION, SPEEDCHOICE_FONT_ID, gSpeedchoiceCurrentVersion, 204 - width, 1, TEXT_SPEED_FF, NULL);
    CopyWindowToVram(SPD_WIN_TEXT_OPTION, 3);
}

// Renders the frame for the options and choices window.
void DrawOptionsAndChoicesWindow(void)
{
    FillWindowPixelBuffer(SPD_WIN_OPTIONS, 0x11);
    CopyWindowToVram(SPD_WIN_OPTIONS, 3);
}

// Self-explanatory.
u8 GetPageDrawCount(u8 page)
{
    if ((page * OPTIONS_PER_PAGE) > CURRENT_OPTIONS_NUM)
        return CURRENT_OPTIONS_NUM % OPTIONS_PER_PAGE;

    return OPTIONS_PER_PAGE;
}

/*
 * Given a page number, renders the page options.
 */
void DrawPageOptions(u8 page) // Page is 1-indexed
{
    u8 i;
    u8 drawCount = GetPageDrawCount(page);

    FillWindowPixelBuffer(SPD_WIN_OPTIONS, 0x11);

    // print page options.
    for(i = 0; i < drawCount; i++)
    {
        struct SpeedchoiceOption *option = (struct SpeedchoiceOption *)&SpeedchoiceOptions[i + (OPTIONS_PER_PAGE * (page - 1))];
        const u8 *string = option->string;

        AddTextPrinterParameterized(SPD_WIN_OPTIONS, SPEEDCHOICE_FONT_ID, string, 4, NEWMENUOPTIONCOORDS(i), TEXT_SPEED_FF, NULL);
        // TODO: Draw on SPD_WIN_OPTIONS, if it's broken
        DrawGeneralChoices(option, gLocalSpeedchoiceConfig.optionConfig[i + ((page-1) * 5)], i, FALSE);
    }

    AddTextPrinterParameterized(SPD_WIN_OPTIONS, SPEEDCHOICE_FONT_ID, gSpeedchoiceOptionPage, 4, NEWMENUOPTIONCOORDS(5), TEXT_SPEED_FF, NULL);
    AddTextPrinterParameterized(SPD_WIN_OPTIONS, SPEEDCHOICE_FONT_ID, gSpeedchoiceOptionStartGame, 4, NEWMENUOPTIONCOORDS(6), TEXT_SPEED_FF, NULL);
    DrawPageChoice(gLocalSpeedchoiceConfig.pageNum);
    CopyWindowToVram(SPD_WIN_OPTIONS, 3);
}

/*
 * Given a true index, convert it to the page index so that the general processor function can
 * set the correctly newly selected option.
 */
void SetPageIndexFromTrueIndex(u8 taskId, s16 index) // data is s16.
{
    if(index == PAGE)
        gLocalSpeedchoiceConfig.pageIndex = 5;
    else if(index == START_GAME)
        gLocalSpeedchoiceConfig.pageIndex = 6;
    else
        gLocalSpeedchoiceConfig.pageIndex = (oldmin((index % OPTIONS_PER_PAGE), OPTIONS_PER_PAGE));
}

// Copied from option menu. Fills the window frames.
static void sub_80BB154(void)
{
    //                   bg, tileNum, x,    y,    width, height,  pal
    FillBgTilemapBufferRect(1, 0x1A2, 1,    0,      1,      1,      7);
    FillBgTilemapBufferRect(1, 0x1A3, 2,    0,      0x1B,   1,      7);
    FillBgTilemapBufferRect(1, 0x1A4, 28,   0,      1,      1,      7);
    FillBgTilemapBufferRect(1, 0x1A5, 1,    1,      1,      2,      7);
    FillBgTilemapBufferRect(1, 0x1A7, 28,   1,      1,      2,      7);
    FillBgTilemapBufferRect(1, 0x1A8, 1,    3,      1,      1,      7);
    FillBgTilemapBufferRect(1, 0x1A9, 2,    3,      0x1B,   1,      7);
    FillBgTilemapBufferRect(1, 0x1AA, 28,   3,      1,      1,      7);
    FillBgTilemapBufferRect(1, 0x1A2, 1,    4,      1,      1,      7);
    FillBgTilemapBufferRect(1, 0x1A3, 2,    4,      0x1A,   1,      7);
    FillBgTilemapBufferRect(1, 0x1A4, 28,   4,      1,      1,      7);
    FillBgTilemapBufferRect(1, 0x1A5, 1,    5,      1,      0x12,   7);
    FillBgTilemapBufferRect(1, 0x1A7, 28,   5,      1,      0x12,   7);
    FillBgTilemapBufferRect(1, 0x1A8, 1,    19,     1,      1,      7);
    FillBgTilemapBufferRect(1, 0x1A9, 2,    19,     0x1A,   1,      7);
    FillBgTilemapBufferRect(1, 0x1AA, 28,   19,     1,      1,      7);

    CopyBgTilemapBufferToVram(1);
}
