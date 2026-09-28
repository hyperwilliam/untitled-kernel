#include <stdio.h>
#include <stdint.h>
#include <kernel/panic.h>
#include <kernel/tty.h>
#include <kernel/vga.h>

// Horay! now i can do architecture specific things.


void kpanic(const char* reason) {
    terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_LIGHT_RED));
    printf("\nKernel Panic: %s\n", reason);
    for(;;);
}
