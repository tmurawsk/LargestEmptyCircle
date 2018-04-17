#ifndef LEC_EDGE_H
#define LEC_EDGE_H

#include "Point.h"
struct Event;
struct Parabola;

typedef struct Edge {
    double a;       //a parameter of the ray
    double b;       //b parameter of the ray
    Point p;        //starting point of the ray
    int xDirection; //-1 -> x- ; 0 -> | ; 1 -> x+
    Event *event;   //pointer to a circle event assigned to the edge
    Parabola *left;
    Parabola *right;
} Edge;


#endif //LEC_EDGE_H
