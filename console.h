#ifndef GRAPHICS_H
#define GRAPHICS_H

#define HEIGHT 41
#define WIDTH  64
#define FPS    30

#define GREEN  28
#define ORANGE 202
#define INDOOR 26 
#define YELLOW 220
#define RED    196
#define BLUE   27
#define WHITE  255
#define RESET  0

#define DISABLE_CURSOR    printf("\x1b[?25l");
#define ENABLE_CURSOR     printf("\x1b[?25h");
#define CURSOR_TOP        printf("\x1b[%dD\x1b[%dA", WIDTH, HEIGHT);
#define CURSOR_DOWN(X)    printf("\x1b[%dB", (X));

// 256 color support required!
// TODO: add support for 16 bit colors
#define FG_COLOR(X)       printf("\x1b[38;5;%dm", (X))
#define BG_COLOR(X)       printf("\x1b[48;5;%dm", (X))
#define FG_BG_COLOR(X, Y) printf("\x1b[38;5;%dm\x1b[48;5;%dm", (X), (Y))
#define RESET_COLOR       printf("\x1b[0m")

#endif // GRAPHICS_H
