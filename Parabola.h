#ifndef LEC_PARABOLA_H
#define LEC_PARABOLA_H

#include "Point.h"
struct Edge;

typedef struct Parabola {
    Point p;
    Edge *right;
    Edge *left;
} Parabola;


#endif //LEC_PARABOLA_H
