#include "linalg.h"

// Do we need a constructor?
Vec init_vec(float x, float y, float z) {
    Vec v = {
        .x = x,
        .y = y,
        .z = z
    };
    return v;
}

Vec vec_sum(Vec v1, Vec v2) {
    Vec v;

    v.x = v1.x + v2.x;
    v.y = v1.y + v2.y;
    v.z = v1.z + v2.z;

    return v;
}

double vec_length(Vec v) {
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
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

Vec flip_xy(Vec v) {
    Vec tmp;

    tmp.x = -1 * v.x;
    tmp.y = -1 * v.y;

    return tmp;
}

Vec v_diff(Vec v1, Vec v2) {
    Vec tmp;

    tmp.x = v2.x - v1.x;
    tmp.y = v2.y - v1.y;
    tmp.z = v2.z - v1.z;

    return tmp;
}

Vec scale(Vec v, float sf) {
    Vec tmp;

    tmp.x = v.x * sf;
    tmp.y = v.y * sf;
    tmp.z = v.z * sf;

    return tmp;
}

// Implement after getting floating point vectors working....
Vec norm(Vec v) {
    Vec tmp;

    tmp.x = ceil(v.x / vec_length(v));
    tmp.y = ceil(v.y / vec_length(v));
    tmp.z = ceil(v.z / vec_length(v));

    // tmp.x = (int) tmp.x;
    // tmp.y = (int) tmp.y;

    return tmp;
}
