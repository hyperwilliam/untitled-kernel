#include <stdio.h>

#include <kernel/tty.h>

// TODO: make panic_early() and panic() display registers and other thingys :)

void panic_early(const char* reason) {
  printf("Early Kernel Panic: %s\n", reason);
	asm volatile ("hlt");
  for(;;);
}

void panic(const char* reason) {
  printf("Kernel Panic: %s\n", reason);
	asm volatile ("hlt");
  for(;;);
}

void kernel_early(void) {
	terminal_initialize();
	printf("UntitledKernel 1.0.0!\n");
	panic_early("Testing The Panic() Function!");
}

void kernel_main() {
  panic("what now :(");
}
