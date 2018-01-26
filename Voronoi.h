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
    double rectangleMinX;
    double rectangleMaxX;
    double rectangleMinY;
    double rectangleMaxY;
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
    void searchForParabola();
    void insertParabola(Parabola *parabola);
    void checkRightEvent(Edge *newRightEdge, Edge *rightEdge);
    void checkLeftEvent(Edge *leftEdge, Edge *newLeftEdge);
    void insertCrossingParabola(Parabola *leftParabola, Parabola *rightParabola, double y0);

public:
    void calculateBrute();
    void calculateFortune();
    void loadLimits(double minX, double maxX, double minY, double maxY);
    void loadPoints(std::vector<Point> points);
    void generatePoints(int howMany);
};


#endif //LEC_VORONOI_H
