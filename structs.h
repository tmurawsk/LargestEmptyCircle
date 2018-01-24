//
// Created by tomson on 15.01.18.
//

#ifndef LEC_STRUCTS_H
#define LEC_STRUCTS_H

typedef struct Point{
    double x;
    double y;

    Point() {};

    Point(double a, double b){
        x = a;
        y = b;
    }

} Point;

typedef struct Edge Edge;

typedef struct Parabola{
    Point p;
    Edge *right;
    Edge *left;
} Parabola;

typedef struct Event Event;

typedef struct Edge{
    double a;       //a parameter of the ray (or x if ray is vertical)
    double b;       //b parameter of the ray
    Point p;        //starting point of the ray
    short xDirection;//-1 -> x- ; 0 -> | ; 1 -> x+
    Event *event;
    Parabola *left;
    Parabola *right;
} Edge;

typedef struct Event{
    Point p;        //trigger point
    double radius;
    Edge *edge1;
    Edge *edge2;
//    Parabola *parabola;
    Event *next;
    Event *prev;
} Event;



#endif //LEC_STRUCTS_H
