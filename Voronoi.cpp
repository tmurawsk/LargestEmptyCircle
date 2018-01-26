#include "Voronoi.h"
#include <iostream>
#include <algorithm>
#include <ctime>

using namespace std;

Voronoi::Voronoi() {
    firstEvent = nullptr;
    firstParabola = nullptr;
    pointIter = 0;
    maxRadius = maxCircleX = maxCircleY = 0;
}

Voronoi::~Voronoi() {
    while (firstParabola != nullptr && firstParabola->right != nullptr) {
        Parabola *tempP = firstParabola;
        Edge *tempE = firstParabola->right;
        firstParabola = firstParabola->right->right;

        delete tempE;
        delete tempP;
    }
    delete firstParabola;

    while (firstEvent != nullptr) {
        Event *temp = firstEvent;
        firstEvent = firstEvent->next;
        temp->prev = nullptr;
        temp->next = nullptr;
        delete temp;
    }
}

void Voronoi::loadPoints(std::vector<Point> points) {
    this->points = points;
}

void Voronoi::loadLimits(double minX, double maxX, double minY, double maxY) {
    rectangleMinX = minX;
    rectangleMaxX = maxX;
    rectangleMinY = minY;
    rectangleMaxY = maxY;
}

void Voronoi::generatePoints(int howMany) {
    srand(time(NULL));

    double x, y;
    for (int i = 0; i < howMany; i++) {
        x = (double) rand() / RAND_MAX;
        x = rectangleMinX + x * (rectangleMaxX - rectangleMinX);
        y = (double) rand() / RAND_MAX;
        y = rectangleMinY + y * (rectangleMaxY - rectangleMinY);

        points.push_back(Point(x, y));
    }
}

void Voronoi::calculateBrute() {
    //brute-force
    Point p1, p2, p3;
    double a1, a2, b1, b2, x1, y1, radius;
    int size = points.size();
    sort(points.begin(), points.end(), Point::isGreaterThan);

    maxRadius = maxCircleX = maxCircleY = 0;

    for (int i = 0; i < size; i++) {
        p1 = points[i];
        for (int j = i + 1; j < size; j++) {
            p2 = points[j];
            for (int k = j + 1; k < size; k++) {
                p3 = points[k];

                if ((p1.x == p2.x && p2.x == p3.x) || (p1.y == p2.y && p2.y == p3.y) ||
                        (p1.y/p1.x == p2.y/p2.x && p2.y/p2.x == p3.y/p3.x))
                    continue;

                if(p1.y == p2.y){
                    x1 = (p1.x + p2.x) / 2;
                    a2 = (p2.x - p3.x) / (p3.y - p2.y);
                    b2 = (p2.y + p3.y) / 2 - a2 * (p2.x + p3.x) / 2;
                    y1 = a2 * x1 + b2;
                }else if(p2.y == p3.y){
                    x1 = (p2.x + p3.x) / 2;
                    a1 = (p1.x - p2.x) / (p2.y - p1.y);
                    b1 = (p1.y + p2.y) / 2 - a1 * (p1.x + p2.x) / 2;
                    y1 = a1 * x1 + b1;
                }else {
                    a1 = (p1.x - p2.x) / (p2.y - p1.y);
                    b1 = (p1.y + p2.y) / 2 - a1 * (p1.x + p2.x) / 2;
                    a2 = (p2.x - p3.x) / (p3.y - p2.y);
                    b2 = (p2.y + p3.y) / 2 - a2 * (p2.x + p3.x) / 2;

                    x1 = (b1 - b2) / (a2 - a1);
                    y1 = a1 * x1 + b1;
                }

                radius = Point::distance(Point(x1, y1), p1);

                if (radius > maxRadius)
                    if (x1 - radius >= rectangleMinX && x1 + radius <= rectangleMaxX
                        && y1 + radius <= rectangleMaxY && y1 - radius >= rectangleMinY) {
                        bool isEmpty = true;
                        for (int z = 0; z < size; z++) {
                            if (z == i || z == j || z == k)
                                continue;
                            double zx = points[z].x - x1;
                            double zy = points[z].y - y1;
                            if (zx * zx + zy * zy < radius * radius) {
                                isEmpty = false;
                                break;
                            }
                        }
                        if (isEmpty) {
                            maxRadius = radius;
                            maxCircleX = x1;
                            maxCircleY = y1;
                        }
                    }
            }
        }
    }
    cout << "Brutal: (" << maxCircleX << ", " << maxCircleY << "), R = " << maxRadius << endl;
}

void Voronoi::calculateFortune() {
    sort(points.begin(), points.end(), Point::isGreaterThan);

    for (int i = 0; i < points.size(); i++)
        cout << "( " << points[i].x << ", " << points[i].y << " )" << endl;

    sweepY = points[pointIter].x;
    firstParabola = new Parabola();
    firstParabola->p = points[pointIter++];
    firstParabola->left = nullptr;
    firstParabola->right = nullptr;

    while (1) {     //main loop
        if ((pointIter == points.size() && firstEvent == nullptr) || sweepY < rectangleMinY)
            break;

        if (pointIter < points.size() &&
            (firstEvent == nullptr || firstEvent->p.y < points[pointIter].y))   //site event
            siteEvent();
        else if (pointIter >= points.size() || firstEvent->p.y >= points[pointIter].y) { //circle event
            circleEvent();
        }
    }

    cout << "\nCircle: (" << maxCircleX << ", " << maxCircleY << "), R = " << maxRadius << endl;
}

void Voronoi::circleEvent() {
    sweepY = firstEvent->p.y;

    Edge *leftEdge = firstEvent->edge1;
    Edge *rightEdge = firstEvent->edge2;
    Parabola *temp1 = leftEdge->left;
    Parabola *temp2 = rightEdge->left;
    Parabola *temp3 = rightEdge->right;

    //entry situation: temp1 - leftEdge - temp2 - rightEdge - temp3
    //deleting temp2 and rightEdge, modifying leftEdge as a new edge
    //final situation: temp1 - leftEdge - temp3

    //calculating direction od the new edge:
    int direction;
    double a = (temp1->p.y - temp3->p.y) / (temp1->p.x - temp3->p.x);
    double b = temp1->p.y - a * temp1->p.x;
    double yCrossing = a * temp2->p.x + b;
    double y2 = temp2->p.y;

    temp3->left = leftEdge;
    leftEdge->right = temp3;

    rightEdge->left = nullptr;
    rightEdge->right = nullptr;
    temp2->left = nullptr;
    temp2->right = nullptr;
    delete rightEdge;
    delete temp2;
    rightEdge = nullptr;
    temp2 = nullptr;

    double radius = firstEvent->radius;
    cout << "CIRCLE event: (" << firstEvent->p.x << ", " << firstEvent->p.y + radius << "), radius: " << radius
         << endl;

    if (radius > maxRadius)
        if (firstEvent->p.x - radius >= rectangleMinX && firstEvent->p.x + radius <= rectangleMaxX
            && firstEvent->p.y + 2 * radius <= rectangleMaxY && firstEvent->p.y >= rectangleMinY) {
            maxRadius = radius;
            maxCircleX = firstEvent->p.x;
            maxCircleY = firstEvent->p.y + radius;
        }

    //calculating new edge parameters
    double a0 = (temp1->p.x - temp3->p.x) / (temp3->p.y - temp1->p.y);
    double b0 = (temp1->p.y + temp3->p.y) / 2 - a0 * (temp1->p.x + temp3->p.x) / 2;

    if (a0 > 0) {
        if (y2 < yCrossing)
            direction = 1;
        else
            direction = -1;
    } else {
        if (y2 < yCrossing)
            direction = -1;
        else
            direction = 1;
    }

    leftEdge->a = a0;
    leftEdge->b = b0;
    leftEdge->p = Point(firstEvent->p.x, firstEvent->p.y + radius);
    leftEdge->xDirection = direction;
    leftEdge->event = nullptr;

    //removing just used circle event
    Event::removeEvent(&firstEvent, &firstEvent);

    //calculating new potential circle events
    Edge *middleEdge = leftEdge;
    rightEdge = temp3->right;
    leftEdge = temp1->left;

    //checking potential circle events on the both sides of the new Edge
    calculateEdgeCrossing(leftEdge, middleEdge, rightEdge);
}

void Voronoi::calculateEdgeCrossing(Edge *leftEdge, Edge *middleEdge, Edge *rightEdge) {

    if (leftEdge != nullptr) {
        double x1, y1;
        if(leftEdge->xDirection == 0){
            x1 = leftEdge->p.x;
            y1 = middleEdge->a * x1 + middleEdge->b;
        }
        else if(middleEdge->xDirection == 0){
            x1 = middleEdge->p.x;
            y1 = leftEdge->a * x1 + leftEdge->b;
        }else{
            x1 = (leftEdge->b - middleEdge->b) / (middleEdge->a - leftEdge->a);
            y1 = middleEdge->a * x1 + middleEdge->b;
        }


        Event *event = leftEdge->event;
        double radius = Point::distance(Point(x1, y1), leftEdge->right->p);

        if ((leftEdge->xDirection > 0 && x1 > leftEdge->p.x) ||
            (leftEdge->xDirection < 0 && x1 < leftEdge->p.x) ||
                leftEdge->xDirection == 0) {
            if ((y1 - radius) < sweepY && (y1 - radius) >= rectangleMinY) {
                if (middleEdge->event == nullptr || (middleEdge->event != nullptr &&
                                                     Point::distance(Point(middleEdge->event->p.x,
                                                                           middleEdge->event->p.y +
                                                                           middleEdge->event->radius), middleEdge->p) >
                                                     Point::distance(Point(x1, y1), middleEdge->p))) {
                    if (event == nullptr || (event != nullptr &&
                                             Point::distance(Point(event->p.x, event->p.y + event->radius),
                                                             leftEdge->p) >
                                             Point::distance(Point(x1, y1), leftEdge->p))) {

                        Event *oldEvent = middleEdge->event;

                        if (oldEvent != nullptr)
                            Event::removeEvent(&firstEvent, &oldEvent);
                        if (event != nullptr)
                            Event::removeEvent(&firstEvent, &event);

                        //creating and inserting new potential circle event into queue
                        Event *newEvent = new Event();
                        newEvent->radius = radius;
                        newEvent->p = Point(x1, y1 - radius);
                        newEvent->edge1 = leftEdge;
                        newEvent->edge2 = middleEdge;
                        middleEdge->event = newEvent;
                        leftEdge->event = newEvent;
                        Event::insertEvent(&firstEvent, &newEvent);
                    }
                }
            }
        }
    }
    if (rightEdge != nullptr) {
        double x1, y1;
        if(rightEdge->xDirection == 0){
            x1 = rightEdge->p.x;
            y1 = middleEdge->a * x1 + middleEdge->b;
        }
        else if(middleEdge->xDirection == 0){
            x1 = rightEdge->p.x;
            y1 = rightEdge->a * x1 + rightEdge->b;
        }else{
            x1 = (rightEdge->b - middleEdge->b) / (middleEdge->a - rightEdge->a);
            y1 = middleEdge->a * x1 + middleEdge->b;
        }

        Event *event = rightEdge->event;

        double radius = Point::distance(Point(x1, y1), middleEdge->right->p);

        if ((rightEdge->xDirection > 0 && x1 > rightEdge->p.x) ||
            (rightEdge->xDirection < 0 && x1 < rightEdge->p.x) ||
                rightEdge->xDirection == 0) {
            if ((y1 - radius) < sweepY && (y1 - radius) >= rectangleMinY) {
                if (middleEdge->event == nullptr || (middleEdge->event != nullptr &&
                                                     Point::distance(Point(middleEdge->event->p.x,
                                                                           middleEdge->event->p.y +
                                                                           middleEdge->event->radius), middleEdge->p) >
                                                     Point::distance(Point(x1, y1), middleEdge->p))) {
                    if (event == nullptr || (event != nullptr &&
                                             Point::distance(Point(event->p.x, event->p.y + event->radius),
                                                             rightEdge->p) >
                                             Point::distance(Point(x1, y1), rightEdge->p))) {

                        Event *oldEvent = middleEdge->event;

                        if (oldEvent != nullptr)
                            Event::removeEvent(&firstEvent, &oldEvent);
                        if (event != nullptr)
                            Event::removeEvent(&firstEvent, &event);

                        //creating and inserting new potential circle event into queue
                        event = new Event();
                        event->radius = radius;
                        event->p = Point(x1, y1 - radius);
                        event->edge1 = middleEdge;
                        event->edge2 = rightEdge;
                        middleEdge->event = event;
                        rightEdge->event = event;
                        Event::insertEvent(&firstEvent, &event);
                    }
                }
            }
        }
    }
}

void Voronoi::siteEvent() {
    sweepY = points[pointIter].y;
    cout << "SITE event : (" << points[pointIter].x << ", " << points[pointIter].y << ")" << endl;

    cout << "beachline: ";
    Parabola *par = firstParabola;
    while (par->right != nullptr) {
        cout << par->p.x << " | ";
        par = par->right->right;
    }
    cout << par->p.x << endl;

    if (firstParabola->right == nullptr) {  //mamy tylko jedna parabole
        if (firstParabola->p.y == points[pointIter].y) { //parabola o tym samym y
            Parabola *newParabola = new Parabola();
            Edge *newEdge = new Edge();

            cout << "jedna parabola, rownolegle" << endl;

            newEdge->p = Point((points[pointIter].x + firstParabola->p.x) / 2, rectangleMaxY);
            newEdge->xDirection = 0;
            newEdge->event = nullptr;
            newEdge->left = firstParabola;
            newEdge->right = newParabola;

            newParabola->p = points[pointIter];
            newParabola->left = newEdge;
            newParabola->right = nullptr;

            firstParabola->right = newEdge;
        } else { //parabola o roznym y
            Parabola *parabolaCopy = new Parabola();
            Parabola *newParabola = new Parabola();
            Edge *newEdgeLeft = new Edge();
            Edge *newEdgeRight = new Edge();
            cout << "jedna parabola" << endl;

            double a0, b0, y0, a1, b1, c1, k, p;

            a0 = (firstParabola->p.x - points[pointIter].x) / (points[pointIter].y - firstParabola->p.y);
            b0 = (firstParabola->p.y + points[pointIter].y) / 2 -
                 a0 * (firstParabola->p.x + points[pointIter].x) / 2;

            k = (firstParabola->p.y + sweepY) / 2;
            p = (firstParabola->p.y - sweepY) / 2;
            a1 = 1 / (4 * p);
            b1 = -(firstParabola->p.x / (2 * p));
            c1 = (firstParabola->p.x * firstParabola->p.x / (4 * p)) + k;

            y0 = a1 * points[pointIter].x * points[pointIter].x + b1 * points[pointIter].x + c1;

            newEdgeLeft->p = newEdgeRight->p = Point(points[pointIter].x, y0);
            newEdgeLeft->a = newEdgeRight->a = a0;
            newEdgeLeft->b = newEdgeRight->b = b0;
            newEdgeLeft->xDirection = -1;
            newEdgeLeft->event = nullptr;
            newEdgeLeft->left = firstParabola;
            newEdgeLeft->right = newParabola;
            newEdgeRight->xDirection = 1;
            newEdgeRight->event = nullptr;
            newEdgeRight->left = newParabola;
            newEdgeRight->right = parabolaCopy;

            parabolaCopy->p = firstParabola->p;
            parabolaCopy->left = newEdgeRight;
            parabolaCopy->right = nullptr;

            newParabola->p = points[pointIter];
            newParabola->left = newEdgeLeft;
            newParabola->right = newEdgeRight;

            firstParabola->right = newEdgeLeft;
        }
    } else if (firstParabola->right->right->right == nullptr) { //mamy tylko dwie parabole (o rownym y)
        Edge *middleEdge = firstParabola->right;
        Parabola *secondParabola = middleEdge->right;
        Parabola *newParabola = new Parabola();

        if (points[pointIter].y == secondParabola->p.y) { //trzeci punkt o tym samym y
            Edge *newEdge = new Edge();

            newEdge->p.x = (points[pointIter].x + secondParabola->p.x) / 2;
            newEdge->p.y = rectangleMaxY;
            newEdge->xDirection = 0;
            newEdge->event = nullptr;
            newEdge->left = secondParabola;
            newEdge->right = newParabola;

            newParabola->p = points[pointIter];
            newParabola->left = newEdge;
            newParabola->right = nullptr;

            secondParabola->right = newEdge;
        } else { //trzeci punkt o roznym y
            Edge *newEdgeLeft = new Edge();
            Edge *newEdgeRight = new Edge();
            Parabola *parabolaCopy = new Parabola();
            Event *newEvent = new Event();
            Parabola *parabola;

            double a0, b0, y0, a1, b1, c1, k, p, xEvent, yEvent, radius;

            if (points[pointIter].x < middleEdge->p.x) { //lewa parabola
                parabola = firstParabola;
                newEdgeRight->event = newEvent;
            } else { //prawa parabola
                parabola = secondParabola;
                newEdgeLeft->event = newEvent;
            }
            middleEdge->event = newEvent;

            a0 = (parabola->p.x - points[pointIter].x) / (points[pointIter].y - parabola->p.y);
            b0 = (parabola->p.y + points[pointIter].y) / 2 -
                 a0 * (parabola->p.x + points[pointIter].x) / 2;

            k = (parabola->p.y + sweepY) / 2;
            p = (parabola->p.y - sweepY) / 2;
            a1 = 1 / (4 * p);
            b1 = -(parabola->p.x / (2 * p));
            c1 = (parabola->p.x * parabola->p.x / (4 * p)) + k;

            y0 = a1 * points[pointIter].x * points[pointIter].x + b1 * points[pointIter].x + c1;

            newEdgeLeft->p = newEdgeRight->p = Point(points[pointIter].x, y0);
            newEdgeLeft->a = newEdgeRight->a = a0;
            newEdgeLeft->b = newEdgeRight->b = b0;
            newEdgeLeft->xDirection = -1;
            newEdgeLeft->event = nullptr;
            newEdgeLeft->left = parabola;
            newEdgeLeft->right = newParabola;
            newEdgeRight->xDirection = 1;
            newEdgeRight->event = nullptr;
            newEdgeRight->left = newParabola;
            newEdgeRight->right = parabolaCopy;

            parabolaCopy->p = parabola->p;
            parabolaCopy->left = newEdgeRight;
            parabolaCopy->right = parabola->right;

            newParabola->p = points[pointIter];
            newParabola->left = newEdgeLeft;
            newParabola->right = newEdgeRight;

            parabola->right->left = parabolaCopy;
            parabola->right = newEdgeLeft;

            //dodajemy event
            xEvent = middleEdge->p.x;
            yEvent = a0 * xEvent + b0;

            radius = Point::distance(points[pointIter], Point(xEvent, yEvent));

            newEvent->p = Point(xEvent, yEvent - radius);
            newEvent->radius = radius;
            if(points[pointIter].x < middleEdge->p.x){
                newEvent->edge1 = newEdgeRight;
                newEvent->edge2 = middleEdge;
            }else{
                newEvent->edge1 = middleEdge;
                newEvent->edge2 = newEdgeLeft;
            }

            Event::insertEvent(&firstEvent, &newEvent);
        }
    } else { //mamy co najmniej trzy parabole
        searchForParabola();
    }
    pointIter++;

    cout << "beachline2: ";
    par = firstParabola;
    while (par->right != nullptr) {
        cout << par->p.x << " | ";
        par = par->right->right;
    }
    cout << par->p.x << endl;

}

void Voronoi::searchForParabola() {

    Parabola *temp1 = firstParabola;
    Edge *leftEdge = temp1->right;
    Parabola *temp2 = leftEdge->right;
    Edge *rightEdge = temp2->right;
    Parabola *temp3 = rightEdge->right;

    double a0, b0, a1, b1, c1, a2, b2, c2, a3, b3, c3, k, p, delta, x1, y1, x2, y2;

    while(1) {
        k = (temp2->p.y + sweepY) / 2;
        p = (temp2->p.y - sweepY) / 2;
        a2 = 1 / (4 * p);
        b2 = -(temp2->p.x / (2 * p));
        c2 = (temp2->p.x * temp2->p.x / (4 * p)) + k;

        delta = (b2 - leftEdge->a) * (b2 - leftEdge->a) - 4 * a2 * (c2 - leftEdge->b);
        if (leftEdge->xDirection < 0) //promien leci w lewo
            x1 = (leftEdge->a - b2 - sqrt(delta)) / (2 * a2); //punkt przeciecia na lewo
        else if (leftEdge->xDirection > 0)
            x1 = (leftEdge->a - b2 + sqrt(delta)) / (2 * a2); //punkt przeciecia na prawo
        else
            x1 = leftEdge->p.x; //jest pionowy

        delta = (b2 - rightEdge->a) * (b2 - rightEdge->a) - 4 * a2 * (c2 - rightEdge->b);
        if (rightEdge->xDirection < 0) //promien leci w lewo
            x2 = (rightEdge->a - b2 - sqrt(delta)) / (2 * a2); //punkt przeciecia na lewo
        else if (rightEdge->xDirection > 0)
            x2 = (rightEdge->a - b2 + sqrt(delta)) / (2 * a2); //punkt przeciecia na prawo
        else
            x2 = rightEdge->p.x; //jest pionowy

        //sprawdzamy czy trafilismy na odpowiednia parabole:
        if (points[pointIter].x > x1 && points[pointIter].x < x2) { //znalazlem parabole (temp2)

            insertParabola(temp2);
            break;

        } else if (points[pointIter].x < x1) { //znalazlem parabole (temp1)

            insertParabola(temp1);
            break;

        } else if (temp3->right == nullptr && points[pointIter].x > x2 && temp3->p.y > sweepY) { //znalazlem parabole (temp3)

            insertParabola(temp3);
            break;

        }else if (temp3->right == nullptr && temp3->p.y == sweepY) { //kolejna parabola o tym samym y
            Parabola *newParabola = new Parabola();
            Edge *newEdge = new Edge();

            newParabola->p = points[pointIter];
            newParabola->left = newEdge;
            newParabola->right = nullptr;

            newEdge->p.x = (temp3->p.x + points[pointIter].x) / 2;
            newEdge->p.y = rectangleMaxY;
            newEdge->xDirection = 0;
            newEdge->event = nullptr;
            newEdge->left = temp3;
            newEdge->right = newParabola;

            newParabola->right = newEdge;

        }else if(points[pointIter].x == x1 || points[pointIter].x == x2) { //punkt trafil dokladnie w przeciecie parabol
            if(points[pointIter].x == x1)
                insertCrossingParabola(temp1, temp2, a2 * x1 * x1 + b2 * x1 + c2);
            else
                insertCrossingParabola(temp2, temp3, a2 * x2 * x2 + b2 * x2 + c2);
        }

        if (temp3->right == nullptr) //jesli koniec
            break;
        else if (temp3->right != nullptr) {
            if (temp3->right->right->right != nullptr) //sa dwa wolne
                temp1 = temp3;
            else //jest tylko jedno wolne
                temp1 = temp2;
            leftEdge = temp1->right;
            temp2 = leftEdge->right;
            rightEdge = temp2->right;
            temp3 = rightEdge->right;
        }
    }
}

void Voronoi::insertCrossingParabola(Parabola *leftParabola, Parabola *rightParabola, double y0) {
    Parabola *newParabola = new Parabola();
    Edge *newLeftEdge = new Edge();
    Edge *newRightEdge = leftParabola->right;

    Event *oldEvent = newRightEdge->event;
    if(oldEvent != nullptr)
        Event::removeEvent(&firstEvent, &oldEvent);

    newParabola->left = newLeftEdge;
    newParabola->right = newRightEdge;
    newParabola->p = points[pointIter];

    double a, b;
    a = (leftParabola->p.x - newParabola->p.x) / (newParabola->p.y - leftParabola->p.y);
    b = (leftParabola->p.y + newParabola->p.y) / 2 - a * (leftParabola->p.x + newParabola->p.x) / 2;

    newLeftEdge->left = newRightEdge->left;
    newLeftEdge->right = newParabola;
    newLeftEdge->event = nullptr;
    newLeftEdge->p = Point(points[pointIter].x, y0);
    newLeftEdge->xDirection = -1;
    newLeftEdge->a = a;
    newLeftEdge->b = b;

    a = (rightParabola->p.x - newParabola->p.x) / (newParabola->p.y - rightParabola->p.y);
    b = (rightParabola->p.y + newParabola->p.y) / 2 - a * (rightParabola->p.x + newParabola->p.x) / 2;

    newRightEdge->left->right = newLeftEdge;
    newRightEdge->left = newParabola;
    newRightEdge->event = nullptr;
    newRightEdge->p = Point(points[pointIter].x, y0);
    newRightEdge->xDirection = 1;
    newRightEdge->a = a;
    newRightEdge->b = b;

    //sprawdzenie eventu po prawej stronie:
    //przeciecie sasiednich promieni:
    if(rightParabola->right != nullptr)
        checkRightEvent(newRightEdge, rightParabola->right);

    //sprawdzenie eventu po lewej stronie:
    //przeciecie sasiednich promieni:
    if(leftParabola->left != nullptr)
        checkLeftEvent(leftParabola->left, newLeftEdge);
}

void Voronoi::insertParabola(Parabola *parabola) {
    Parabola *newParabola = new Parabola();
    Parabola *copyParabola = new Parabola;
    Edge *newLeftEdge = new Edge();
    Edge *newRightEdge = new Edge();

    cout << "ZNALAZLEM PARABOLE" << endl;

    copyParabola->p = parabola->p;
    copyParabola->right = parabola->right;
    copyParabola->left = newRightEdge;

    newParabola->p = points[pointIter];
    newParabola->left = newLeftEdge;
    newParabola->right = newRightEdge;

    if(parabola->right != nullptr)
        parabola->right->left = copyParabola;
    parabola->right = newLeftEdge;


    double a0 = (parabola->p.x - points[pointIter].x) / (points[pointIter].y - parabola->p.y);
    double b0 = (parabola->p.y + points[pointIter].y) / 2 - a0 * (parabola->p.x + points[pointIter].x) / 2;

    double y1 = a0 * points[pointIter].x + b0;

    newLeftEdge->p = newRightEdge->p = Point(points[pointIter].x, y1);
    newLeftEdge->a = newRightEdge->a = a0;
    newLeftEdge->b = newRightEdge->b = b0;
    newLeftEdge->xDirection = -1;
    newLeftEdge->event = nullptr;
    newLeftEdge->left = parabola;
    newLeftEdge->right = newParabola;
    newRightEdge->xDirection = 1;
    newRightEdge->event = nullptr;
    newRightEdge->left = newParabola;
    newRightEdge->right = copyParabola;

    //sprawdzenie eventu po prawej stronie:
    //przeciecie sasiednich promieni:
    if(copyParabola->right != nullptr)
        checkRightEvent(newRightEdge, copyParabola->right);

    //sprawdzenie eventu po lewej stronie:
    //przeciecie sasiednich promieni:
    if(parabola->left != nullptr)
        checkLeftEvent(parabola->left, newLeftEdge);
}

void Voronoi::checkRightEvent(Edge *newRightEdge, Edge *rightEdge) {

    if(newRightEdge->xDirection == 0 && rightEdge->xDirection == 0)
        return;

    if(newRightEdge->a == rightEdge->a)
        return;

    double x1;
    if(rightEdge->xDirection == 0)
        x1 = rightEdge->p.x;
    else
        x1 = (newRightEdge->b - rightEdge->b) / (rightEdge->a - newRightEdge->a);
    double y1 = newRightEdge->a * x1 + newRightEdge->b;

    Event *event = rightEdge->event;

    double radius = Point::distance(Point(x1, y1), points[pointIter]);

    if ((rightEdge->xDirection > 0 && x1 > rightEdge->p.x) ||
        (rightEdge->xDirection < 0 && x1 < rightEdge->p.x) ||
            rightEdge->xDirection == 0) {
        if ((y1 - radius) < sweepY && (y1 - radius) >= rectangleMinY) { //przypisujemy swoj event
            if (event == nullptr ||
                (event != nullptr && Point::distance(Point(event->p.x, event->p.y + event->radius), points[pointIter]) <
                                     event->radius)) {

                if (event != nullptr)
                    Event::removeEvent(&firstEvent, &event);

                Event *newEvent = new Event();
                newEvent->radius = radius;
                newEvent->p = Point(x1, y1 - radius);
                newEvent->edge1 = newRightEdge;
                newEvent->edge2 = rightEdge;
                newRightEdge->event = newEvent;
                rightEdge->event = newEvent;
                Event::insertEvent(&firstEvent, &newEvent);
            }
        }
    }
}

void Voronoi::checkLeftEvent(Edge *leftEdge, Edge *newLeftEdge) {

    if(leftEdge->xDirection == 0 && newLeftEdge->xDirection == 0)
        return;

    if(leftEdge->a == newLeftEdge->a)
        return;

    double x1;
    if(leftEdge->xDirection == 0)
        x1 = leftEdge->p.x;
    else
        x1 = (leftEdge->b - newLeftEdge->b) / (newLeftEdge->a - leftEdge->a);
    double y1 = newLeftEdge->a * x1 + newLeftEdge->b;

    Event *event = leftEdge->event;

    double radius = Point::distance(Point(x1, y1), points[pointIter]);

    if ((leftEdge->xDirection > 0 && x1 > leftEdge->p.x) ||
        (leftEdge->xDirection < 0 && x1 < leftEdge->p.x) ||
            leftEdge->xDirection == 0) {
        if ((y1 - radius) < sweepY && (y1 - radius) >= rectangleMinY) { //przypisujemy swoj event
            if (event == nullptr || (event != nullptr && Point::distance(Point(event->p.x, event->p.y + event->radius), points[pointIter]) <
                event->radius)) {

                if(event != nullptr)
                    Event::removeEvent(&firstEvent, &event);

                Event *newEvent = new Event();
                newEvent->radius = radius;
                newEvent->p = Point(x1, y1 - radius);
                newEvent->edge1 = leftEdge;
                newEvent->edge2 = newLeftEdge;
                newLeftEdge->event = newEvent;
                leftEdge->event = newEvent;
                Event::insertEvent(&firstEvent, &newEvent);
            }
        }
    }
}
