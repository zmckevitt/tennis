#ifndef LINALG_H
#define LINALG_H

#include <math.h>

typedef struct Vec {
    double x;
    double y;
    double z;
} Vec;

Vec init_vec(float x, float y, float z);

Vec vec_sum(Vec v1, Vec v2);

double vec_length(Vec v);

Vec flip_x(Vec v);

Vec flip_y(Vec v);

Vec flip_xy(Vec v);

Vec v_diff(Vec v1, Vec v2);

Vec scale(Vec v, float sf);

// Implement after getting floating point vectors working....
Vec norm(Vec v);

#endif // LINALG_H
