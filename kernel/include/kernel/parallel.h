#ifndef _KERNEL_PARALLEL_H
#define _KERNEL_PARALLEL_H

#include <stddef.h>
#include <stdint.h>
void parallel_initialise(void);
int parallel_putchar(char c);
void parallel_write(const char* data, size_t size);
void parallel_writestring(const char* data);
#endif
