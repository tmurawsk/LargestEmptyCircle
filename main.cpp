#include <iostream>
#include <fstream>
#include "Voronoi.h"

using namespace std;

vector<Point> readPoints(string filename){
    fstream file;
    file.open(filename, ios::in);

    if (!file.good()) {
        cerr << "ERROR READING FILE " << filename << " !" << endl;
        exit(-1);
    }

    int size;
    file >> size;
    vector<Point> points;

    int i = 0;
    while (!file.eof() && i < size) { //reading points
        double x, y;
        file >> x >> y;
        points.push_back(Point(x, y));
    }
    file.close();

    return points;
}

int main(int argc, char **argv) {

//    if(argc < 2 || atoi(argv[1]) < 1 || atoi(argv[1]) > 3){
//        cout << "NIEPRAWIDLOWA KOMENDA!\nPoprawna forma:" << endl;
//        cout << "./largestEmptyCircle [tryb] [lista_parametrow]" << endl;
//        cout << "Wiecej informacji w pliku readme.txt" << endl;
//
//        return 0;
//    }

//    int mode = atoi(argv[1]);
    int mode = 2;

    Voronoi *voronoi = new Voronoi();

    switch(mode){
        case 2:
            vector<Point> points = readPoints("../pointsTest.txt");
            voronoi->loadPoints(points);
            voronoi->loadLimits(-50, 50, -50, 50);
            voronoi->calculateFortune();
            voronoi->calculateBrute();
            break;
    }

//    voronoi->generatePoints(howMany, -50, 50, -50, 50);
//    voronoi->calculateFortune();
//    voronoi->calculateBrute();

    delete voronoi;

    return 0;
}