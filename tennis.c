#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "console.h"
#include "linalg.h"

void sigint_handler(int signum) {
    // move down height lines
    printf("\x1b[%dB", HEIGHT);
    enable_cursor();
    fflush(stdout);
    exit(1);
}

// TODO: find way to make this dynamic
typedef struct Frame {
    size_t width;
    size_t height;
    char buf[WIDTH][HEIGHT];
} Frame;

Frame init_frame() {
    Frame frame = {
        .width = WIDTH,
        .height = HEIGHT,
    };
    return frame;
}

// draws picture with character at position
int generate_frame(struct vec pos, int last1, int last2, int ballheight, struct vec guy1, struct vec guy2, Frame *frame) {

    size_t width = frame->width;
    size_t height = frame->height;

    int posx = (int)pos.x;
    int posy = (int)pos.y;

    int scalex = width/8;
    int quit = 0;
    for(int y=0; y<height; ++y) {
        for(int x=0; x<width; ++x) {
            if(x == 0 || y == 0 || x == width - 1 || y == height -1) {
                frame->buf[x][y] = '#';
            }
            else if(y==height/2) {
                frame->buf[x][y] = '=';
            }
            else if ((x == width/2 - 1 || x == width/2) && (y > height/2 - height/4 && y < height/2 + height/4)) {
                frame->buf[x][y] = '|';
            }
            else if (x == scalex || x == width - scalex -1) {
                frame->buf[x][y] = '|';
            }
            else if ((y == height/2 - height/4 || y == height/2 + height/4) && (x < width - scalex -1 && x > scalex)) {
                frame->buf[x][y] = '-';
            }
            else {
                frame->buf[x][y] = ' ';
            }
            if(posx == x && posy == y) {
                if(posx < scalex || posx > width-scalex-1) {
                    frame->buf[x][y] = 'X';
                }
                else {
                    if(ballheight == 0) {
                        frame->buf[x][y] = '.';
                    }
                    else if (ballheight == 1) {
                        frame->buf[x][y] = '*';
                    }
                    else {
                        frame->buf[x][y] = '@';
                    }
                }
            }
            if(guy1.x == x && guy1.y == y) {
                if(last1){
                    frame->buf[x][y] = '\\';
                }
                else {
                    frame->buf[x][y] = '/';
                }
            }
            if(guy2.x == x && guy2.y == y) {
                if(last2) {
                    frame->buf[x][y] = '/';
                }
                else {
                    frame->buf[x][y] = '\\';
                }
            }
        }
    }
    return 0;
}

void render_frame(Frame *frame) {
    for(int y=0; y<frame->height; y++) {
        char row[frame->width];
        for(int x=0; x<frame->width; x++) {
            // fputc(frame->buf[x][y], stdout);
            row[x] = frame->buf[x][y];
        }
        fwrite(row, frame->width, 1, stdout);
        fputc('\n', stdout);
    }
}

// Ideas:
//  Movable characters
//  Extend graphics terminal to the right for text ourputs 

// TODO
//  Make draw() args into a struct 
//  Implement floating point vectors
int main(int argc, char** argv) {

    // Disable cursor and reenable on exit
    struct sigaction newaction;
    newaction.sa_handler = sigint_handler;
    sigaction(SIGINT, &newaction, NULL);

    disable_cursor();

    // TODO: make input frame dependent on console size
    Frame frame = init_frame();

    struct vec guy1;
    struct vec guy2;

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
    struct vec diff = v_diff(guy1, guy2);

    // give guy1 the ball first
    struct vec pos = guy1;

    // choose velocity pointing to guy2
    struct vec vel = norm(diff);//init_vec(1,1);
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
