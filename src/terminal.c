#include "terminal.h"
#include "string.h"

size_t terminal_row, terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

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

void terminal_scroll_text() {
    // Move all the text one step up
    for(size_t y = 1; y < VGA_HEIGHT - 1; y++)
        for(size_t x = 0; x < VGA_WIDTH; x++)
            terminal_buffer[(y - 1) * VGA_WIDTH + x] = terminal_buffer[y * VGA_HEIGHT + x];
    
    // Reset bottom row
    for(size_t x = 0; x < VGA_WIDTH; x++)
        terminal_put_entry_at(' ', vga_entry_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK), VGA_HEIGHT - 1, x);
}