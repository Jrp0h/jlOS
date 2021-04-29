#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GRAY = 7,
    VGA_COLOR_DARK_GRAY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc , uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str)  {
    size_t len = 0;

    while(str[len])
        len++;

    return len;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row, terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize();
void terminal_write_string(const char* data);
void terminal_set_color (uint8_t color);
void terminal_write(const char* data, size_t size);
void terminal_put_char(const char c);
void terminal_put_entry_at(char c, uint8_t color, size_t x, size_t y);
void terminal_put_rainbow_text(const char* c);
void terminal_reset_color();


void terminal_initialize()  {
    terminal_row = 0;
    terminal_column = 0;

    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);

    terminal_buffer = (uint16_t*) 0xB8000;

    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_write_string(const char* data) {
    terminal_write(data, strlen(data));
}

void terminal_set_color (uint8_t color){
    terminal_color = color;
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) 
        terminal_put_char(data[i]);
}

void terminal_put_char(const char c) {

    if(c == '\n')
    {
        terminal_row++;

        if(terminal_row >= VGA_HEIGHT) {
            terminal_row = 0;
        }

        terminal_column = 0;
        return;
    }

    if(c == '\t')
    {
        terminal_column += 4;

        if(terminal_column >= VGA_WIDTH)
            terminal_column = 0;
        
        return;
    }

    terminal_put_entry_at(c, terminal_color, terminal_column, terminal_row);

    if(++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if(++terminal_row == VGA_HEIGHT) {
            terminal_row = 0;
        }
    }
}

void terminal_put_entry_at(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_put_rainbow_text(const char* str) {
    size_t text_length = strlen(str);

    for(size_t i = 0; i < text_length; i++) {
        terminal_set_color(vga_entry_color((enum vga_color)((i % 5) + 9), VGA_COLOR_BLACK));
        terminal_put_char(str[i]);
    }
}

void terminal_reset_color() {
    terminal_set_color(vga_entry_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK));
}

void kernel_main(void) {
    terminal_initialize();

    terminal_put_rainbow_text("Hello from the kernel");
    /* terminal_reset_color(); */
    /* terminal_write_string("This is normal color\n\tTab works"); */
    /* terminal_set_color(vga_entry_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK)); */
}
