#include <iostream>
#include "Voronoi.h"

using namespace std;


int main() {
    cout << "Hello, World!" << endl;

//    fstream file;
//    file.open("../pointsTest.txt", ios::in);
//
//    if (!file.good()) {
//        cerr << "ERROR READING FILE !" << endl;
//        return -1;
//    }
//
//    file >> size;
//    Point points[size];
//
//    int i = 0;
//    while (!file.eof() && i < size) { //reading points
//        double x, y;
//        file >> x >> y;
//        points[i].x = x;
//        points[i++].y = y;
//    }
//    file.close();

    Voronoi *voronoi = new Voronoi();

    voronoi->calculateFortune(50);
    voronoi->calculateBrute();

    delete voronoi;

    return 0;
}