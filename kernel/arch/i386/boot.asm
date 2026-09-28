; GCC's Assembler is scary :(
; TODO: make this use multiboot2, but only if multiboot1 is not useful enough.
MBALIGN  equ  1 << 0
MEMINFO  equ  1 << 1
VIDEOINFO equ 1 << 2
MBFLAGS  equ  MBALIGN | MEMINFO | VIDEOINFO
MAGIC    equ  0x1BADB002
CHECKSUM equ -(MAGIC + MBFLAGS)

VMODETYPE equ 0
VMODEWIDTH equ 640
VMODEHEIGHT equ 480
VMODEDEPTH equ 8
section .multiboot
align 4
	dd MAGIC
	dd MBFLAGS
	dd CHECKSUM
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd VMODETYPE
	dd VMODEWIDTH
	dd VMODEHEIGHT
	dd VMODEDEPTH

section .bss
align 16
stack_bottom:
resb 16384 ; TODO: add the 'stack smashing' protection, (how do i do that tho)
stack_top:


section .text
global _start:function (_start.end - _start)
_start:
	mov esp, stack_top
	push eax
	push ebx
    call setGdt ; woah, it doesnt crash! :D
	extern _init
	call _init
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
db 0x92 ; access byte (this is the only difference, it probably disables some execute option)
db 0xCF ; i believe that these are in the correct order, anyways flags 0-3 and limit 16-19
db 0 ; base bits 24-31
; TODO: add the user mode stuff once we get there (if ever!)
.end:

global _interrupt_wrapper:function (_interrupt_wrapper.end - _interrupt_wrapper)
extern code_exception_handler
interrupt_wrapper:
    pushad
    cld    ; C code following the sysV ABI requires DF to be clear on function entry
    add esp, 32
    call code_exception_handler
    sub esp, 32
    popad
    iret
.end:


global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31

%macro ISR_NOERRCODE 1
isr%1:
    push dword 0
    push dword %1
    jmp interrupt_wrapper
%endmacro

%macro ISR_ERRCODE 1
isr%1:
    push dword %1
    jmp interrupt_wrapper
%endmacro

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7

ISR_ERRCODE 8

ISR_NOERRCODE 9

ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14

ISR_NOERRCODE 15
ISR_NOERRCODE 16

ISR_ERRCODE 17

ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29

ISR_ERRCODE 30

ISR_NOERRCODE 31
