#include <stdio.h>
#include <stdint.h>
#include "multiboot.h"
#include <kernel/tty.h>

// TODO: make panic_early() and panic() display registers and other thingys :)
const char* kernel_version_high = "1"; // i should increment this one if the update might break some programs :)
const char* kernel_version_mid = "0"; // i should increment this one if i add new features, but they wont break anything :)
const char* kernel_version_low = "0";// i should increment this one for bugfixes.
const char* kernel_version_prefix = "-Prototype"; // do i really have to explain this one tho :)

struct mem_reserve_block {
  unsigned int address;
  unsigned int size;
};

void panic_early(const char* reason) {
  printf("Early Kernel Panic: %s\n", reason);
  for(;;);
}

void panic(const char* reason) {
  printf("Kernel Panic: %s\n", reason);
  for(;;);
}

void kernel_early(multiboot_info_t* mbd, uint32_t magic) {
	terminal_initialize();
	printf("UntitledKernel %s.%s.%s%s!\n", kernel_version_high, kernel_version_mid, kernel_version_low, kernel_version_prefix);
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
      panic_early("Multiboot magic is not valid!");
    }

    if(!(mbd->flags >> 6 & 0x1)) {
      panic_early("Invalid Memory Map From Multiboot!");
    }


    printf("---- MEMORY MAP ----\n\n");
    int i;
    for(i = 0; i < mbd->mmap_length;
        i += sizeof(multiboot_memory_map_t))
        {
          multiboot_memory_map_t* mmmt =
          (multiboot_memory_map_t*) (mbd->mmap_addr + i);

          //printf("Start Addr: 0x%X Length: 0x%X Size: 0x%X Type: 0x%X\n",
          //       mmmt->addr_low, mmmt->len_low, mmmt->size, mmmt->type);
          if (mmmt->addr_high == 0) {
            if(mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) {
              printf("RAM Available at 0x%X to 0x%X!\n", mmmt->addr_low, mmmt->len_low + mmmt->addr_low);
            }
            if(mmmt->type == MULTIBOOT_MEMORY_RESERVED) {
              printf("Reserved Memory at 0x%X to 0x%X!\n", mmmt->addr_low, mmmt->len_low + mmmt->addr_low);
            }
            if(mmmt->type == MULTIBOOT_MEMORY_ACPI_RECLAIMABLE) {
              printf("APCI Reclaimable at 0x%X to 0x%X!\n", mmmt->addr_low, mmmt->len_low + mmmt->addr_low);
            }
            if(mmmt->type == MULTIBOOT_MEMORY_NVS) {
              printf("NVS at 0x%X to 0x%X..?\n", mmmt->addr_low);
            }
            if(mmmt->type == MULTIBOOT_MEMORY_BADRAM) {
              printf("Faulty RAM at 0x%X to 0x%X!, Get that checked!\n", mmmt->addr_low, mmmt->len_low + mmmt->addr_low);
            }
          }
        }
    printf("\n--------------------\n");
	panic_early("Nothing Left To Do...");
}

void kernel_main() {
  panic("what now :(");
}
