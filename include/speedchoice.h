#ifndef _GUARD_SPEEDCHOICE_H
#define _GUARD_SPEEDCHOICE_H

// =======================================
// Global Speedchoice Configuration
// =======================================

// The current total number of selectable options. Be sure to count preset too and
// player name.
#define CURRENT_OPTIONS_NUM 15

// Maximum number of possible selectable options per option.
#define MAX_CHOICES 6

// Maximum number of options to print per page besides Start Game and Page.
#define OPTIONS_PER_PAGE 5

// Maximum number of options including Page and Start Game.
#define ALLOPTIONS_PER_PAGE OPTIONS_PER_PAGE + 2

// Number of defined pages.
#define MAX_PAGES 3

// Deprecated macro. May have been in use for Sapphire Speedchoice when the window system
// was different back then.
#define MENUOPTIONCOORDS(i) (5 + (2 * i)) // DEPRECATED

// Deprecated macro. Using Random() formula for spinner timer: this define isnt used to
// check Spinner timing anymore.
#define SPINNER_HELL_TIMER 4 // DEPRECATED

// Maximum number of tile vision trainers have when Max Vision is set.
#define MAX_VISION_RANGE 8

// Number of frames per spinner iteration in Purge setting for Spinners.
#define PURGE_SPINNER_TIMER 48

// Deprecated macro. I think battle speed is defined seperately now in the battle_main file
// somewhere.
#define BATTLE_SPEED_FRACTION 4 // DEPRECATED

// Reserved macros for a future version for a wild encounter queue system.
// Not implemented yet.
#define GRASS_QUEUE_SIZE 20
#define WATER_QUEUE_SIZE 15
#define ROCKSMASH_QUEUE_SIZE 15
#define FISHING_QUEUE_SIZE 5

// To Avoid breaking stuff, we used old min/max macros. I'm too lazy to correct this.
#define oldmin(a, b) (a < b ? a : b)
#define oldmax(a, b) (a > b ? a : b)

// The chosen font ID for Speedchoice.
#define SPEEDCHOICE_FONT_ID 1

// Enumeration for Speedchoice Options. Used for CheckSpeedchoiceOption. These options
// MUST match the order of the save data struct in global.h!

/**
 * The following Speedchoice options have been deprecated:
 *
 * Instant Text (moved to Options)
 * Nerf Roxanne (made default)
 * Super Bike (made default)
 * New Wild Encounters (made default)
 * Run Everywhere (made default)
 * Meme Island (made default)
 * Better Marts (made default)
 * Early Bike (made default)
 * Fast Egg Hatch (made default)
 *
 * Please ensure when rebasing these are readded per implementation.
 */
enum
{
    // ----------------------
    // NORMAL OPTIONS
    // ----------------------
    PRESET,
    PLAYER_NAME_SET,
    EXPMATH,
    PLOTLESS,
    //INSTANTTEXT, // DEPRECATED (default)
    SPINNERS,
    MAXVISION,
    //NERFROXANNE, // DEPRECATED (default)
    //SUPERBIKE, // DEPRECATED (default)
    //NEWWILDENC, // DEPRECATED (default)
    EARLYFLY,
    //RUN_EVERYWHERE, // DEPRECATED (default)
    //MEME_ISLAND, // DEPRECATED (default)
    //BETTER_MARTS, // DEPRECATED (default)
    GOOD_EARLY_WILDS,
    EARLYSURF,
    NICE_MENU_ORDER,
    EASY_FALSE_SWIPE,
    FAST_CATCH,
    //EARLY_BIKE, // DEPRECATED (default)
    //FAST_EGG_HATCH, // DEPRECATED (default)
    GEN_7_X_ITEMS,
    EVO_EVERY_LEVEL,
    INVERSE_BATTLES,

    // ----------------------
    // STATIC OPTIONS
    // ----------------------
    PAGE,
    START_GAME
};

/*
 * Enumerations for GetPageOptionTrueIndex. When passing this, the function will
 * return the true index from that page given whether you are specifying the first
 * option of the page or the last one.
 */
enum
{
    FIRST,
    LAST
};

// We used to share the enums for options, but we don't anymore because it's confusing
// as fuck. Please define enums for each option and have them match the option config.

// ----------------------
// EXP ENUM
// ----------------------
enum
{
    EXP_KEEP,
    EXP_BW,
    EXP_NONE
};

// ----------------------
// PLOTLESS ENUM
// ----------------------
enum
{
    PLOT_SEMI,
    PLOT_KEEP,
    PLOT_FULL
};

// ----------------------
// INSTANT TEXT ENUM (DEPRECATED)
// ----------------------
enum
{
    IT_ON,
    IT_OFF
};

// ----------------------
// SPINNERS ENUM
// ----------------------
enum
{
    SPIN_NERF,
    SPIN_KEEP,
    SPIN_HELL,
    SPIN_WHY
};

// ----------------------
// MAX VISION ENUM
// ----------------------
enum
{
    MAX_OFF,
    MAX_SANE,
    MAX_HELL
};

// ----------------------
// NERF ROXANNE ENUM (DEPRECATED)
// ----------------------
enum
{
    NERF_YES,
    NERF_NO
};

// ----------------------
// SUPER BIKE ENUM (DEPRECATED)
// ----------------------
enum
{
    BIKE_ON,
    BIKE_OFF
};

// ----------------------
// NEW WILD ENC ENUM (DEPRECATED)
// ----------------------
enum
{
    NEW_ON,
    NEW_OFF
};

// ----------------------
// EARLY FLY ENUM
// ----------------------
enum
{
    FLY_YES,
    FLY_NO
};

// ----------------------
// RUN EVERYWHERE ENUM (DEPRECATED)
// ----------------------
enum
{
    RUN_ON,
    RUN_OFF
};

// ----------------------
// MEME ISLAND ENUM (DEPRECATED)
// ----------------------
enum
{
    MEME_BIG, // dot net
    MEME_SMALL // you're banned
};

// ----------------------
// BETTER MARTS ENUM (DEPRECATED)
// ----------------------
enum
{
    MARTS_ON,
    MARTS_OFF
};

// ----------------------
// GOOD EARLY WILDS ENUM
// ----------------------
enum
{
    GOOD_OFF,
    GOOD_STATIC,
    GOOD_RAND
};

// ----------------------
// EARLY SURF ENUM
// ----------------------
enum
{
    SURF_ON,
    SURF_OFF
};

// ----------------------
// PARTY MENU ENUM
// ----------------------
enum
{
    NICE_MENU_ORDER_ON,
    NICE_MENU_ORDER_OFF
};

// ----------------------
// EASY FALSE SWIPE ENUM
// ----------------------
enum
{
    EASY_FALSE_SWIPE_OFF,
    EASY_FALSE_SWIPE_TUTOR,
    EASY_FALSE_SWIPE_HM05
};

// ----------------------
// FAST CATCH ENUM
// ----------------------
enum
{
    FAST_CATCH_ON,
    FAST_CATCH_OFF
};

// ----------------------
// EARLY BIKE ENUM (DEPRECATED)
// ----------------------
enum
{
    EARLY_BIKE_YES,
    EARLY_BIKE_NO
};

// ----------------------
// FAST EGG HATCHING ENUM (DEPRECATED)
// ----------------------
enum
{
    FAST_EGG_HATCH_YES,
    FAST_EGG_HATCH_NO
};
// ----------------------
// GEN 7 X ITEMS ENUM
// ----------------------
enum
{
    GEN_7_X_ITEMS_ON,
    GEN_7_X_ITEMS_OFF
};

// ----------------------
// EVO_EVERY_LEVEL ENUM
// ----------------------
enum
{
    EVO_EV_ON,
    EVO_EV_OFF
};

// ----------------------
// INVERSE_BATTLES ENUM
// ----------------------
enum
{
    INV_BATTLES_ON,
    INV_BATTLES_OFF
};

// Enumeration for optionType in the Speedchoice struct below.
enum
{
    NORMAL,
    ARROW,
    PLAYER_NAME
};

/*
 * Every Speedchoice option uses a configuration to determine how to render it's options.
 * For example (Yes/No), (On/Off), etc. These Configurations determine each option's coordinate
 * to use and the string to use. Ideally I could probably do lots of fancy math to determine
 * the X as well but I'd rather just hardcode it for detailing.
 */
struct OptionChoiceConfig
{
    s16 x; // do not store the Y coordinate. Y is automatically calculated depending on the row the option is placed and therefore is not necessary.
    u8 *string; // can be null
};

/*
 * To define an option, this global configuration struct is used to define every option that is
 * selectable or togglable.
 */
struct SpeedchoiceOption
{
    // Number of options. (2 = On/Off, etc)
    u8 optionCount;
    
    // see option type enumeration above.
    u8 optionType;
    
    // Pointer to the name of the option.
    const u8 *string;
    
    // Pointer to the array of Option configuration array. An array of MAX_CHOICES since no
    // need to define more than that. Use NULL for static options.
    const struct OptionChoiceConfig *options;
    
    // Pointer to Tooltip string. Printed when SELECT is pressed on the option.
    const u8 *tooltip;
    
    // Determines if the option is enabled or not. This was used during Speedchoice development.
    bool8 enabled;
}; 

/*
 * Before writing to the Save Block, we store the configuration in RAM temporarily before
 * flushing it to the Save Block.
 */
struct SpeedchoiceConfigStruct
{
    // The currently stored selected options per Speedchoice option. These are written
    // to the save block at the end.
    u8 optionConfig[CURRENT_OPTIONS_NUM];
    
    // The true ID of the selected page. This takes into account number of pages, etc.
    // If you're on Page 2, the trueIndex of the first option is 8. (7+1)
    u8 trueIndex;
    
    // The page ID of the option on the current page. 2nd option on page 2 is 2, etc.
    u8 pageIndex;
    
    // Current page number.
    u8 pageNum;
};

/*
 * In Gen 3, there is an oversight by Game Freak where upon going to a sub menu and returning
 * the spinner timers are reset due to reloading the field. Since spinners dont start trying
 * to spin until later, this oversight can be used to run past spinners safely without
 * encountering them. Because this trivializes Spinner Hell, we fix this when Hell/Why is
 * toggled by backing up the NPC timers and restoring them upon returning to the field. We skip
 * the player object's timer to avoid sprite issues.
 */
struct MapObjectTimerBackup
{
    bool8 backedUp;
    s16 spriteId;
    s16 timer;
};

/*
 * we need to extern the randomizer check value. If the ROM could see the value assigned
 * here (which is 0 by default), then it would be optimized out, but we need this location
 * to be externally written to by the randomizer.
 */
extern u32 gRandomizerCheckValue;

// ----------------------
// Prototypes
// ----------------------
void CB2_InitSpeedchoiceMenu(void);
bool8 CheckSpeedchoiceOption(u8, u8);

#endif // _GUARD_SPEEDCHOICE_H
