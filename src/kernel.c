#include "terminal.h"

void kernel_main() {
    terminal_initialize();
    terminal_put_rainbow_text("test");
}