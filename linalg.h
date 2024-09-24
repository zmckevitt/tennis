#ifndef LINALG_H
#define LINALG_H

#include <math.h>

struct vec {
    double x;
    double y;
};

// Do we need a constructor?
struct vec init_vec(float x, float y); 

struct vec vec_sum(struct vec v1, struct vec v2); 

double vec_length(struct vec v); 

struct vec flip_x(struct vec v); 

struct vec flip_y(struct vec v); 

struct vec flip(struct vec v);

struct vec v_diff(struct vec v1, struct vec v2); 

struct vec scale(struct vec v, float sf); 

// Implement after getting floating point vectors working....
struct vec norm(struct vec v);

// Transform function?

#endif // LINALG_H
