#ifndef ARCH_I386_INTERRUPTS_H
#define ARCH_I386_INTERRUPTS_H
#include <stdint.h>
void idt_set_entry(uint8_t interrupt, uint32_t handler_address, uint16_t selector, uint8_t flags);
void interrupts_initialise(void);
#endif
