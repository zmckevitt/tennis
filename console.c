#include <stdio.h>

#include "console.h"

void disable_cursor() {
    printf("\x1b[?25l");
}

void enable_cursor() {
    printf("\x1b[?25h");
}

// returns cursor to starting position
void reset_cursor() {
    printf("\x1b[%dD", WIDTH);
    printf("\x1b[%dA", HEIGHT);
}
