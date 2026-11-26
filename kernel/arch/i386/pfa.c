// Page frame allocator
unsigned int* bitmapTable;
unsigned int bitmapEntries;
unsigned int bitmap_memBase;
#include <stdbool.h>
#include <stddef.h>
#include <bitfunc.h>

bool frameIsClear(unsigned int frame) {
  uint32_t dword = bitmapTable[frame >> 5];
  return !getBit(dword, frame & 0x1F);
}

void setFrame(unsigned int frame, bool value) {
  uint32_t dword = bitmapTable[frame >> 5];
  bitmapTable[frame >> 5] = setBit(dword, frame & 0x1F, value);
  return;
}

uint32_t* pagealloc() { // malloc() but for pages???????????????????????????????????????
  for (unsigned int i = 0; i < (bitmapEntries >> 5); i++) {
    if (bitmapTable[i] != 0xFFFFFFFF) {
      for (unsigned char bit = 0; bit < 32; bit++) {
        if (frameIsClear((i << 5) + bit)) {
          setFrame((i << 5) + bit, true);
          return (uint32_t*)((((i << 5) + bit) * 4096) + bitmap_memBase);
        }
      }
    }
  }
  return (uint32_t*)0xC0DEC0DE;
}
