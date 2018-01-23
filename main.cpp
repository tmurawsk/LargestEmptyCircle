#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include "structs.h"

using namespace std;

bool isGreaterThan(Point p1, Point p2) {
    if (p1.y == p2.y)
        return p1.x < p2.x;

    return p1.y > p2.y;
}

double distance(Point p1, Point p2) {
    double a = p2.x - p1.x;
    double b = p2.y - p1.y;
    return sqrt(a * a + b * b);
}

void insertEvent(Event **beg, Event **act) {
    if (*beg == nullptr) {
        *beg = *act;
        return;
    }

    Event *first = *beg;
    while (1) {
        if (first->p.y < (*act)->p.y) {
            if (first->prev == nullptr) {
                first->prev = *act;
                (*act)->next = first;
                (*act)->prev = nullptr;
                *beg = *act;
            } else {
                (*act)->prev = first->prev;
                (*act)->next = first;
                first->prev->next = *act;
                first->prev = *act;
            }
            break;
        }

        if (first->next == nullptr) {
            first->next = *act;
            (*act)->prev = first;
            (*act)->next = nullptr;
            break;
        }
        first = first->next;
    }
}

void generatePoints(int howMany, double minX, double maxX, double minY, double maxY) {
    srand(time(NULL));
    fstream file;
    file.open("../pointsRandom.txt", ios::out);

    if (!file.good()) {
        cerr << "ERROR WRITING FILE pointsRandom.txt" << endl;
        exit(-1);
    }

    double d;
    file << howMany << endl;
    for (int i = 0; i < howMany; i++) {
        d = (double) rand() / RAND_MAX;
        file << minX + d * (maxX - minX) << " ";
        d = (double) rand() / RAND_MAX;
        file << minY + d * (maxY - minY) << endl;
    }

    file.close();
}


int main() {
    cout << "Hello, World!" << endl;
    int size;
    double rectangleMaxY = 50, rectangleMinY = -50, rectangleMaxX = 50, rectangleMinX = -50;
    double maxRadius = 0, maxCircleX = 0, maxCircleY = 0;

    //generatePoints(6, rectangleMinX, rectangleMaxX, rectangleMinY, rectangleMaxY);

    fstream file;
    file.open("../points.txt", ios::in);

    if (!file.good()) {
        cerr << "ERROR READING FILE !" << endl;
        return -1;
    }

    file >> size;
    Point points[size];

    int i = 0;
    while (!file.eof() && i < size) { //reading points
        double x, y;
        file >> x >> y;
        points[i].x = x;
        points[i++].y = y;
    }
    file.close();

    sort(points, points + size, isGreaterThan);

    for (int i = 0; i < size; i++)
        cout << "( " << points[i].x << ", " << points[i].y << " )" << endl;

    int pointIter = 0;
    double sweepY = rectangleMaxY;
    Parabola *firstParabola = nullptr;   //first parabola in beachline
    Event *firstEvent = nullptr;    //first circle event in a queue

    sweepY = points[pointIter].x;
    firstParabola = new Parabola();
    firstParabola->p = points[pointIter++];
    firstParabola->event = nullptr;
    firstParabola->left = nullptr;
    firstParabola->right = nullptr;

    while (1) {     //main loop
        if (pointIter == size && firstEvent == nullptr)
            break;

        if (pointIter < size && (firstEvent == nullptr || firstEvent->p.y < points[pointIter].y)) {  //site event
            sweepY = points[pointIter].y;
            cout << "SITE event : (" << points[pointIter].x << ", " << points[pointIter].y << ")" << endl;

            cout << "beachline: ";
            Parabola *par = firstParabola;
            while (par->right != nullptr) {
                cout << par->p.x << " | ";
                par = par->right->right;
            }
            cout << endl;

            if (firstParabola->right == nullptr) {  //there is only one parabola in the list
                if (firstParabola->p.y == points[pointIter].y) { //parabola o tym samym y
                    Parabola *newParabola = new Parabola();
                    Edge *newEdge = new Edge();

                    cout << "jedna parabola, rownolegle" << endl;

                    newEdge->p = Point((points[pointIter].x + firstParabola->p.x) / 2, rectangleMaxY);
                    newEdge->xDirection = 0;
                    newEdge->left = firstParabola;
                    newEdge->right = newParabola;

                    newParabola->p = points[pointIter];
                    newParabola->event = nullptr;
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
                    newEdgeRight->xDirection = 1;
                    newEdgeLeft->left = firstParabola;
                    newEdgeLeft->right = newParabola;
                    newEdgeRight->left = newParabola;
                    newEdgeRight->right = parabolaCopy;

                    parabolaCopy->p = firstParabola->p;
                    parabolaCopy->event = nullptr;
                    parabolaCopy->left = newEdgeRight;
                    parabolaCopy->right = nullptr;

                    newParabola->p = points[pointIter];
                    newParabola->event = nullptr;
                    newParabola->left = newEdgeLeft;
                    newParabola->right = newEdgeRight;

                    firstParabola->right = newEdgeLeft;
                }
            } else { //szukamy paraboli

            }
            pointIter++;
        }
    }


    while (firstParabola->right != nullptr) {
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

    //brute-force
    Point p1, p2, p3;
    double a1, a2, b1, b2, x1, y1, radius;
    maxRadius = 0;
    for (int i = 0; i < size; i++) {
        p1 = points[i];
        for (int j = i + 1; j < size; j++) {
            p2 = points[j];
            for (int k = j + 1; k < size; k++) {
                p3 = points[k];

                if ((p1.x == p2.x && p2.x == p3.x) || (p1.y == p2.y && p2.y == p3.y))
                    continue;

                a1 = (p1.x - p2.x) / (p2.y - p1.y);
                b1 = (p1.y + p2.y) / 2 - a1 * (p1.x + p2.x) / 2;
                a2 = (p2.x - p3.x) / (p3.y - p2.y);
                b2 = (p2.y + p3.y) / 2 - a2 * (p2.x + p3.x) / 2;

                x1 = (b1 - b2) / (a2 - a1);
                y1 = a1 * x1 + b1;

                radius = distance(Point(x1, y1), p1);

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

    return 0;
}