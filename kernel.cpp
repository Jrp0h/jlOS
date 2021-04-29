#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

size_t strlen(const char* str)  {
    size_t len = 0;

    while(str[len])
        len++;

    return len;
}

class VGA {
public:
    enum class Color {
        BLACK = 0,
        BLUE = 1,
        GREEN = 2,
        CYAN = 3,
        RED = 4,
        MAGENTA = 5,
        BROWN = 6,
        LIGHT_GRAY = 7,
        DARK_GRAY = 8,
        LIGHT_BLUE = 9,
        LIGHT_GREEN = 10,
        LIGHT_CYAN = 11,
        LIGHT_RED = 12,
        LIGHT_MAGENTA = 13,
        LIGHT_BROWN = 14,
        WHITE = 15
    };

    static const size_t Width = 80;
    static const size_t Height = 25;

    static inline uint8_t EntryColor(Color fg, Color bg) {
        return (uint8_t)fg | (uint8_t)bg << 4;
    }

    static inline uint16_t Entry(unsigned char uc , uint8_t color) {
        return (uint16_t) uc | (uint16_t) color << 8;
    }

};

class Terminal {
public:
    Terminal() {
        m_Row = 0;
        m_Column = 0;

        m_Color = VGA::EntryColor(VGA::Color::LIGHT_GRAY, VGA::Color::BLACK);

        m_Buffer = (uint16_t*) 0xB8000;

        for (size_t y = 0; y < VGA::Height; y++) {
            for (size_t x = 0; x < VGA::Width; x++) {
                const size_t index = y * VGA::Width + x;
                m_Buffer[index] = VGA::Entry(' ', m_Color);
            }
        }
    }

void WriteString(const char* data) {
    Write(data, strlen(data));
}

void SetColor (uint8_t color){
    m_Color = color;
}

void Write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) 
        PutChar(data[i]);
}

void PutChar(const char c) {

    if(c == '\n')
    {
        m_Row++;

        if(m_Row >= VGA::Height) {
            m_Row = 0;
        }

        m_Column = 0;
        return;
    }

    if(c == '\t')
    {
        m_Column += 4;

        if(m_Column >= VGA::Width)
            m_Column = 0;
        
        return;
    }

    PutEntryAt(c, m_Color, m_Column, m_Row);

    if(++m_Column == VGA::Width) {
        m_Column = 0;
        if(++m_Row == VGA::Height) {
            m_Row = 0;
        }
    }
}

void PutEntryAt(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA::Width + x;
    m_Buffer[index] = VGA::Entry(c, color);
}

void PutRainbowText(const char* str) {
    size_t text_length = strlen(str);

    for(size_t i = 0; i < text_length; i++) {
        SetColor(VGA::EntryColor((VGA::Color)((i % 5) + 9), VGA::Color::BLACK));
        PutChar(str[i]);
    }
}

void ResetColor() {
    SetColor(VGA::EntryColor(VGA::Color::LIGHT_GRAY, VGA::Color::BLACK));
}
private:
    size_t m_Row, m_Column;
    uint8_t m_Color;
    uint16_t* m_Buffer;
};


void kernel_main(void) {
    Terminal terminal;
    terminal.PutRainbowText("Hello");
}
