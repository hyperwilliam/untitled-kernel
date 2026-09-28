#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>

#include <kernel/vga.h>
#include <kernel/colors.h>
#include "io.c"
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

static bool highresMode = false;
static uint8_t graphicType = 2;
static size_t VID_WIDTH = 80; // this is for the video mode, using VGA as a fallback.
static size_t VID_HEIGHT = 25;
static size_t VID_FHEIGHT = 16;
static size_t VID_FWIDTH = 8;
static uint32_t* VID_MEMORY = (uint32_t*) 0xB8000;
static size_t VID_LINE_OFFSET = 160;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;
static uint8_t* pixel_buffer;
static uint8_t* psf_data;
void terminal_initialize() {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_highres(uint16_t width, uint16_t height, uint8_t fontwidth, uint8_t fontheight, uint32_t videoaddr, uint32_t psfAddr, uint8_t video_type, uint32_t video_pitch) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    VID_MEMORY = (uint32_t*)videoaddr;
	VID_WIDTH = width;
	VID_HEIGHT = height;
	VID_FWIDTH = fontwidth;
	VID_FHEIGHT = fontheight;
	VID_LINE_OFFSET = video_pitch;
	psf_data = (uint8_t*)psfAddr;
    pixel_buffer = (uint8_t*)VID_MEMORY;
	highresMode = true;
	graphicType = video_type;
	if (graphicType != 2) {
	  for (size_t y = 0; y < VID_HEIGHT; y++) {
		for (size_t x = 0; x < VID_WIDTH; x++) {
			const size_t index = y * VID_WIDTH + x;
			pixel_buffer[index] = VGA_COLOR_BLACK;
		}
	  }
	  VID_WIDTH = width / fontwidth;
	  VID_HEIGHT = height / fontheight;
	} else {
		for (size_t y = 0; y < VID_HEIGHT; y++) {
			for (size_t x = 0; x < VID_WIDTH; x++) {
				const size_t index = y * VID_WIDTH + x;
				terminal_buffer[index] = vga_entry(' ', terminal_color);
			}
		}
	}
}

void update_cursor(int x, int y)  {
	uint16_t pos = y * VGA_WIDTH + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void drawChar(unsigned char c, size_t x, size_t y, uint8_t color) {
  size_t index = (x * VID_FWIDTH) + (y * (VID_LINE_OFFSET * VID_FHEIGHT));
  for (uint8_t yp = 0; yp < VID_FHEIGHT; yp++) {
	uint8_t value = psf_data[(c * VID_FHEIGHT) + yp];
    for (uint8_t xp = 0; xp < VID_FWIDTH; xp++) {
	  if ((value >> (7 - xp)) & 1) {
        pixel_buffer[index + xp + (yp * VID_LINE_OFFSET)] = color & 0xF;
	  } else {
		pixel_buffer[index + xp + (yp * VID_LINE_OFFSET)] = color >> 4;
	  }
    }
  }
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	if (!highresMode) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
	return;
	} else if (graphicType == 2){
		const size_t index = y * VGA_WIDTH + x;
		terminal_buffer[index] = vga_entry(c, color);
	} else {
		drawChar(c, x, y, color);
		//pixel_buffer[index] = color;
	}
}

void terminal_putchar(char c) {
	unsigned char uc = c;
	if (c == '\n') {
		terminal_row += 1;
		terminal_column = 0;
	} else {
	terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
	if (!highresMode || graphicType == 2) {
	  if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
		  terminal_row = 0;
	  }
	} else {
		if (++terminal_column == VID_WIDTH) {
			terminal_column = 0;
			if (++terminal_row == VID_HEIGHT)
				terminal_row = 0;
		}
	}
  }
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++) {
		terminal_putchar(data[i]);
	}
	update_cursor(terminal_column,terminal_row);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}
