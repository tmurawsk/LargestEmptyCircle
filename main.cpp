#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include "structs.h"

using namespace std;

bool isGreaterThan(Point p1, Point p2){
    return p1.y > p2.y;
}

double distance(Point p1, Point p2){
    double a = p2.x - p1.x;
    double b = p2.y - p1.y;
    return sqrt(a * a  + b * b);
}


int main() {
    cout << "Hello, World!" << endl;
    fstream file;
    file.open("../points.txt", ios::in);

    if(!file.good()){
        cerr << "ERROR READING FILE points.txt !" << endl;
        return -1;
    }

    int size;
    double maxY = 50, minY = -50, maxX = 50, minX = -50;

    file >> size;
    Point points[size];

    int i = 0;
    while(!file.eof() && i < size){ //reading points
        double x, y;
        file >> x >> y;
        points[i].x = x;
        points[i++].y = y;
    }

    sort(points, points + size, isGreaterThan);

    for(int i = 0; i < size; i++)
        cout << "( " << points[i].x << ", " << points[i].y << " )" << endl;

    int pointIter = 0;
    double sweepY = maxY;
    Edge *rootEdge = nullptr;   //root in beachline binary tree
    Event *firstEvent = nullptr;//first circle event in a queue


    while(true){     //main loop
        if(pointIter == size && firstEvent == nullptr)
            break;

        if(firstEvent == nullptr){  //site event
            if(rootEdge == nullptr){    //first loop
                sweepY = points[pointIter].x;
                rootEdge = new Edge();
                rootEdge->p = points[pointIter++];
                rootEdge->left = nullptr;
                rootEdge->right = nullptr;
            }
            else{   //not first loop
                //znajdujemy pod ktora parabola jest badany punkt (iteracja od rootEdge, wyliczanie a,b,c)
            }
        }
    }





    file.close();

    return 0;
}