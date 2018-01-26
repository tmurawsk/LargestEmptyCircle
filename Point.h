#ifndef LEC_POINT_H
#define LEC_POINT_H

#include <cmath>

typedef struct Point{
    double x;
    double y;

    Point() {};

    Point(double a, double b){
        x = a;
        y = b;
    }

    //function used to sort points with std::sort
    static bool isGreaterThan(Point p1, Point p2) {
        if (p1.y == p2.y)
            return p1.x < p2.x;

        return p1.y > p2.y;
    }

    //function returning distance between two given points
    static double distance(Point p1, Point p2) {
        double a = p2.x - p1.x;
        double b = p2.y - p1.y;
        return sqrt(a * a + b * b);
    }

} Point;

#endif //LEC_POINT_H
