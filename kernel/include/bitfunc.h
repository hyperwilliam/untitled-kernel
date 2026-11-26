#ifndef BITFUNC_H
#define BITFUNC_H
#include <stdbool.h>
#include <stdint.h>
// might be expanded upon, eventually!
bool getBit(uint32_t value, uint8_t index);
uint32_t setBit(uint32_t value, uint8_t index, bool state);

#endif
