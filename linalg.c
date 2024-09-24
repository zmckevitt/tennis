#include "linalg.h"

// Do we need a constructor?
Vec init_vec(float x, float y) {
    Vec v;
    v.x = x;
    v.y = y;

    return v;
}

Vec vec_sum(Vec v1, Vec v2) {
    Vec v;

    v.x = v1.x + v2.x;
    v.y = v1.y + v2.y;

    return v;
}

double vec_length(Vec v) {
    return sqrt(v.x*v.x + v.y*v.y);
}

Vec flip_x(Vec v) {
    Vec tmp;

    tmp.x = -1 * v.x;
    tmp.y = v.y;

    return tmp;
}

Vec flip_y(Vec v) {
    Vec tmp;

    tmp.x = v.x;
    tmp.y = -1 * v.y;

    return tmp;
}

Vec flip(Vec v) {
    Vec tmp;

    tmp.x = -1 * v.x;
    tmp.y = -1 * v.y;

    return tmp;
}

Vec v_diff(Vec v1, Vec v2) {
    Vec tmp;

    tmp.x = v2.x - v1.x;
    tmp.y = v2.y - v1.y;

    return tmp;
}

Vec scale(Vec v, float sf) {
    Vec tmp;

    tmp.x = v.x * sf;
    tmp.y = v.y * sf;

    return tmp;
}

// Implement after getting floating point vectors working....
Vec norm(Vec v) {
    Vec tmp;

    tmp.x = ceil(v.x / vec_length(v));
    tmp.y = ceil(v.y / vec_length(v));

    // tmp.x = (int) tmp.x;
    // tmp.y = (int) tmp.y;

    return tmp;
}
