#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <pthread.h>

#include "console.h"
#include "linalg.h"

// pthreads for game and controller for both players
pthread_t gtid, ctid;
pthread_mutex_t framelock;
pthread_mutex_t statelock;

typedef char Frame[WIDTH][HEIGHT];

static Frame frame;

enum Action {
    STRAIGHT = 0,
    LEFT = 1,
    RIGHT = 2,
};

typedef struct GameObj {
    Vec pos;
    Vec vel;
    char c;
    int action;
} GameObj;

GameObj init_gameobj(Vec pos, Vec vel, char c) {
    GameObj gameobj = {
        .pos = pos,
        .vel = vel,
        .c = c,
        .action = STRAIGHT
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
    int courtcolor;
    
} GameState;

static GameState gamestate;

void draw_field() {
    size_t width = WIDTH;
    size_t height = HEIGHT;
    
    int scalex = width/8;
    int quit = 0;
    pthread_mutex_lock(&framelock);
    for(int y=0; y<height; ++y) {
        for(int x=0; x<width; ++x) {
            if(x == 0 || y == 0 || x == width - 1 || y == height -1) {
                frame[x][y] = '#';
            }
            else if(y==height/2) {
                frame[x][y] = '=';
            }
            else if ((x == width/2 - 1 || x == width/2) && (y > height/2 - height/4 && y < height/2 + height/4)) {
                frame[x][y] = '|';
            }
            else if (x == scalex || x == width - scalex -1) {
                frame[x][y] = '|';
            }
            else if ((y == height/2 - height/4 || y == height/2 + height/4) && (x < width - scalex -1 && x > scalex)) {
                frame[x][y] = '-';
            }
            else {
                frame[x][y] = ' ';
            }
        }
    }
    pthread_mutex_unlock(&framelock);
}

// draws picture with character at position
void generate_frame() {
    pthread_mutex_lock(&statelock);
    GameObj guy1 = gamestate.guy1;
    GameObj guy2 = gamestate.guy2;
    GameObj ball = gamestate.ball;
    pthread_mutex_unlock(&statelock);
    
    int ballx = (int)ball.pos.x;
    int bally = (int)ball.pos.y;
    int ballz = (int)ball.pos.z; // ( ͡° ͜ʖ ͡°)

    int guy1x = (int)guy1.pos.x;
    int guy1y = (int)guy1.pos.y;
    
    int guy2x = (int)guy2.pos.x;
    int guy2y = (int)guy2.pos.y;

    draw_field();

    pthread_mutex_lock(&framelock);
    if(ballz == 0) {
        frame[ballx][bally] = '.';
    }
    else if (ballz == 1) {
        frame[ballx][bally] = '*';
    }
    else {
        frame[ballx][bally] = '@';
    }

    frame[guy1x][guy1y] = guy1.c;
    frame[guy2x][guy2y] = guy2.c;

    if(guy1.action == LEFT && guy1x > 0) {
        frame[guy1x-1][guy1y] = '<';
    }
    if(guy1.action == RIGHT && guy1x < WIDTH-1) {
        frame[guy1x+1][guy1y] = '>';
    }
    if(guy1.action == STRAIGHT) {
        frame[guy1x][guy1y+1] = 'v';
    }
    if(guy2.action == LEFT && guy2x > 0) {
        frame[guy2x-1][guy2y] = '<';
    }
    if(guy2.action == RIGHT && guy2x < WIDTH-1) {
        frame[guy2x+1][guy2y] = '>';
    }
    if(guy2.action == STRAIGHT) {
        frame[guy2x][guy2y-1] = '^';

    }
    pthread_mutex_unlock(&framelock);
}

void render_frame() {

    pthread_mutex_lock(&statelock);
    int court = gamestate.courtcolor;
    int guy1x = (int)(gamestate.guy1.pos.x);
    int guy1y = (int)(gamestate.guy1.pos.y);
    int guy2x = (int)(gamestate.guy2.pos.x);
    int guy2y = (int)(gamestate.guy2.pos.y);
    int ballx = (int)(gamestate.ball.pos.x);
    int bally = (int)(gamestate.ball.pos.y);
    pthread_mutex_unlock(&statelock);

    pthread_mutex_lock(&framelock);
    for(int y=0; y<HEIGHT; y++) {
        BG_COLOR(court);
        for(int x=0; x<WIDTH; x++) {
            if(x == ballx && y == bally) {
                // 256 bit colors
                FG_COLOR(YELLOW);
                fputc(frame[x][y], stdout);
                RESET_COLOR;
                BG_COLOR(court);
            // make player background white with colored "racket"
            } else if (x == guy1x && y == guy1y) {
                FG_BG_COLOR(RED, WHITE);
                fputc(frame[x][y], stdout);
                RESET_COLOR;
                BG_COLOR(court);
            } else if (x == guy2x && y == guy2y) {
                FG_BG_COLOR(BLUE, WHITE);
                fputc(frame[x][y], stdout);
                RESET_COLOR;
                BG_COLOR(court);
            } else {
                fputc(frame[x][y], stdout);
            }
        }
        RESET_COLOR;
        fputc('\n', stdout);
    }
    pthread_mutex_unlock(&framelock);
}

void* game(void* vargs) {
    // Guys dont have velocity yet
    int x = 15;
    int y = 4;
    // TODO : input character for firstname?
    GameObj guy1 = init_gameobj(init_vec(x,y,0), init_vec(0,0,0), '1');
    GameObj guy2 = init_gameobj(init_vec(WIDTH-x-1, HEIGHT-y-1,0), init_vec(0,0,0), '2');

    // Vector from guy1 to guy2
    Vec diff = v_diff(guy1.pos, guy2.pos);

    // Start ball with guy1
    // Initial velocity is normalized in the direction of guy2
    GameObj ball = init_gameobj(guy1.pos, init_vec(0.5,0.5,0), '.');

    GameState* gs = &gamestate;

    pthread_mutex_lock(&statelock);
    gs->guy1 = guy1;
    gs->guy2 = guy2;
    gs->ball = ball;
    pthread_mutex_unlock(&statelock);

    while(1) {
        pthread_mutex_lock(&statelock);
        gs->ball.pos = vec_sum(gs->ball.pos, gs->ball.vel);
        gs->guy1.pos = vec_sum(gs->guy1.pos, gs->guy1.vel);
        gs->guy2.pos = vec_sum(gs->guy2.pos, gs->guy2.vel);
        if(gs->guy1.pos.x >= WIDTH) {
            gs->guy1.vel.x = 0;
            gs->guy1.pos.x = WIDTH-1;
        }
        if(gs->guy1.pos.x <= 0) {
            gs->guy1.vel.x = 0;
            gs->guy1.pos.x = 0;
        }
        if(gs->guy2.pos.x >= WIDTH) {
            gs->guy2.vel.x = 0;
            gs->guy2.pos.x = WIDTH-1;
        }
        if(gs->guy2.pos.x <= 0) {
            gs->guy2.vel.x = 0;
            gs->guy2.pos.x = 0;
        }
        if(gs->ball.pos.y < diff.y/6 + y || 
          (gs->ball.pos.y < diff.y + y && gs->ball.pos.y >= (5 * diff.y / 6) + y)){
            gs->ball.pos.z = 0;
        }
        if((gs->ball.pos.y < 2* diff.y/6 + y && gs->ball.pos.y >= diff.y/6 + y) || 
           (gs->ball.pos.y >= 4 * diff.y/6 + y && gs->ball.pos.y < 5*diff.y/6 + y )) {
            gs->ball.pos.z = 1;
        }
        if(gs->ball.pos.y < 4*diff.y/6 + y && gs->ball.pos.y >= 3*diff.y/6 + y) {
            gs->ball.pos.z = 2;
        }
        if(collision(gs->guy1, gs->ball, 2)) {
            if(gs->guy1.action == STRAIGHT) {
                gs->ball.vel.x = 0;
                gs->ball.vel.y = 0.5;
            }
            if(gs->guy1.action == LEFT) {
                gs->ball.vel.x = -0.5;
                gs->ball.vel.y = 0.5;
            }
            if(gs->guy1.action == RIGHT) {
                gs->ball.vel.x = 0.5;
                gs->ball.vel.y = 0.5;
            }
        }
        if(collision(gs->guy2, gs->ball, 2)) {
            if(gs->guy2.action == STRAIGHT) {
                gs->ball.vel.x = 0;
                gs->ball.vel.y = -0.5;
            }
            if(gs->guy2.action == LEFT) {
                gs->ball.vel.x = -0.5;
                gs->ball.vel.y = -0.5;
            }
            if(gs->guy2.action == RIGHT) {
                gs->ball.vel.x = 0.5;
                gs->ball.vel.y = -0.5;
            }
        }
        if(gs->ball.pos.x >= WIDTH-1 || gs->ball.pos.x <= 1) {
            gs->ball.vel = flip_x(gs->ball.vel);
        }
        if(gs->ball.pos.y >= HEIGHT-1 || gs->ball.pos.y <= 1) {
            gs->ball.vel = flip_y(gs->ball.vel);
        }
        pthread_mutex_unlock(&statelock);
        generate_frame();
        render_frame();
        CURSOR_TOP; 
        usleep(1000 * 1000 / FPS);
    }
}

// Some deep lore in avoiding ncurses
// https://web.archive.org/web/20180401093525/http://cc.byexamples.com/2007/04/08/non-blocking-user-input-in-loop-without-ncurses/
// https://stackoverflow.com/questions/20349585/c-library-function-to-check-the-keypress-from-keyboard-in-linux
// https://stackoverflow.com/questions/21903495/how-to-print-a-variable-continuously-in-a-loop-and-terminate-with-a-hit-of-escap
#define NB_ENABLE 1
#define NB_DISABLE 0

int kbhit() {
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

void nonblock(int state) {
    struct termios ttystate;
    
    // get terminal state
    tcgetattr(STDIN_FILENO, &ttystate);

    if(state == NB_ENABLE) {
        // turn off canonical mode
        ttystate.c_lflag &= ~ICANON;
        // minimum number of input read
        ttystate.c_cc[VMIN] = 1;
    }
    else if(state == NB_DISABLE) {
        // turn on canonical mode
        ttystate.c_lflag |= ICANON;
    }
    // set the terminal attributes
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

// Expects vargs to be (int*)
void* controller(void* vargs) {
    char c;

    GameObj* guy1 = &gamestate.guy1;
    GameObj* guy2 = &gamestate.guy2;

    int i=0;
    nonblock(NB_ENABLE);
    while(1) {
        usleep(1);
        i=kbhit();
        if(i != 0) {
            c = fgetc(stdin);
            pthread_mutex_lock(&statelock);
            switch(c) {
            // guy1
            case 'a':
                guy1->vel.x = -0.5; 
                break;
            case 'd':
                guy1->vel.x = 0.5;
                break;
            case 'w':
                guy1->vel.x = 0;
                break;
            case 'q':
                guy1->action = LEFT;
                break;
            case 's':
                guy1->action = STRAIGHT;
                break;
            case 'e':
                guy1->action = RIGHT;
                break;
            // guy2
            case 'j':
                guy2->vel.x = -0.5; 
                break;
            case 'l':
                guy2->vel.x = 0.5;
                break;
            case 'k':
                guy2->vel.x = 0;
                break;
            case 'u':
                guy2->action = LEFT;
                break;
            case 'i':
                guy2->action = STRAIGHT;
                break;
            case 'o':
                guy2->action = RIGHT;
                break;
            default:
            }
            pthread_mutex_unlock(&statelock);
        }
        else {
            // guy->vel.x = 0;
            // guy->vel.y = 0;
        }
    }
}
// end of deep lore in avoiding ncurses

void disable_stdin_echo() {
    struct termios term;
    tcgetattr(fileno(stdin), &term);

    term.c_lflag &= ~ECHO;
    tcsetattr(fileno(stdin), 0, &term);
}

void enable_stdin_echo() {
    struct termios term;
    tcgetattr(fileno(stdin), &term);

    term.c_lflag |= ECHO;
    tcsetattr(fileno(stdin), 0, &term);
}

void sigint_handler(int signum) {
    // move down height lines
    CURSOR_DOWN(HEIGHT);
    ENABLE_CURSOR;

    // kill game and controller pthreads
    pthread_kill(gtid, 0);
    pthread_kill(ctid, 0);

    pthread_mutex_destroy(&framelock);
    pthread_mutex_destroy(&statelock);

    enable_stdin_echo();

    fflush(stdout);
    exit(1);
}

int main(int argc, char** argv) {

    pthread_mutex_init(&framelock, NULL);
    pthread_mutex_init(&statelock, NULL);

    // determine court color, default green
    gamestate.courtcolor = GREEN;
    if(argc > 1) {
        if(strcmp(argv[1], "grass") == 0) {
            gamestate.courtcolor = GREEN;
        }
        if(strcmp(argv[1], "clay") == 0) {
            gamestate.courtcolor = ORANGE;
        }
        if(strcmp(argv[1], "court") == 0) {
            gamestate.courtcolor = INDOOR; // court blue
        }
    }

    // Disable cursor and reenable on exit
    struct sigaction newaction;
    newaction.sa_handler = sigint_handler;
    sigaction(SIGINT, &newaction, NULL);

    DISABLE_CURSOR;

    disable_stdin_echo();

    pthread_create(&gtid, NULL, game, NULL);
    pthread_create(&ctid, NULL, controller, NULL);

    pthread_join(gtid, NULL);
    pthread_join(ctid, NULL);
    // unreachable
    fprintf(stderr, "Unreachable!\n");
    exit(1);
}
