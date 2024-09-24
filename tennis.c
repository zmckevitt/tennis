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

GameObj init_gameobj(Vec pos, Vec vel, char c) {
    GameObj gameobj = {
        .pos = pos,
        .vel = vel,
        .c = c
    };
    return gameobj;
}

// returns 1 if obj2 is within x radius of obj1 
// returns 0 otherwise
int collision(GameObj obj1, GameObj obj2, int radius) {

    return (obj2.pos.x >= obj1.pos.x - radius && obj2.pos.x <= obj1.pos.x + radius)
            && obj2.pos.y == obj1.pos.y;
}

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
void generate_frame(GameObj ball, GameObj guy1, GameObj guy2, Frame *frame) {
    size_t width = WIDTH;
    size_t height = HEIGHT;

    int ballx = (int)ball.pos.x;
    int bally = (int)ball.pos.y;
    int ballz = (int)ball.pos.z; // ( ͡° ͜ʖ ͡°)

    int guy1x = (int)guy1.pos.x;
    int guy1y = (int)guy1.pos.y;
    
    int guy2x = (int)guy2.pos.x;
    int guy2y = (int)guy2.pos.y;

    draw_field(frame);

    if(ballz == 0) {
        (*frame)[ballx][bally] = '.';
    }
    else if (ballz == 1) {
        (*frame)[ballx][bally] = '*';
    }
    else {
        (*frame)[ballx][bally] = '@';
    }

    (*frame)[guy1x][guy1y] = guy1.c;
    (*frame)[guy2x][guy2y] = guy2.c;
}

void render_frame(GameObj ball, GameObj guy1, GameObj guy2, Frame *frame) {
    for(int y=0; y<HEIGHT; y++) {
        printf("\x1b[48;5;28m");
        for(int x=0; x<WIDTH; x++) {
            if(x == ball.pos.x && y == ball.pos.y) {
                // 256 bit colors
                printf("\x1b[38;5;220m");
                fputc((*frame)[x][y], stdout);
                printf("\x1b[0m");
                printf("\x1b[48;5;28m");
            // make player background white with colored "racket"
            } else if (x == guy1.pos.x && y == guy1.pos.y) {
                printf("\x1b[38;5;196m");
                printf("\x1b[48;5;255m");
                fputc((*frame)[x][y], stdout);
                printf("\x1b[0m");
                printf("\x1b[48;5;28m");
            } else if (x == guy2.pos.x && y == guy2.pos.y) {
                printf("\x1b[38;5;27m");
                printf("\x1b[48;5;255m");
                fputc((*frame)[x][y], stdout);
                printf("\x1b[0m");
                printf("\x1b[48;5;28m");

            } else {
                fputc((*frame)[x][y], stdout);
            }
        }
        printf("\x1b[0m");
        fputc('\n', stdout);
    }
}

int main(int argc, char** argv) {

    // Disable cursor and reenable on exit
    struct sigaction newaction;
    newaction.sa_handler = sigint_handler;
    sigaction(SIGINT, &newaction, NULL);

    disable_cursor();

    Frame frame;
    
    // Guys dont have velocity yet
    int x = 15;
    int y = 4;
    GameObj guy1 = init_gameobj(init_vec(x,y,0), init_vec(0,0,0), '/');
    GameObj guy2 = init_gameobj(init_vec(WIDTH-x-1, HEIGHT-y-1,0), init_vec(0,0,0), '\\');

    // Vector from guy1 to guy2
    Vec diff = v_diff(guy1.pos, guy2.pos);

    // Start ball with guy1
    // Initial velocity is normalized in the direction of guy2
    GameObj ball = init_gameobj(guy1.pos, norm(diff), '.');

    while(1) {
        ball.pos = vec_sum(ball.pos, ball.vel);
        if(ball.pos.y < diff.y/6 + y || (ball.pos.y < diff.y + y && ball.pos.y >= (5 * diff.y / 6) + y)){
            ball.pos.z = 0;
        }
        if((ball.pos.y < 2* diff.y/6 + y && ball.pos.y >= diff.y/6 + y) || 
           (ball.pos.y >= 4 * diff.y/6 + y && ball.pos.y < 5*diff.y/6 + y )) {
            ball.pos.z = 1;
        }
        if(ball.pos.y < 4*diff.y/6 + y && ball.pos.y >= 3*diff.y/6 + y) {
            ball.pos.z = 2;
        }
        if(collision(guy1, ball, 2) || collision(guy2, ball, 2)) {
            ball.vel = flip_xy(ball.vel);
            if(guy1.c == '/') {
                guy1.c = '\\';
            } else {
                guy1.c = '/';
            }
            if(guy2.c == '/') {
                guy2.c = '\\';
            } else {
                guy2.c = '/';
            }
        }
        if(ball.pos.x >= WIDTH-1 || ball.pos.x <= 1) {
            ball.vel = flip_x(ball.vel);
        }
        if(ball.pos.y >= HEIGHT-1 || ball.pos.y <= 1) {
            ball.vel = flip_y(ball.vel);
        }
        generate_frame(ball, guy1, guy2, &frame);
        render_frame(ball, guy1, guy2, &frame);
        reset_cursor();
        usleep(1000 * 1000 / FPS);
    }
    return 0;
}
