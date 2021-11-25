#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

//////////////////////////////////////////////////////////////
//
// MATH
//
//////////////////////////////////////////////////////////////

struct vec {
    double x;
    double y;
};

// Do we need a constructor?
struct vec init_vec(float x, float y) {
    struct vec v;
    v.x = x;
    v.y = y;

    return v;
}

struct vec vec_sum(struct vec v1, struct vec v2) {
    struct vec v;

    v.x = v1.x + v2.x;
    v.y = v1.y + v2.y;

    return v;
}

double vec_length(struct vec v) {
    return sqrt(v.x*v.x + v.y*v.y);
}

struct vec flip_x(struct vec v) {
    struct vec tmp;

    tmp.x = -1 * v.x;
    tmp.y = v.y;

    return tmp;
}

struct vec flip_y(struct vec v) {
    struct vec tmp;

    tmp.x = v.x;
    tmp.y = -1 * v.y;

    return tmp;
}

struct vec flip(struct vec v) {
    struct vec tmp;

    tmp.x = -1 * v.x;
    tmp.y = -1 * v.y;

    return tmp;
}

struct vec v_diff(struct vec v1, struct vec v2) {
    struct vec tmp;

    tmp.x = v2.x - v1.x;
    tmp.y = v2.y - v1.y;

    return tmp;
}

struct vec scale(struct vec v, float sf) {
    struct vec tmp;

    tmp.x = v.x * sf;
    tmp.y = v.y * sf;

    return tmp;
}

// Implement after getting floating point vectors working....
struct vec norm(struct vec v) {
    struct vec tmp;

    tmp.x = ceil(v.x / vec_length(v));
    tmp.y = ceil(v.y / vec_length(v));

    // tmp.x = (int) tmp.x;
    // tmp.y = (int) tmp.y;

    return tmp;
}

// Transform function?

//////////////////////////////////////////////////////////////
//
// Graphics
//
//////////////////////////////////////////////////////////////

#define HEIGHT 40
#define WIDTH 64
#define FPS 20


// returns cursor to starting position
static void back(void)
{
    printf("\x1b[%dD", WIDTH);
    printf("\x1b[%dA", HEIGHT);
}

// draws picture with character at position
void draw(struct vec pos, int last1, int last2, int height, struct vec guy1, struct vec guy2) {

    char row[WIDTH];

    int posx = (int)pos.x;
    int posy = (int)pos.y;

    int scalex = WIDTH/8;
    int quit = 0;
    for(int y=0; y<HEIGHT; ++y) {
        for(int x=0; x<WIDTH; ++x) {
            if(x == 0 || y == 0 || x == WIDTH - 1 || y == HEIGHT -1) {
                row[x] = '#';
            }
            else if(y==HEIGHT/2) {
                row[x] = '=';
            }
            else if ((x == WIDTH/2 - 1 || x == WIDTH/2) && (y > HEIGHT/2 - HEIGHT/4 && y < HEIGHT/2 + HEIGHT/4)) {
                row[x] = '|';
            }
            else if (x == scalex || x == WIDTH - scalex -1) {
                row[x] = '|';
            }
            else if ((y == HEIGHT/2 - HEIGHT/4 || y == HEIGHT/2 + HEIGHT/4) && (x < WIDTH - scalex -1 && x > scalex)) {
                row[x] = '-';
            }
            else {
                row[x] = ' ';
            }
            if(posx == x && posy == y) {
                if(posx < scalex || posx > WIDTH-scalex-1) {
                    row[x] = 'X';
                }
                else {
                    if(height == 0) {
                        row[x] = '.';
                    }
                    else if (height == 1) {
                        row[x] = '*';
                    }
                    else {
                        row[x] = '@';
                    }
                }
            }
            if(guy1.x == x && guy1.y == y) {
                if(last1){
                    row[x] = '\\';
                }
                else {
                    row[x] = '/';
                }
            }
            if(guy2.x == x && guy2.y == y) {
                if(last2) {
                    row[x] = '/';
                }
                else {
                    row[x] = '\\';
                }
            }
        }
        fwrite(row, WIDTH, 1, stdout);
        fputc('\n', stdout);
        
    }

}

// Ideas:
//  Add DEPTH to ball. Increase in ASCII from low light to high light . -> @
//  Movable characters
//  Extend graphics terminal to the right for text ourputs 

// TODO
//  Make draw() args into a struct 
//  Implement floating point vectors
int main(int argc, char** argv) {

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
        draw(pos, last1, last2, height, guy1, guy2);
        back();
        usleep(1000 * 1000 / FPS);
    }
    return 0;
}