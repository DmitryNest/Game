#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

struct vector {
    int x;
    int y;
};

struct vector* create_vector(int x, int y)
{
    struct vector* v = (struct vector*)malloc(sizeof(struct vector));
    v->x = x;
    v->y = y;

    return v;
}

void destroy_vector(struct vector* v)
{
    free(v);
}

#endif
