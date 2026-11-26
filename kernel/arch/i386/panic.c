#include <stdio.h>
#include <stdint.h>
#include <kernel/panic.h>
#include <kernel/tty.h>
#include <kernel/vga.h>
#include "printf_early.h"
// Horay! now i can do architecture specific things.


void kpanic(const char* reason) {
    printf_early("\nKernel Panic: %s\n", reason);
    for(;;);
}
