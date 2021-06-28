#include "global.h"
#include "time_events.h"
#include "event_data.h"
#include "field_weather.h"
#include "pokemon.h"
#include "random.h"
#include "overworld.h"
#include "rtc.h"
#include "script.h"
#include "task.h"
#include "speedchoice.h"

static u32 GetMirageRnd(void)
{
    u32 hi = VarGet(VAR_MIRAGE_RND_H);
    u32 lo = VarGet(VAR_MIRAGE_RND_L);
    return (hi << 16) | lo;
}

static void SetMirageRnd(u32 rnd)
{
    VarSet(VAR_MIRAGE_RND_H, rnd >> 16);
    VarSet(VAR_MIRAGE_RND_L, rnd);
}

// unused
void InitMirageRnd(void)
{
    SetMirageRnd((Random() << 16) | Random());
}

void UpdateMirageRnd(u16 days)
{
    s32 rnd = GetMirageRnd();
    while (days)
    {
        rnd = ISO_RANDOMIZE2(rnd);
        days--;
    }
    SetMirageRnd(rnd);
}

bool8 IsMirageIslandPresent(void)
{
    //if(CheckSpeedchoiceOption(MEME_ISLAND, MEME_BIG) == TRUE)
        return TRUE; // always present.
    /*
    else
    {
        u16 rnd = GetMirageRnd() >> 16;
        int i;
    
        for (i = 0; i < PARTY_SIZE; i++)
            if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) && (GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY) & 0xFFFF) == rnd)
                return TRUE;
    
        return FALSE;
    }
    */
}

void UpdateShoalTideFlag(void)
{
    #define LOW_TIDE 0
    #define HIGH_TIDE 1
    static const u8 tide[] =
    {
        HIGH_TIDE, // 00 (NIGHT)
        HIGH_TIDE, // 01 (NIGHT)
        HIGH_TIDE, // 02 (NIGHT)
        LOW_TIDE,  // 03 (NIGHT)
        LOW_TIDE,  // 04 (MORNING)
        LOW_TIDE,  // 05 (MORNING)
        LOW_TIDE,  // 06 (MORNING)
        LOW_TIDE,  // 07 (MORNING)
        LOW_TIDE,  // 08 (MORNING)
        HIGH_TIDE, // 09 (MORNING)
        HIGH_TIDE, // 10 (DAY)
        HIGH_TIDE, // 11 (DAY)
        HIGH_TIDE, // 12 (DAY)
        HIGH_TIDE, // 13 (DAY)
        HIGH_TIDE, // 14 (DAY)
        LOW_TIDE,  // 15 (DAY)
        LOW_TIDE,  // 16 (DAY)
        LOW_TIDE,  // 17 (DAY)
        LOW_TIDE,  // 18 (DAY)
        LOW_TIDE,  // 19 (DAY)
        LOW_TIDE,  // 20 (NIGHT)
        HIGH_TIDE, // 21 (NIGHT)
        HIGH_TIDE, // 22 (NIGHT)
        HIGH_TIDE, // 23 (NIGHT)
    };
    #undef LOW_TIDE
    #undef HIGH_TIDE

    if (IsMapTypeOutdoors(GetLastUsedWarpMapType()))
    {
        RtcCalcLocalTime();
        if (tide[gLocalTime.hours])
            FlagSet(FLAG_SYS_SHOAL_TIDE);
        else
            FlagClear(FLAG_SYS_SHOAL_TIDE);
    }
}

static void Task_WaitWeather(u8 taskId)
{
    if (IsWeatherChangeComplete())
    {
        EnableBothScriptContexts();
        DestroyTask(taskId);
    }
}

void WaitWeather(void)
{
    CreateTask(Task_WaitWeather, 80);
}

void InitBirchState(void)
{
    *GetVarPointer(VAR_BIRCH_STATE) = 0;
}

void UpdateBirchState(u16 days)
{
    u16 *state = GetVarPointer(VAR_BIRCH_STATE);
    *state += days;
    *state %= 7;
}
