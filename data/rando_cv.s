	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

    @ This value is written to by the randomizer.
    .align 4
gRandomizerCheckValue::
    .4byte 0x00000000
