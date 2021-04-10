	.include "asm/macros.inc"
	.include "constants/constants.inc"
	.text
	.syntax unified
	.align 2, 0
	arm_func_start NESPipelineTest_Internal
NESPipelineTest_Internal:
	mov	r1, lr
	mov	r0, #0
	add	lr, pc, #8
	ldr	r0, [pc, #-16]
	str	r0, [lr]
	mov	r0, #0xFF
	mov	r0, #0xFF
	bx	r1
	.pool
	arm_func_end NESPipelineTest_Internal
	.global NESPipelineTest_Internal_End
NESPipelineTest_Internal_End:
	arm_func_start TimerPrescalerTest
TimerPrescalerTest:
	stmda sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
	@ Load from va_list
	ldr r1, [r0, #4]
	ldr r0, [r0]

	mov r2, #0x400
	mov r4, r0
	mov r5, #0
	str r5, [r4] @ Reset
	str r1, [r4] @ Start
_08009274:
	subs r2, r2, #1
	bne _08009274
	mov r0, r0
	mov r0, r0
	ldrh r0, [r4]
	str r5, [r4]
	ldmib sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bx lr
	arm_func_end TimerPrescalerTest
	.global TimerPrescalerTest_End
TimerPrescalerTest_End:
