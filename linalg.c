#include "linalg.h"

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
