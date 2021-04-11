#include <stdarg.h>
#include "global.h"
#include "emu_accuracy_tests.h"
#include "boot_error_screen.h"
#include "string_util.h"
#include "text.h"

extern const char NESPipelineTest_Internal[];
extern const char NESPipelineTest_Internal_End[];
extern const char TimerPrescalerTest[];
extern const char TimerPrescalerTest_End[];

u16 SetIME(u16 c) {
    u16 backupIME = REG_IME;
    REG_IME = c;
    return backupIME;
}

s32 DoTest(const char * start, const char * end, u32 expectedValue, ...)
{
    u32 * d;
    const u32 * s;
    u8 buffer[(size_t)(end - start)];
    va_list va_args;
    u32 resp;
    va_start(va_args, expectedValue);

    d = (u32 *)buffer;
    s = (const u32 *)start;
    while (s < (const u32 *)end)
        *d++ = *s++;
    resp = ((u32 (*)(va_list))buffer)(va_args);
    return resp == expectedValue;
}

extern u32 PrefetchBufferResult_Func(void);
extern u32 PrefetchBufferResult_Func_End(void); // not a func but needed to shut compiler up

s32 PrefetchBufferTest(void)
{
    s32 result;
    u16 waitCntBackup;
    u16 imeBak;
    u32 prefetch;

    result = 0;
    imeBak = SetIME(0);
    waitCntBackup = REG_WAITCNT;
    REG_WAITCNT = 0x4014;
    prefetch = PrefetchBufferResult_Func();
    if ( prefetch != 24 )
        result |= 1;
    REG_WAITCNT = 0x14;
    prefetch = PrefetchBufferResult_Func();
    if ( prefetch != 51 )
        result |= 2;
    REG_WAITCNT = waitCntBackup;
    SetIME(imeBak);
    return (result != 0) ? 0 : 1; // If result isnt 0, it means it failed. Return the correct error status.
}

s32 NESPipelineTest(void)
{
    return DoTest(
        NESPipelineTest_Internal,
        NESPipelineTest_Internal_End,
        255
    );
}

s32 TimingTest(void)
{
    s32 i, j;
    u32 failMask = 0;
    const u32 expected[] = {
        [TIMER_1CLK]    = 4096,
        [TIMER_64CLK]   = 64,
        [TIMER_256CLK]  = 16,
        [TIMER_1024CLK] = 4
    };
    u32 flagNo = 0;
    u16 imeBak = REG_IME;
    REG_IME = 0;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (!DoTest(
                TimerPrescalerTest,
                TimerPrescalerTest_End,
                expected[j],
                &REG_TMCNT(i),
                ((j | TIMER_ENABLE) << 16)
            ))
                failMask |= 1 << flagNo;
            flagNo++;
        }
    }

    REG_IME = imeBak;
    return failMask == 0;
}

static const u8 sText_InsnPrefetch[] = _("Insn Prefetch");
static const u8 sText_TimerPrescaler[] = _("Timer Prescaler");
static const u8 sText_PrefetchBuffer[] = _("Prefetch Buffer");

static const struct TestSpec {
    const u8 * name;
    s32 (*const func)(void);
} sTestSpecs[] = {
    {sText_InsnPrefetch, NESPipelineTest},
    {sText_TimerPrescaler, TimingTest},
    //{sText_PrefetchBuffer, PrefetchBufferTest},
};

void RunEmulationAccuracyTests(void)
{
    s32 i;
    s32 c;
    u8 * ptr;
    gWhichErrorMessage = FATAL_OKAY;
    gWhichTestFailed[0] = EOS;
    ptr = gWhichTestFailed;
    for (i = 0, c = 0; i < NELEMS(sTestSpecs); i++)
    {
        if (!sTestSpecs[i].func())
        {
            gWhichErrorMessage = FATAL_ACCU_FAIL;
            if (c % 2)
            {
                s32 x = 204 - GetStringWidth(2, sTestSpecs[i].name, 1);
                *ptr++ = EXT_CTRL_CODE_BEGIN;
                *ptr++ = EXT_CTRL_CODE_CLEAR_TO;
                *ptr++ = x;
                ptr = StringCopy(ptr, sTestSpecs[i].name);
                *ptr++ = CHAR_NEWLINE;
                *ptr = EOS;
            }
            else
            {
                ptr = StringCopy(ptr, sTestSpecs[i].name);
            }
            c++;
        }
    }
}
