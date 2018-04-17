#include <iostream>
#include <fstream>
#include <chrono>
#include "Voronoi.h"

using namespace std;

vector<Point> readPoints(string filename);
void mode1(Voronoi *voronoi, char **argv);
void mode2(Voronoi *voronoi, char **argv);
void mode3(Voronoi *voronoi, char **argv);
void mode4(Voronoi *voronoi, char **argv);

int main(int argc, char **argv) {

    if(argc < 2 || atoi(argv[1]) < 1 || atoi(argv[1]) > 4){
        cout << "NIEPRAWIDLOWA KOMENDA!\nPoprawna forma:" << endl;
        cout << "./largestEmptyCircle [tryb] [lista_parametrow]" << endl;
        cout << "Wiecej informacji w pliku readme.txt" << endl;

        return 0;
    }

    int mode = atoi(argv[1]);

    Voronoi *voronoi = new Voronoi();

    switch(mode){
        case 1:     //points from .txt file
            if(argc < 7){
                cout << "BLEDNA LICZBA PARAMETROW!" << endl;
                return -2;
            }
            mode1(voronoi, argv);
            break;

        case 2:     //random-generated points
            if(argc < 7){
                cout << "BLEDNA LICZBA PARAMETROW!" << endl;
                return -2;
            }
            mode2(voronoi, argv);
            break;

        case 3:     //random-generated points with time measure for both algorithms
            if(argc < 10){
                cout << "BLEDNA LICZBA PARAMETROW!" << endl;
                return -2;
            }
            mode3(voronoi, argv);
            break;

        case 4:     //random-generated points with time measure, only for Fortune
            if(argc < 10){
                cout << "BLEDNA LICZBA PARAMETROW!" << endl;
                return -2;
            }
            mode4(voronoi, argv);
            break;
    }

    delete voronoi;

    return 0;
}

void mode1(Voronoi *voronoi, char **argv){
    vector<Point> points = readPoints(argv[6]);
    voronoi->loadPoints(points);
    voronoi->loadLimits(atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]));
    cout << "Wynik algorytmu Fortune'a:" << endl;
    voronoi->calculateFortune();
    cout << "( " << voronoi->maxCircleX << ", " << voronoi->maxCircleY << " ), R: " << voronoi->maxRadius << endl;
    cout << "Wynik algorytmu brute-force:" << endl;
    voronoi->calculateBrute();
    cout << "( " << voronoi->maxCircleX << ", " << voronoi->maxCircleY << " ), R: " << voronoi->maxRadius << endl;
}

void mode2(Voronoi *voronoi, char **argv){
    voronoi->loadLimits(atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]));
    voronoi->generatePoints(atoi(argv[6]));

    for(int i = 0; i < voronoi->points.size(); i++)
        cout << "( " << voronoi->points[i].x << ", " << voronoi->points[i].y << " )" << endl;

    cout << "Wynik algorytmu Fortune'a:" << endl;
    voronoi->calculateFortune();
    cout << "( " << voronoi->maxCircleX << ", " << voronoi->maxCircleY << " ), R: " << voronoi->maxRadius << endl;
    cout << "Wynik algorytmu brute-force:" << endl;
    voronoi->calculateBrute();
    cout << "( " << voronoi->maxCircleX << ", " << voronoi->maxCircleY << " ), R: " << voronoi->maxRadius << endl;
}

void mode3(Voronoi *voronoi, char **argv){
    voronoi->loadLimits(atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]));
    int nStart = atoi(argv[6]);
    int interval = atoi(argv[7]);
    int howManyTimes = atoi(argv[8]);
    int generation = atoi(argv[9]);

    fstream file;
    file.open("out.txt", ios::out);
    if(!file.good()){
        cout << "BLAD OTWARCIA PLIKU out.txt !" << endl;
        exit(-1);
    }

    file << "N\tFortune\t\tBrute-force" << endl;
    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = end - start;
    double durationAvgFortune;
    double durationAvgBrute;


    for(int i = 0; i < howManyTimes; i++){
        durationAvgFortune = 0;
        durationAvgBrute = 0;
        for(int j = 0; j < generation; j++){
            voronoi->generatePoints(nStart);

            start = std::chrono::system_clock::now();
            voronoi->calculateFortune();
            end = std::chrono::system_clock::now();

            duration = end - start;
//            file << nStart << "\t" << duration.count() << "\t";
            durationAvgFortune += duration.count();

            start = std::chrono::system_clock::now();
            voronoi->calculateBrute();
            end = std::chrono::system_clock::now();

            duration = end - start;
//            file << duration.count() << endl;
            durationAvgBrute += duration.count();

            voronoi->points.clear();
            delete voronoi;
            voronoi = new Voronoi();
            voronoi->loadLimits(atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]));

        }
        file << nStart << "\t" << durationAvgFortune/generation << "\t" << durationAvgBrute/generation << "\n";
        nStart += interval;
        file << endl;
    }

    file.close();

}

void mode4(Voronoi *voronoi, char **argv){
    voronoi->loadLimits(atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]));
    int nStart = atoi(argv[6]);
    int interval = atoi(argv[7]);
    int howManyTimes = atoi(argv[8]);
    int generation = atoi(argv[9]);

    fstream file;
    file.open("out.txt", ios::out);
    if(!file.good()){
        cout << "BLAD OTWARCIA PLIKU out.txt !" << endl;
        exit(-1);
    }

    file << "N\tFortune" << endl;
    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = end - start;
    double durationAvgFortune;
    double durationAvgBrute;


    for(int i = 0; i < howManyTimes; i++){
        durationAvgFortune = 0;
        durationAvgBrute = 0;
        for(int j = 0; j < generation; j++){
            voronoi->generatePoints(nStart);

            start = std::chrono::system_clock::now();
            voronoi->calculateFortune();
            end = std::chrono::system_clock::now();

            duration = end - start;
            //file << nStart << "\t" << duration.count() << "\n";
            durationAvgFortune += duration.count();

            start = std::chrono::system_clock::now();
            //voronoi->calculateBrute();
            end = std::chrono::system_clock::now();

            duration = end - start;
            //file << duration.count() << endl;
            durationAvgBrute += duration.count();

            voronoi->points.clear();
            delete voronoi;
            voronoi = new Voronoi();
            voronoi->loadLimits(atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]));

        }
        file << nStart << "\t" << durationAvgFortune/generation;// << "\t" << durationAvgBrute/generation << "\n";
                nStart += interval;
        file << endl;
    }

    file.close();

}

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