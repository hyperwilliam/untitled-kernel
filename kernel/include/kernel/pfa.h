#ifndef KERNEL_PFA_H
#define KERNEL_PFA_H

#include <stdbool.h>
#include <stddef.h>

extern unsigned int* bitmapTable;
extern unsigned int bitmapEntries;
extern unsigned int bitmap_memBase;
bool frameIsClear(unsigned int frame);
void setFrame(unsigned int frame, unsigned char value);
uint32_t* pagealloc();

#endif
