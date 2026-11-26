#include <stdbool.h>
#include <stdint.h>
bool getBit(uint32_t value, uint8_t index) {
  return (bool)((value >> index) & 1);
};

uint32_t setBit(uint32_t value, uint8_t index, bool state) {
  uint32_t mask = (1 << index);
  return (value & ~mask) | ((unsigned char)state << index);
};
