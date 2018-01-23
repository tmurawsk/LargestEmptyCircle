//
// Created by tomson on 15.01.18.
//

#ifndef LEC_STRUCTS_H
#define LEC_STRUCTS_H

typedef struct Point{
    double x;
    double y;

    Point(){}

    Point(double a, double b){
        x = a;
        y = b;
    }

} Point;

typedef struct Edge{
    double a;       //a parameter of the ray
    double b;       //b parameter of the ray
    Point p;        //starting point of the ray
    int xDirection;//-1 -> x- ; 0 -> | ; 1 -> x+
    Parabola *left;
    Parabola *right;
} Edge;

typedef struct Event Event;

typedef struct Parabola{
    Point p;
    Event *event;
    Edge *right;
    Edge *left;
} Parabola;

typedef struct Event{
    Point p;
    double radius;
//    Edge *edge1;
//    Edge *edge2;
    Parabola *parabola;
    Event *next;
    Event *prev;
} Event;



#endif //LEC_STRUCTS_H
