#define PARALLEL_OUTPUT true // if false, nothing will be outputted to the parallel port.
#define PARALLEL_PORT 0x378 // TODO: grab this from the bios data area
#include <string.h>
#include <stdint.h>
#include "io.c"
void parallel_initialise() {}
#ifdef PARALLEL_OUTPUT
int parallel_putchar(unsigned char value) {
	if (value == 0x0A) {
		// i hear there is an auto line feed, i didnt know how to get it working.
		while((!inb(PARALLEL_PORT + 1)) & 0x80) {
			outb(0x80, 0);
		}
		outb(PARALLEL_PORT, 0xD);
		unsigned char ctrlReg = inb(PARALLEL_PORT + 2);
		outb(PARALLEL_PORT + 2, ctrlReg | 1);
		outb(0x80, 0);
		outb(PARALLEL_PORT + 2, ctrlReg);
		while((!inb(PARALLEL_PORT + 1)) & 0x80) {
			outb(0x80, 0);
		}
	}
	while((!inb(PARALLEL_PORT + 1)) & 0x80) {
		outb(0x80, 0);
	}
	outb(PARALLEL_PORT, value);
	unsigned char ctrlReg = inb(PARALLEL_PORT + 2);
	outb(PARALLEL_PORT + 2, ctrlReg | 1);
	outb(0x80, 0);
	outb(PARALLEL_PORT + 2, ctrlReg);
	while((!inb(PARALLEL_PORT + 1)) & 0x80) {
		outb(0x80, 0);
	}
	return value;
}

void parallel_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++) {
		parallel_putchar(data[i]);
	}
}

void parallel_writestring(const char* data) {
	parallel_write(data, strlen(data));
}
#endif
#ifndef PARALLEL_OUTPUT
void parallel_putchar(unsigned char value) {}
void parallel_write(const char* data, size_t size) {}
void parallel_writestring(const char* data) {}
#endif
