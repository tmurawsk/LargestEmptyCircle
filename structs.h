//
// Created by tomson on 15.01.18.
//

#ifndef LEC_STRUCTS_H
#define LEC_STRUCTS_H

typedef struct Point{
    double x;
    double y;

    Point(double a, double b){
        x = a;
        y = b;
    }
};

typedef struct Edge{
    double a;       //a parameter of the ray
    double b;       //b parameter of the ray
    Point p;        //starting point of th ray
    bool xDirection;//1 -> x+; 0 -> x-
    Edge *left;
    Edge *right;
    Edge *parent;
};

typedef struct Event{
    double y;
    Edge *edge1;
    Edge *edge2;
    Event *next;
    Event *prev;
};



#endif //LEC_STRUCTS_H
