#include <stdio.h>

#include <kernel/tty.h>

// TODO: make panic_early() and panic() display registers and other thingys :)
const char* kernel_version_high = "1"; // i should increment this one if the update might break some programs :)
const char* kernel_version_mid = "0"; // i should increment this one if i add new features, but they wont break anything :)
const char* kernel_version_low = "0";// i should increment this one for bugfixes.
const char* kernel_version_prefix = "-Prototype"; // do i really have to explain this one tho :)

struct mem_reserve_block {
  unsigned int address;
  unsigned int size;
}
void panic_early(const char* reason) {
  printf("Early Kernel Panic: %s\n", reason);
  for(;;);
}

void panic(const char* reason) {
  printf("Kernel Panic: %s\n", reason);
  for(;;);
}

void kernel_early(unsigned int* multiboot) {
	terminal_initialize();
	printf("UntitledKernel %s.%s.%s%s!\n", kernel_version_high, kernel_version_mid, kernel_version_low, kernel_version_prefix);
	panic_early("Testing The Panic() Function!");
}

void kernel_main() {
  panic("what now :(");
}
