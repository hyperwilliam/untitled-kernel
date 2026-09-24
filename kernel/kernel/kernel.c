#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "multiboot.h"
#include <kernel/tty.h>

// TODO: make panic_early() and panic() display registers and other thingys :)
const char* kernel_version_high = "1"; // i should increment this one if the update might break some programs :)
const char* kernel_version_mid = "0"; // i should increment this one if i add new features, but they wont break anything :)
const char* kernel_version_low = "0";// i should increment this one for bugfixes.
const char* kernel_version_prefix = "-Prototype"; // do i really have to explain this one tho :)
unsigned int kernel_reserve = 0x1000000; // reserve 16 MB for kernel :)
unsigned int targetRamBase = 0; // base of the largest RAM region.
unsigned int targetRamSize = 0; // size of the largest RAM region.
struct mem_reserve_block {
  unsigned int address;
  unsigned int size;
};

struct tar_header { // you know, i could probably make a better version of this file format, well i suppose if it works it works.
  char filename[100];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char chksum[8];
  char typeflag[1];
};
#define PSF_FONT_MAGIC 0x864ab572

typedef struct {
  uint32_t magic;         /* magic bytes to identify PSF */
  uint32_t version;       /* zero */
  uint32_t headersize;    /* offset of bitmaps in file, 32 */
  uint32_t flags;         /* 0 if there's no unicode table */
  uint32_t numglyph;      /* number of glyphs */
  uint32_t bytesperglyph; /* size of each glyph */
  uint32_t height;        /* height in pixels */
  uint32_t width;         /* width in pixels */
} PSF_font;

struct tar_header *RamdiskHeaders[32];

void kpanic(const char* reason) {
  printf("Kernel Panic: %s\n", reason);
  for(;;);
}

unsigned int gettarsize(const char *in)
{

  unsigned int size = 0;
  unsigned int j;
  unsigned int count = 1;

  for (j = 11; j > 0; j--, count *= 8)
    size += ((in[j - 1] - '0') * count);

  return size;

}

unsigned int parse(unsigned int address) {

  unsigned int i;

  for (i = 0; ; i++) {

    struct tar_header *header = (struct tar_header *)address;

    if (header->filename[i] == '\0')
      break;

    unsigned int size = gettarsize(header->size);

    RamdiskHeaders[i] = header;

    address += ((size / 512) + 1) * 512;

    if (size % 512)
      address += 512;

  }

  return i;

}

void kernel_early(multiboot_info_t* mbd, uint32_t magic) {
    // alright, now to read the MODULES (until i find the psf file.)
    // to read kernel panics or messages here, you will need to comment out the VIDEOINFO field in the MBFLAGS at boot.asm!
	terminal_initialize();
    if (mbd->mods_count == 0) {
      kpanic("You need to load a init.rd in the modules.");
    }
    uint32_t* moduleptr = (uint32_t*) mbd->mods_addr;
    parse(*moduleptr);
    printf("Looking for font.psf in init.rd!\n");
    bool foundFont = false;
    unsigned char fileindex = 0;
    const char* fontfile = "./font.psf";
    while(!foundFont) {
      foundFont = memcmp(RamdiskHeaders[fileindex]->filename, fontfile, strlen(fontfile)) == 0;
      if (foundFont) {
        printf("Found %s!\n", fontfile);
        continue;
      } else {
        fileindex++;
      }
    }
    printf("Trying to read the file..!\n");
    PSF_font *fontheader = (PSF_font *)RamdiskHeaders[fileindex];
    fontheader += 16; // i believe this will work.
    if (fontheader->magic != PSF_FONT_MAGIC) {
      kpanic("'%s' Is Not a valid PSF2 File!");
    }
    if (fontheader->version != 0) {
      kpanic("'%s' Is Not a valid PSF2 Version 0 File!");
    }
    printf("PSF2 Header Size: %X\n",fontheader->headersize); // TODO: make this output decimal instead of hexadecimal.
    printf("PSF2 Font Width: %X\n",fontheader->width); // TODO: make this output decimal instead of hexadecimal.
    printf("PSF2 Font Height: %X\n",fontheader->height); // TODO: make this output decimal instead of hexadecimal.
    //printf("PSF2 Header Size: %X\n",fontheader->headersize);
    uint32_t* fontData =  (uint32_t*)fontheader + 8;
    printf("FontData = 0x%X\n", fontData);
    terminal_highres(mbd->framebuffer_width, mbd->framebuffer_height, fontheader->width, fontheader->height, mbd->framebuffer_addr, (unsigned int)fontData, mbd->framebuffer_type, mbd->framebuffer_pitch); // input 0 = width, input 1 = height, input 2 = font width, input 3 = font height, input 4 = pointer to video mem, input 5 = pointer to PSF file (stored and found in init.rd) input 6 = video mode type, input 7 = bytes per line
    if (mbd->framebuffer_type == 2) {
      printf("Now Outputting To Display!\n");
    } else {
      printf("Font File Read! Now Outputting to display!\n");
    }
	printf("UntitledKernel %s.%s.%s%s!\n", kernel_version_high, kernel_version_mid, kernel_version_low, kernel_version_prefix);
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
      kpanic("Multiboot magic is not valid!");
    }
    if(!(mbd->flags >> 6 & 0x1)) {
      kpanic("Invalid Memory Map From Multiboot!");
    }
    printf("---- MEMORY MAP ----\n\n");
    unsigned int i;
    for(i = 0; i < mbd->mmap_length;
        i += sizeof(multiboot_memory_map_t))
        {
          multiboot_memory_map_t* mmmt =
          (multiboot_memory_map_t*) (mbd->mmap_addr + i);

          //printf("Start Addr: 0x%X Length: 0x%X Size: 0x%X Type: 0x%X\n",
          //       mmmt->addr_low, mmmt->len_low, mmmt->size, mmmt->type);
          if (mmmt->addr_high == 0) {
            if(mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) {
              printf("RAM Available at 0x%X to 0x%X!\n", mmmt->addr_low, mmmt->len_low + mmmt->addr_low - 1);
              if (mmmt->len_low > targetRamSize) {
                targetRamSize = mmmt->len_low;
                targetRamBase = mmmt->addr_low;
              }
            }
            if(mmmt->type == MULTIBOOT_MEMORY_RESERVED) {
              printf("Reserved Memory at 0x%X to 0x%X!\n", mmmt->addr_low, mmmt->len_low + mmmt->addr_low - 1);
            }
            if(mmmt->type == MULTIBOOT_MEMORY_ACPI_RECLAIMABLE) {
              printf("APCI Reclaimable at 0x%X to 0x%X!\n", mmmt->addr_low, mmmt->len_low + mmmt->addr_low - 1);
            }
            if(mmmt->type == MULTIBOOT_MEMORY_NVS) {
              printf("APCI NVS RAM at 0x%X to 0x%X..?\n", mmmt->addr_low, mmmt->len_low + mmmt->addr_low - 1);
            }
            if(mmmt->type == MULTIBOOT_MEMORY_BADRAM) {
              printf("Faulty RAM at 0x%X to 0x%X!, Get that checked!\n", mmmt->addr_low, mmmt->len_low + mmmt->addr_low - 1);
            }
          }
        }
    printf("\n--------------------\n\n");
    if (targetRamBase == 0x100000) { // this is where our kernel is loaded, so we will increase the base value and decrement the size to avoid overwriting it.
      targetRamBase += kernel_reserve;
      targetRamSize -= kernel_reserve;
      printf("Corrected RAM Offset To Avoid Overwriting Kernel!\n");
    }
	//abort();
    kpanic("Nothing Left To Do...\n");
}

void kernel_main() {
  kpanic("what now :(");
}
