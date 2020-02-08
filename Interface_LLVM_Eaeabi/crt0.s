/************************************************************************/
/*      GCC/LLCM ARM Baremetal Software / (C) Toshio MURAI  2012-2016   */
/*      This is Free Software.   You can redistribute this software     */
/*      and/or modify it ,under the terms of the GNU General Public     */
/*      License version 3, or (at your option) any later version.       */
/*      or under the terms of the BSD 3-Clause License.	(Dual Licence)  */
/************************************************************************/
	.text
	.code 32
##########################################################

.equ	CPSR_MODE_USER,	0x10
.equ	CPSR_MODE_FIQ,	0x11
.equ	CPSR_MODE_IRQ,	0x12
.equ	CPSR_MODE_SVR,	0x13
.equ	CPSR_MODE_ABORT,	0x17
.equ	CPSR_MODE_UNDEFINED,	0x1B
.equ	CPSR_MODE_SYSTEM,	0x1F

.equ	CPSR_IRQ_INHIBIT,	0x80
.equ	CPSR_FIQ_INHIBIT,	0x40
.equ	CPSR_THUMB,	0x20

##########################################################
#	Power	On Reset Start up Routine
##########################################################
	.globl	start
start:

#
.ifdef	HYPER
	mrs r0,cpsr
	bic r0,r0,#0x1F
	orr r0,r0,#0x13
	msr spsr_cxsf,r0

	ldr r0,=.L000
	msr ELR_hyp,r0
	eret
.L000:
.endif
#

	/*#(CPSR_MD_IRQ=0x12|CPSR_IRQ=0x80|CPSR_FIQ=0x40) ~THUMB 0x20	*/
	MOV	R0, #(CPSR_MODE_IRQ | CPSR_IRQ_INHIBIT | CPSR_FIQ_INHIBIT )
						/* MOV	R1,	#0xD2	*/
	MSR	CPSR_c, r0				/* cpsr=cxsf*/
	LDR	SP,	= _interruptStack	/* R13 Interrupt stack */
						/* mov sp, #0x4000 */

	/*#(CPSR_MD_IRQ=0x12|CPSR_IRQ=0x80|CPSR_FIQ=0x40) ~THUMB 0x20	*/
	MOV	R0, #(CPSR_MODE_SVR | CPSR_IRQ_INHIBIT | CPSR_FIQ_INHIBIT )
						/* MOV	R1,	#0xD3	*/
	MSR	CPSR_c, r0				/* cpsr=cxsf*/
	LDR	SP,	= _supervisorStack		/* R13 System stack */
						/* mov	sp, #0x8000 */
	BL	initializeMemory
						#.ifdef	RaspberryPI
.ifdef	JTAG
	BL	initializeJtagPin		/* Only for Raspberry PI */
.endif
						#.endif
	BL	main
exit:
	B	exit


#
#	Interrupt	disable
#
	.global	disableInterrupt
disableInterrupt:	/*	 IRQ Disable	FIQ 0x40*/
	MRS	R0,	CPSR
	ORR	R0,	R0	,#0x080	/*#CPSR_IRQ*/
	MSR	CPSR_cxsf,	R0
	MOV	PC,	LR		 /*	return */
#
#	Interrupt	enable
#
	.global	enableInterrupt
enableInterrupt:	/*	 IEQ Enable FIQ ~0x40*/

	MRS	R0,	CPSR
	AND	R0,	R0	,#~0x80	/*#~CPSR_IRQ*/
	MSR	CPSR_cxsf,	r0
	MOV	PC,	LR		 /*	return */

##_enable_interrupts:
##	MRS	R0, CPSR
##	BIC	R0, R0, #0x80
##	MSR	CPSR_C, r0
##	MOV	PC, LR

##########################################################
	.global	readVectorBaseRegister
readVectorBaseRegister:
	MRC	P15,	0, R0, C12, C0, 0 
	MOV	PC,	LR		 /*	return */

##########################################################
	.global	writeVectorBaseRegister
writeVectorBaseRegister:
#	MRC	p15,	0, r0, c12, c0, 0 
#	LDR	r0,#0x20100000
	MCR	P15,	0, R0, C12, C0, 0 
	MOV	PC,	LR		 /*	return */

##########################################################
	.global	setHighVector
setHighVector:
	MRC	P15,	0,R0, C1, C0, 0
	MOV	R1,#0x00002000
	ORR	R0,R1
	MCR	P15,	0,R0, C1, C0, 0
	MOV	PC,	LR		 /*	return */
	
##########################################################
	.global	setLowVector
setLowVector:
	MRC	P15,	0,R0, C1, C0, 0
	MOV	R1,#0xFFFFDFFF
	AND	R0,R1
	MCR	P15,	0,R0, C1, C0, 0
	MOV	PC,	LR		 /*	return */

##########################################################
	.global	readSCTLR
readSCTLR:
	MRC	P15,	0,R0, C1, C0, 0
	MOV	PC,	LR		 /*	return */

##########################################################
	.global disableInstructionCache
disableInstructionCache:
	PUSH	{lr}
	MRC	p15, #0, r0, c1, c0, #0
	BIC	r0,	r0, #0x00001000
	MCR	p15, #0, r0, c1, c0, #0
@	BL	CP15ICacheFlush
	POP	{lr}
	BX	lr

##########################################################
	.global disableDataCache
disableDataCache:
	PUSH	{r4-r11, lr}
	MRC	p15, #0, r0, c1, c0, #0
	BIC	r0,	r0, #0x00000004
	MCR	p15, #0, r0, c1, c0, #0
@	BL	CP15DCacheCleanFlush
	POP	{r4-r11, lr}
	BX	lr

##########################################################
	.global disableMMU
disableMMU:
	MCR	p15, #0, r0, c8, c7, #0	@ Invalidate TLB	
	MRC	p15, #0, r0, c1, c0, #0	
	BIC	r0, r0, #1	
	MCR	p15, #0, r0, c1, c0, #0	@ Clear MMU bit
	BX	lr

##########################################################
	.global	undefInst
undefInst:
	.long	0xE7FFDEFE
	.long	0xE7FFDEFE
	BX	lr

