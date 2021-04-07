#ifndef _GUARD_DONE_BUTTON_H
#define _GUARD_DONE_BUTTON_H

// The done stat fields are defined in global.h.

// This is kept seperate and then loaded when the save is loaded.
struct FrameTimers
{
    u32 frameCount;  
    u32 owFrameCount;
    u32 battleFrameCount;
    u32 menuFrameCount;
    u32 introsFrameCount;
};

enum DoneButtonStat
{
    // DoneButtonStats1
    // These frame counters are accounted for by a seperate struct.
    DB_NO_STAT, // used for struct definitions.
    /*accounted*/ DB_FRAME_COUNT_TOTAL,
    /*accounted*/ DB_FRAME_COUNT_OW,
    /*accounted*/ DB_FRAME_COUNT_BATTLE,
    /*accounted*/ DB_FRAME_COUNT_MENU, // count start menu + PC
    /*accounted*/ DB_FRAME_COUNT_INTROS,
    /*accounted*/ DB_SAVE_COUNT,
    /*accounted*/ DB_RELOAD_COUNT,
    /*accounted*/ DB_CLOCK_RESET_COUNT,
    /*accounted*/ DB_STEP_COUNT,
    /*accounted*/ DB_STEP_COUNT_WALK,
    /*accounted*/ DB_STEP_COUNT_SURF,
    /*accounted*/ DB_STEP_COUNT_BIKE,
    /*accounted*/ DB_STEP_COUNT_RUN, // Gen 3 exclusive
    /*accounted*/ DB_BONKS,
    /*accounted*/ DB_TOTAL_DAMAGE_DEALT, 
    /*accounted*/ DB_ACTUAL_DAMAGE_DEALT, // regardless of max
    /*accounted*/ DB_TOTAL_DAMAGE_TAKEN,
    /*accounted*/ DB_ACTUAL_DAMAGE_TAKEN, // regardless of max
    /*accounted*/ DB_OWN_MOVES_HIT,
    /*accounted*/ DB_OWN_MOVES_MISSED,
    /*accounted*/ DB_ENEMY_MOVES_HIT,
    /*accounted*/ DB_ENEMY_MOVES_MISSED,
    // DoneButtonStats2
    /*accounted*/ DB_OWN_MOVES_SE,
    /*accounted*/ DB_OWN_MOVES_NVE,
    /*accounted*/ DB_ENEMY_MOVES_SE,
    /*accounted*/ DB_ENEMY_MOVES_NVE,
    /*accounted*/ DB_CRITS_DEALT,
    /*accounted*/ DB_OHKOS_DEALT,
    /*accounted*/ DB_CRITS_TAKEN,
    /*accounted*/ DB_OHKOS_TAKEN,
    /*accounted*/ DB_PLAYER_HP_HEALED,
    /*accounted*/ DB_ENEMY_HP_HEALED,
    /*accounted*/ DB_PLAYER_POKEMON_FAINTED,
    /*accounted*/ DB_ENEMY_POKEMON_FAINTED,
    /*accounted*/ DB_EXP_GAINED,
    /*accounted*/ DB_SWITCHOUTS,
    /*accounted*/ DB_BATTLES,
    /*accounted*/ DB_TRAINER_BATTLES,
    /*accounted*/ DB_WILD_BATTLES,
    /*accounted*/ DB_BATTLES_FLED,
    /*accounted*/ DB_FAILED_RUNS,
    /*accounted*/ DB_MONEY_MADE,
    /*accounted*/ DB_MONEY_SPENT,
    /*accounted*/ DB_MONEY_LOST,
    /*accounted*/ DB_ITEMS_PICKED_UP,
    /*accounted*/ DB_ITEMS_BOUGHT,
    /*accounted*/ DB_ITEMS_SOLD,
    /*accounted*/ DB_MOVES_LEARNT,
    /*accounted*/ DB_BALLS_THROWN,
    /*accounted*/ DB_POKEMON_CAUGHT_IN_BALLS,
    /*accounted*/ DB_EVOLUTIONS_ATTEMPTED,
    /*accounted*/ DB_EVOLUTIONS_COMPLETED,
    /*accounted*/ DB_EVOLUTIONS_CANCELLED
};

void TryIncrementButtonStat(enum DoneButtonStat stat);
void TryAddButtonStatBy(enum DoneButtonStat stat, u32 add);
u32 GetDoneButtonStat(enum DoneButtonStat stat);
void Task_InitDoneButtonMenu(u8 taskId);

extern struct FrameTimers gFrameTimers;
extern bool8 sInSubMenu;
extern bool8 sInBattle;
extern bool8 sInField;
extern bool8 sInIntro;

#endif // _GUARD_DONE_BUTTON_H
