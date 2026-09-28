#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>
#include <stdint.h>
void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_highres(uint16_t width, uint16_t height, uint8_t fontwidth, uint8_t fontheight, uint32_t videoaddr, uint32_t psfAddr, uint8_t vtype, uint32_t video_pitch);
void terminal_setcolor(uint8_t color);
#endif
