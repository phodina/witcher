.set IRQ_BASE, 0x20
	
.section .text

.extern _ZN4myos21hardwarecommunication16InterruptManager15handleInterruptEhj

.global _ZN4myos21hardwarecommunication16InterruptManager22IgnoreInterruptRequestEv

.macro HandleException num
.global _ZN4myos21hardwarecommunication16InterruptManager16HandleException\num\()Ev
_ZN4myos21hardwarecommunication16InterruptManager16handleException\num\()Ev:
	movb $\num, (interruptnumber)
	jmp int_bottom
.endm

.macro HandleInterruptRequest num
.global _ZN4myos21hardwarecommunication16InterruptManager26HandleInterruptRequest\num\()Ev
_ZN4myos21hardwarecommunication16InterruptManager26HandleInterruptRequest\num\()Ev:
	movb $\num + IRQ_BASE, (interruptnumber)
	jmp int_bottom
.endm
	
HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x0C
	
int_bottom:	

	# Store previous content
	pusha
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs
	
	pushl %esp
	push (interruptnumber)
	call _ZN4myos21hardwarecommunication16InterruptManager15handleInterruptEhj
	addl  %esp, 6
	movl %eax, %esp

	# Resotre previous content
	popl %gs
	popl %fs
	popl %es
	popl %ds
	popa

	# Return from interrupt
_ZN4myos21hardwarecommunication16InterruptManager22IgnoreInterruptRequestEv:	
	iret
	
.data
interruptnumber:	 .byte 0
