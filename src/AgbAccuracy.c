#include <stdarg.h>
#include "global.h"
#include "AgbAccuracy.h"

// Standalone emulator test in a single file/header. Can be compiled stand alone, just run 
// RunAgbAccuracyTests to get the result as a return. Read the header file for the
// documentation. Assumes the standard GBA SDK definitions.

// Authors: PikalaxALT, RevoSucks
// Credit to the AGS Aging Test cartridge developers and endrift for
// helping out/advice.

// These ends are declared in the asm tests below, but GCC isn't smart
// enough to see it to let the linker handle it, even if they were global'd
// in the below assembler.
extern u32 NESPipelineTest_Internal_End(void);
extern u32 TimerPrescalerTest_End(void);

// -----------------------------------
// Assembler Routines
// -----------------------------------

#if MODERN
__attribute__((no_reorder,target("arm")))
#endif
static NAKED u32 NESPipelineTest_Internal(void) {
    asm_unified(".arm\n"
                "mov	r1, lr\n"
                "mov	r0, #0\n"
                "add	lr, pc, #8\n"
                "ldr	r0, [pc, #-16]\n"
                "str	r0, [lr]\n"
                "mov	r0, #0xFF\n"
                "mov	r0, #0xFF\n"
                "bx	r1\n"
                ".pool\n"
                "NESPipelineTest_Internal_End:");
}

#if MODERN
__attribute__((no_reorder,target("arm")))
#endif
static NAKED u32 TimerPrescalerTest(void) {
    asm_unified(".arm\n"
                "stmda sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}\n"
                "@ Load from va_list\n"
                "ldr r1, [r0, #4]\n"
                "ldr r0, [r0]\n"
                "mov r2, #0x400\n"
                "mov r4, r0\n"
                "mov r5, #0\n"
                "str r5, [r4] @ Reset\n"
                "str r1, [r4] @ Start\n"
                "TimerPrescalerLoop:\n"
                "subs r2, r2, #1\n"
                "bne TimerPrescalerLoop\n"
                "mov r0, r0\n"
                "mov r0, r0\n"
                "ldrh r0, [r4]\n"
                "str r5, [r4]\n"
                "ldmib sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}\n"
                "bx lr\n"
                "TimerPrescalerTest_End:\n");
}

// sub_800326C from AGS Aging Cart
#if MODERN
__attribute__((no_reorder))
#endif
static NAKED u32 PrefetchBufferResult_Func(void) {
    asm_unified(".thumb\n"
                "push {r4, r5, r6, r7, lr}\n"
                "ldr r4, =0x04000100\n"
                "movs r5, #0\n"
                "str r5, [r4]\n"
                "ldr r6, =0x00800000\n"
                "str r6, [r4]\n"
                "ldr r2, [r4]\n"
                "ldr r2, [r4]\n"
                "ldr r2, [r4]\n"
                "ldr r2, [r4]\n"
                "ldr r2, [r4]\n"
                "ldr r2, [r4]\n"
                "ldr r2, [r4]\n"
                "ldr r2, [r4]\n"
                "ldrh r0, [r4]\n"
                "str r5, [r4]\n"
                "pop {r4, r5, r6, r7}\n"
                "pop {r1}\n"
                "bx r1\n"
                ".pool\n");
}

// ------------------------------
// C Routines
// ------------------------------

static u16 SetIME(u16 c) {
    u16 backupIME = REG_IME;
    REG_IME = c;
    return backupIME;
}

static s32 DoTest(const char * start, const char * end, u32 expectedValue, ...)
{
    u32 * d;
    const u32 * s;
    u8 buffer[(size_t)(end - start)];
    va_list va_args;
    u32 resp;
    va_start(va_args, expectedValue);

    CpuCopy32(start, buffer, (size_t)(end - start));
    resp = ((u32 (*)(va_list))buffer)(va_args);
    return resp == expectedValue;
}

static s32 PrefetchBufferTest(void)
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

static s32 NESPipelineTest(void)
{
    return DoTest(
        (void *)&NESPipelineTest_Internal,
        (void *)&NESPipelineTest_Internal_End,
        255
    );
}

static s32 TimingTest(void)
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
                (void *)&TimerPrescalerTest,
                (void *)&TimerPrescalerTest_End,
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

s32 PlaceholderTest(void) {
    return 0;
}

const struct TestSpec gTestSpecs[] = {
    [TEST_PREFETCH_BUFFER]            = {TRUE, "Prefetch Buffer", PrefetchBufferTest},
    [TEST_TIMER_PRESCALER]            = {TRUE, "Timer Prescaler", TimingTest},
    [TEST_INSN_PREFETCH]              = {TRUE, "Inst Prefetch",   NESPipelineTest},
};

// Make sure you run this before any sound drivers such as m4a is initialized or you
// will get sound corruption due to manipulating the timers for these tests.
// 
// Pass -1 to run all tests. The result is the bitfield array of which failed.
// Read the Agb header for more information.
u64 RunAgbAccuracyTests(u64 enabledField)
{
    s32 i;
    u64 whichFailed = 0;

    for (i = 0; i < NELEMS(gTestSpecs); i++)
    {
        if (((enabledField & (1ULL << i)) && gTestSpecs[i].enabled) && !gTestSpecs[i].func()) {
            whichFailed |= (1ULL << i);
        }
    }
    return whichFailed;
}
