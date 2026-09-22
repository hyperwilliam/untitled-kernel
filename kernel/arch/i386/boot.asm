; GCC's Assembler is scary :(
; TODO: make this use multiboot2, and if there isnt a solution for 386 and similar, make my own!
MBALIGN  equ  1 << 0
MEMINFO  equ  1 << 1
MBFLAGS  equ  MBALIGN | MEMINFO
MAGIC    equ  0x1BADB002
CHECKSUM equ -(MAGIC + MBFLAGS)

section .multiboot
align 4
	dd MAGIC
	dd MBFLAGS
	dd CHECKSUM

section .bss
align 16
stack_bottom:
resb 16384 ; TODO: add the 'stack smashing' protection, (how do i do that tho)
stack_top:


section .text
global _start:function (_start.end - _start)
_start:

	mov esp, stack_top
	extern _init
	call _init
	extern kernel_main
	call kernel_main
	cli
.hang:	hlt
	jmp .hang
.end:
