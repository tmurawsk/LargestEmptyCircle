#ifndef LEC_VORONOI_H
#define LEC_VORONOI_H

#include <vector>
#include "Parabola.h"
#include "Event.h"

class Voronoi {
public:
    Parabola *firstParabola;    //beginning of the beach line
    Event *firstEvent;          //first event in the queue
    std::vector<Point> points;  //vector (sorted, so technically queue) of given points
    int pointIter;              //iterator for the vector of points
    double rectangleMinX;       //limits of the given rectangle
    double rectangleMaxX;
    double rectangleMinY;
    double rectangleMaxY;
    double maxRadius;           //radius of the biggest found empty circle
    double maxCircleX;          //its X coordinate
    double maxCircleY;          //its Y coordinate
    double sweepY;              //Y coordinate of the sweep line

    Voronoi();
    ~Voronoi();

private:
    void circleEvent(); //function handling circle events
    void siteEvent();   //function handling site events
    void calculateEdgeCrossing(Edge *leftEdge, Edge *middleEdge, Edge *rightEdge); //function checking for the circle events right after one of the circle events
    void searchForParabola(); //function searching for the parabola right over the point from the site event
    void insertParabola(Parabola *parabola); //inserting parabola into beach line
    void checkRightEvent(Edge *newRightEdge, Edge *rightEdge); //checking edge to the right of the actual edge for the circle event
    void checkLeftEvent(Edge *leftEdge, Edge *newLeftEdge); //checking left edge for circle event
    void insertCrossingParabola(Parabola *leftParabola, Parabola *rightParabola, double y0); //function handling situation when point from site event hits directly into the crossing point of parabolas

public:
    void calculateBrute(); //function implementing brute-force algorithm
    void calculateFortune(); //function implementing Fortune's algorithm
    void loadLimits(double minX, double maxX, double minY, double maxY); //setting rectangle coordinates
    void loadPoints(std::vector<Point> points); //setting points vector
    void generatePoints(int howMany); //generating random points
};


#endif //LEC_VORONOI_H
