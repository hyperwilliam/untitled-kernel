; GCC's Assembler is scary :(
; TODO: make this use multiboot2, but only if multiboot1 is not useful enough.
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
    call setGdt ; woah, it doesnt crash! :D
	extern _init
	call _init
	push ebx
	extern kernel_early
	call kernel_early ; i spent a whole hour trying to figure out why my kernel was not working, then i saw that i was calling kernel_main... oops :)
	cli
.hang:	hlt
	jmp .hang
.end:


gdtr:
.limit:
dw gdt.end - gdt
.addr:
dd gdt

; keep architecture specific stuff OUT of the kernel >:(
setGdt:
   lgdt [gdtr]
   jmp 0x08:.reloadCS
   .reloadCS:
   mov ax, 0x10
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax
   mov ss, ax
   ret
.end:

gdt:
.null:
dq 0
.kcode:
dw 0xFFFF ; limit bits 0-15
dw 0 ; base bits 0-15
db 0 ; base bits 16-23
db 0x9A ; access byte
db 0xCF ; i believe that these are in the correct order, anyways flags 0-3 and limit 16-19
db 0 ; base bits 24-31
.kdata:
dw 0xFFFF ; limit bits 0-15
dw 0 ; base bits 0-15
db 0 ; base bits 16-23
db 0x92 ; access byte (this is the only difference, it probably disables some write option)
db 0xCF ; i believe that these are in the correct order, anyways flags 0-3 and limit 16-19
db 0 ; base bits 24-31
; TODO: add the user mode stuff once we get there (if ever!)
.end:
