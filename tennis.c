#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>

#include "console.h"
#include "linalg.h"

void sigint_handler(int signum) {
    // move down height lines
    printf("\x1b[%dB", HEIGHT);
    // reenable the cursor
    enable_cursor();
    fflush(stdout);
    exit(1);
}

typedef char Frame[WIDTH][HEIGHT];

// No clue how big this should be
static Frame framebuffer[64];

typedef struct GameObj {
    Vec pos;
    Vec vel;
    char c;
} GameObj;

typedef struct GameState {
    GameObj guy1;
    GameObj guy2;
    GameObj ball;
    uint64_t tick;
    
} GameState;

void draw_field(Frame* frame) {
    size_t width = WIDTH;
    size_t height = HEIGHT;
    
    int scalex = width/8;
    int quit = 0;
    for(int y=0; y<height; ++y) {
        for(int x=0; x<width; ++x) {
            if(x == 0 || y == 0 || x == width - 1 || y == height -1) {
                (*frame)[x][y] = '#';
            }
            else if(y==height/2) {
                (*frame)[x][y] = '=';
            }
            else if ((x == width/2 - 1 || x == width/2) && (y > height/2 - height/4 && y < height/2 + height/4)) {
                (*frame)[x][y] = '|';
            }
            else if (x == scalex || x == width - scalex -1) {
                (*frame)[x][y] = '|';
            }
            else if ((y == height/2 - height/4 || y == height/2 + height/4) && (x < width - scalex -1 && x > scalex)) {
                (*frame)[x][y] = '-';
            }
            else {
                (*frame)[x][y] = ' ';
            }
        }
    }
}

// draws picture with character at position
void generate_frame(Vec ball, int last1, int last2, int ballheight, Vec guy1, Vec guy2, Frame *frame) {
    size_t width = WIDTH;
    size_t height = HEIGHT;

    int ballx = (int)ball.x;
    int bally = (int)ball.y;

    int guy1x = (int)guy1.x;
    int guy1y = (int)guy1.y;
    
    int guy2x = (int)guy2.x;
    int guy2y = (int)guy2.y;

    draw_field(frame);

    if(ballheight == 0) {
        (*frame)[ballx][bally] = '.';
    }
    else if (ballheight == 1) {
        (*frame)[ballx][bally] = '*';
    }
    else {
        (*frame)[ballx][bally] = '@';
    }

    // legacy, redo this somehow
    if(last1) {
        (*frame)[guy1x][guy1y] = '\\';
    } else {
        (*frame)[guy1x][guy1y] = '/';
    }
    if(last2) {
        (*frame)[guy2x][guy2y] = '/';
    }
    else {
        (*frame)[guy2x][guy2y] = '\\';
    }
}

void render_frame(Frame *frame) {
    for(int y=0; y<HEIGHT; y++) {
        char row[WIDTH];
        // 16 bit color
        // printf("\x1b[42m");
        // 256 bit color
        printf("\x1b[48;5;28m");
        for(int x=0; x<WIDTH; x++) {
            // TODO: put this into GameState
            // if(x == frame->ball.x && y == frame->ball.y) {
            //     // 256 bit colors
            //     printf("\x1b[38;5;220m");
            //     fputc(frame[x][y], stdout);
            //     printf("\x1b[0m");
            //     printf("\x1b[48;5;28m");
            // // make player background white with black "racket"
            // } else if (x == frame->guy1.x && y == frame->guy1.y) {
            //     printf("\x1b[38;5;196m");
            //     printf("\x1b[48;5;255m");
            //     fputc(frame[x][y], stdout);
            //     printf("\x1b[0m");
            //     printf("\x1b[48;5;28m");
            // } else if (x == frame->guy2.x && y == frame->guy2.y) {
            //     printf("\x1b[38;5;27m");
            //     printf("\x1b[48;5;255m");
            //     fputc(frame[x][y], stdout);
            //     printf("\x1b[0m");
            //     printf("\x1b[48;5;28m");

            // } else {
            //     fputc(frame[x][y], stdout);
            // }
            //row[x] = frame[x][y];
            fputc((*frame)[x][y], stdout);
        }
        //fwrite(row, WIDTH, 1, stdout);
        printf("\x1b[0m");
        fputc('\n', stdout);
    }
    // If we disable flushing on newlines
    // fflush(stdout);
}

int main(int argc, char** argv) {
    // Dont flush stdout every newline
    // https://stackoverflow.com/questions/40227807/how-can-i-print-a-string-with-newline-without-flushing-the-buffer
    // setvbuf(stdout, NULL, _IOFBF, 0);

    // Disable cursor and reenable on exit
    struct sigaction newaction;
    newaction.sa_handler = sigint_handler;
    sigaction(SIGINT, &newaction, NULL);

    disable_cursor();

    // TODO: make input frame dependent on console size
    // Frame frame = init_frame();
    Frame frame;

    Vec guy1;
    Vec guy2;

    int x, y;
    if(argc < 3) {
        x = 15;
        y = 4;
    }
    else {
        x = atoi(argv[1]);
        y = atoi(argv[2]);
    }
    guy1 = init_vec(x,y);
    guy2 = init_vec(WIDTH - x - 1, HEIGHT - y -1);
    if(argc == 5){
        guy2 = init_vec(atoi(argv[3]), atoi(argv[4]));
    }
    Vec diff = v_diff(guy1, guy2);

    // give guy1 the ball first
    Vec pos = guy1;

    // choose velocity pointing to guy2
    Vec vel = norm(diff);//init_vec(1,1);
    // vel.x+=0.75;

    int last1 = 1;
    int last2 = 1;
    int height = 0;
    while(1) {
        pos = vec_sum(pos, vel);
        if(pos.y < diff.y/6 + y|| (pos.y < diff.y + y && pos.y >= (5 * diff.y / 6) + y)){
            height = 0;
        }
        if((pos.y < 2* diff.y/6 + y && pos.y >= diff.y/6 + y) || (pos.y >= 4 * diff.y/6 + y && pos.y < 5*diff.y/6 + y ) ) {
            height = 1;
        }
        if(pos.y < 4*diff.y/6 + y && pos.y >= 3*diff.y/6 + y) {
            height = 2;
        }
        if(((pos.x >= guy1.x - 2 && pos.x <= guy1.x + 2) && pos.y == guy1.y)
            || ((pos.x <= guy2.x + 2 && pos.x >= guy2.x - 2) && pos.y == guy2.y)) {
            vel = flip(vel);
            last1 = !last1;
            last2 = !last2;
        }
        if(pos.x >= WIDTH-1 || pos.x <= 1) {
            vel = flip_x(vel);
        }
        if(pos.y >= HEIGHT-1 || pos.y <= 1) {
            vel = flip_y(vel);
        }
        generate_frame(pos, last1, last2, height, guy1, guy2, &frame);
        render_frame(&frame);
        reset_cursor();
        usleep(1000 * 1000 / FPS);
    }
    return 0;
}
