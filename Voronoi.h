#ifndef LEC_VORONOI_H
#define LEC_VORONOI_H

#include <vector>
#include "Parabola.h"
#include "Event.h"

class Voronoi {
    Parabola *firstParabola;
    Event *firstEvent;
    std::vector<Point> points;
    int pointIter;
    int rectangleMinX;
    int rectangleMaxX;
    int rectangleMinY;
    int rectangleMaxY;
    double maxRadius;
    double maxCircleX;
    double maxCircleY;
    double sweepY;

public:
    Voronoi();
    ~Voronoi();

private:
    void circleEvent();
    void siteEvent();
    void calculateEdgeCrossing(Edge *leftEdge, Edge *middleEdge, Edge *rightEdge);
    void generatePoints(int howMany, double minX, double maxX, double minY, double maxY);
    void searchForParabola();
    void insertParabola(Parabola *parabola);
    void checkRightEvent(Edge *newRightEdge, Edge *rightEdge);
    void checkLeftEvent(Edge *leftEdge, Edge *newLeftEdge);

public:
    void calculateBrute();
    void calculateFortune(int howMany);
};


#endif //LEC_VORONOI_H
