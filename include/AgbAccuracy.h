#ifndef _AGB_ACCURACY_H
#define _AGB_ACCURACY_H

/**
 * AGB Accuracy Tests Documentation
 * 
 * This file is designed to where when RunAgbAccuracyTests it will run the tests defined
 * to be enabled in the value passed to the function as a bitfield.
 *
 * The following is a list of all tests, in order, defined as (1 << n) in the bitfield:
 *
 * Memory Tests:
 * CPU EXTERNAL WORK RAM
 * CPU INTERAL WORK RAM
 * PALETTE RAM
 * VRAM
 * OAM
 * CARTRIDGE TYPE FLAG
 * PREFETCH BUFFER
 * WAIT STATE WAIT CONTROL
 * CARTRIDGE RAM WAIT CONTROL
 * 
 * Lcd Tests:
 * V COUNTER
 * V COUNT INTR FLAG
 * H BLANK INTR FLAG
 * V BLANK INTR FLAG
 * V COUNT STATUS
 * H BLANK STATUS
 * V BLANK STATUS
 *
 * Timer Tests:
 * TIMER PRESCALER
 * TIMER CONNECT
 * TIMER INTR FLAG
 *
 * DMA Tests:
 * DMA0 ADDRESS CONTROL
 * DMA1 ADDRESS CONTROL
 * DMA2 ADDRESS CONTROL
 * DMA3 ADDRESS CONTROL
 * DMA V BLANK START
 * DMA H BLANK START
 * DMA DISPLAY START
 * DMA INTR FLAG
 * DMA PRIORITY
 *
 * COM Tests:
 * MULTI PLAY SIO
 *
 * Key Tests:
 * KEY INPUT
 * KEY INPUT SIMPLE
 *
 * Interrupt Tests:
 * V BLANK INTR
 * H BLANK INTR
 * V COUNT INTR
 * TIMER INTR
 * SIO INTR
 * DMA INTR
 * KEY INTR
 *
 * NOTE: Not every test has been added to the library.
 */
 
// ----------------------------------------
// Test List Configuration
// ----------------------------------------

// Name will point to an ASCII name for the test. Please be sure to, if needed, convert it to
// a game specific charmap if printing is desired.
struct TestSpec {
    bool8 enabled;
    const u8 * name;
    s32 (*const func)(void);
};

enum TestList {
    /* Memory */
    TEST_CPU_EXTERNAL_WORK_RAM,
    TEST_CPU_INTERNAL_WORK_RAM,
    TEST_PALETTE_RAM,
    TEST_VRAM,
    TEST_OAM,
    TEST_CARTRIDGE_TYPE_FLAG,
    TEST_PREFETCH_BUFFER,
    TEST_WAIT_STATE_WAIT_CONTROL,
    TEST_CARTRIDGE_RAM_WAIT_CONTROL,
    /* Lcd */
    TEST_V_COUNTER,
    TEST_V_COUNT_INTR_FLAG,
    TEST_H_COUNT_INTR_FLAG,
    TEST_V_BLANK_INTR_FLAG,
    TEST_V_COUNT_STATUS,
    TEST_H_BLANK_STATUS,
    TEST_V_BLANK_STATUS,
    /* Timer */
    TEST_TIMER_PRESCALER,
    TEST_TIMER_CONNECT,
    TEST_TIMER_INTR_FLAG,
    /* DMA */
    TEST_DMA0_ADDRESS_CONTROL,
    TEST_DMA1_ADDRESS_CONTROL,
    TEST_DMA2_ADDRESS_CONTROL,
    TEST_DMA3_ADDRESS_CONTROL,
    TEST_DMA_V_BLANK_START,
    TEST_DMA_H_BLANK_START,
    TEST_DMA_DISPLAY_START,
    TEST_DMA_INTR_FLAG,
    TEST_DMA_PRIORITY,
    /* COM */
    TEST_MULTI_PLAY_SIO,
    /* Key */
    TEST_KEY_INPUT,
    TEST_KEY_INPUT_SIMPLE,
    /* Interrupt */
    TEST_V_BLANK_INTR,
    TEST_H_BLANK_INTR,
    TEST_V_COUNT_INTR,
    TEST_TIMER_INTR,
    TEST_SIO_INTR,
    TEST_DMA_INTR,
    TEST_KEY_INTR,
    /* Extra: NES Prefetch Test */
    TEST_INSN_PREFETCH
};

extern const struct TestSpec gTestSpecs[];

u64 RunAgbAccuracyTests(u64);

#endif // _AGB_ACCURACY_H
