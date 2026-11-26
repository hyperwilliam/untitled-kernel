// Should initialise the enviroment, stuff like memory management, video output, and more.


// first, init() is called by boot.asm
// now we have to initialise the video output
// this will also be done by reading the video arguments
// i should also use CPU specific features if present and useful.

#include "multiboot.h"
#include "printf_early.h"
#include "interrupts.h"
#include <kernel/parallel.h>
#include <kernel/version.h>
#include <kernel/pfa.h>
void panic(const char* reason) {
    printf_early("Early panic: %s\n", reason);
}
extern unsigned int topOfKernel; // decided by linker
extern const char* compiletime; // decided by assembler
extern const char* compiledate; // decided by assembler
unsigned int targetRamBase = 0; // base of the largest RAM region.
unsigned int targetRamSize = 0; // size of the largest RAM region.

void kernel_init(multiboot_info_t* mbd, uint32_t magic) {
    parallel_initialise();
    printf_early("Started.\n");
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        panic("Multiboot magic is not valid!");
    }
    if(!(mbd->flags >> 6 & 0x1)) {
        panic("Invalid Memory Map From Multiboot!");
    }
    printf_early("UntitledKernel-r%u", KERN_VER_HIGH);
    if (KERN_VER_LOW != 0) {
        printf_early(".%u-i386!\n", KERN_VER_LOW);
    } else {
        printf_early("-i386!\n");
    }
    printf_early("Initial Assembly built on %s at %s!\n", &compiledate, &compiletime);
    printf_early("---- MEMORY MAP ----\n\n");
    unsigned int i;
    for(i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t))  {
        multiboot_memory_map_t* mmmt = (multiboot_memory_map_t*) (mbd->mmap_addr + i);
        if (mmmt->addr_high == 0) {
            if(mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) {
                printf_early("RAM Available at 0x%X to 0x%X!\n", mmmt->addr_low, mmmt->len_low + mmmt->addr_low - 1);
                if (mmmt->len_low > targetRamSize) {
                    targetRamBase = mmmt->addr_low;
                    targetRamSize = mmmt->len_low;
                }
            }
            if(mmmt->type == MULTIBOOT_MEMORY_RESERVED) {
                printf_early("Reserved Memory at 0x%X to 0x%X!\n", mmmt->addr_low, mmmt->len_low + mmmt->addr_low - 1);
            }
            if(mmmt->type == MULTIBOOT_MEMORY_ACPI_RECLAIMABLE) {
                printf_early("APCI Reclaimable at 0x%X to 0x%X!\n", mmmt->addr_low, mmmt->len_low + mmmt->addr_low - 1);
            }
            if(mmmt->type == MULTIBOOT_MEMORY_NVS) {
                printf_early("APCI NVS RAM at 0x%X to 0x%X..?\n", mmmt->addr_low, mmmt->len_low + mmmt->addr_low - 1);
            }
            if(mmmt->type == MULTIBOOT_MEMORY_BADRAM) {
                printf_early("Faulty RAM at 0x%X to 0x%X!, Get that checked!\n", mmmt->addr_low, mmmt->len_low + mmmt->addr_low - 1);
            }
        }
    }
    printf_early("\n--------------------\n\n");
    printf_early("Kernel will operate with %u bytes of memory!\n", targetRamSize);
    int realPagesCount = targetRamSize / 4096;
    printf_early("...Meaning we have %u pages!\n", realPagesCount);
    int pagesCount = realPagesCount - (realPagesCount / 4096) - 1;
    bitmapEntries = realPagesCount;
    printf_early("...%u pages if i count in the bitmap.\n", pagesCount);
    unsigned int reservedPages = 1;
    printf_early("So bitmap would take %u bytes.\n", realPagesCount / 8);
    if (targetRamBase == 0x100000) { // this is where our kernel is loaded, so we will have to account for it.
        reservedPages += (((unsigned int)&topOfKernel) - 0x100000) / 4096;
        printf_early("Also reserving %u pages for the kernel!\n", reservedPages - 1);
    }
    bitmap_memBase = targetRamBase;
    bitmapTable = &topOfKernel;
    for (int i = 0; i < realPagesCount / 4; i++) {
        bitmapTable[i] = 0;
    }
    for (unsigned int i = 0; i < reservedPages; i++) {
        pagealloc();
    }
    printf_early("Reserved %u Pages!\n", reservedPages);
    interrupts_initialise();
    printf_early("Initialised Interrupt Descriptor Table!\n");
    // anyways, setting a breakpoint interrupt prints the debug message, but then crashes
    // i'll figure out why that happens at a later time.
    panic("Nothing Left to do!");
}

