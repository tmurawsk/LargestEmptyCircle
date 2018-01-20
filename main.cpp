#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include "structs.h"

using namespace std;

bool isGreaterThan(Point p1, Point p2) {
    if(p1.y == p2.y)
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

void generatePoints(int howMany, double minX, double maxX, double minY, double maxY){
    srand(time(NULL));
    fstream file;
    file.open("../pointsRandom.txt", ios::out);

    if(!file.good()){
        cerr << "ERROR WRITING FILE pointsRandom.txt" << endl;
        exit(-1);
    }

    double d;
    file << howMany << endl;
    for(int i = 0; i < howMany; i++){
        d = (double)rand() / RAND_MAX;
        file << minX + d * (maxX - minX) << " ";
        d = (double)rand() / RAND_MAX;
        file << minY + d * (maxY - minY) << endl;
    }

    file.close();
}


int main() {
    cout << "Hello, World!" << endl;
    int size;
    double rectangleMaxY = 50, rectangleMinY = -50, rectangleMaxX = 50, rectangleMinX = -50;
    double maxRadius = 0, maxCircleX = 0, maxCircleY = 0;

    //generatePoints(15, rectangleMinX, rectangleMaxX, rectangleMinY, rectangleMaxY);

    fstream file;
    file.open("../pointsRandom.txt", ios::in);

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
            //znajdujemy pod ktora parabola jest badany punkt (iteracja od firstParabola, wyliczanie a,b,c)

            sweepY = points[pointIter].y;
            cout << "site event: (" << points[pointIter].x << ", " << points[pointIter].y << ")" << endl;
            cout << "beachline: ";
            Parabola *par = firstParabola;
            while(par != nullptr){
                cout << par->p.x << " | ";
                par = par->right;
            }
            cout << endl;

            if(points[pointIter].x == -8)
                cout << "";

            if (firstParabola->right == nullptr) {  //there is only one parabola in the list
                if(firstParabola->p.y == points[pointIter].y){ //parabola o tym samym y
                    Parabola *newParabola = new Parabola();

                    cout << "jedna parabola, rownolegle" << endl;
                    newParabola->p = points[pointIter];
                    newParabola->event = nullptr;
                    newParabola->left = firstParabola;
                    newParabola->right = nullptr;

                    firstParabola->right = newParabola;
                }
                else {
                    Parabola *parabolaCopy = new Parabola();
                    Parabola *newParabola = new Parabola();

                    cout << "jedna parabola" << endl;
                    parabolaCopy->p = firstParabola->p;
                    parabolaCopy->event = nullptr;
                    parabolaCopy->left = newParabola;
                    parabolaCopy->right = nullptr;

                    newParabola->p = points[pointIter];
                    newParabola->event = nullptr;
                    newParabola->left = firstParabola;
                    newParabola->right = parabolaCopy;

                    firstParabola->right = newParabola;
                }
            } else {   //wstawiamy w srodek - szukamy przecietej paraboli; obliczamy circle event i usuwamy aktualny
                Parabola *temp1 = firstParabola;
                Parabola *temp2 = temp1->right;
                Parabola *temp3 = temp2->right;
                double a1, b1, c1, a2, b2, c2, a3, b3, c3, k, p, x1, y1, x2, y2;

                cout << "wincyj paraboli" << endl;
                while (1) { //searching for the parabola
                    //badamy przeciecie paraboli temp1-temp2 i temp2-temp3 - moze miedzy nimi lezy nasz punkt
                    if (temp1 == firstParabola) { //beginning of the search
                        k = (temp1->p.y + sweepY) / 2;
                        p = (temp1->p.y - sweepY) / 2;
                        a1 = 1 / (4 * p);
                        b1 = -(temp1->p.x / (2 * p));
                        c1 = (temp1->p.x * temp1->p.x / (4 * p)) + k;

                        k = (temp2->p.y + sweepY) / 2;
                        p = (temp2->p.y - sweepY) / 2;
                        a2 = 1 / (4 * p);
                        b2 = -(temp2->p.x / (2 * p));
                        c2 = (temp2->p.x * temp2->p.x / (4 * p)) + k;

                        k = (temp3->p.y + sweepY) / 2;
                        p = (temp3->p.y - sweepY) / 2;
                        a3 = 1 / (4 * p);
                        b3 = -(temp3->p.x / (2 * p));
                        c3 = (temp3->p.x * temp3->p.x / (4 * p)) + k;

                        //obliczenie przeciecia parabol temp1 i temp2
                        if(temp1->p.y == sweepY)
                            x1 = temp1->p.x;
                        else if(temp2->p.y == sweepY)
                            x1 = temp2->p.x;
                        else {
                            k = (b2 - b1) * (b2 - b1) - 4 * (a2 - a1) * (c2 - c1);
                            x1 = (b1 - b2 - sqrt(k)) / (2 * (a2 - a1));
                        }

                        if(temp2->p.y == sweepY)
                            x2 = temp2->p.x;
                        else if(temp3->p.y == sweepY)
                            x2 = temp3->p.x;
                        else {
                            k = (b3 - b2) * (b3 - b2) - 4 * (a3 - a2) * (c3 - c2);
                            x2 = (b2 - b3 - sqrt(k)) / (2 * (a3 - a2));
                        }
                    } else { //at least second iteration
                        a1 = a2;
                        b1 = b2;
                        c1 = c2;
                        a2 = a3;
                        b2 = b3;
                        c2 = c3;
                        x1 = x2;

                        if(temp3->p.y == sweepY)
                            x2 = temp3->p.x;
                        else {
                            k = (temp3->p.y + sweepY) / 2;
                            p = (temp3->p.y - sweepY) / 2;
                            a3 = 1 / (4 * p);
                            b3 = -(temp3->p.x / (2 * p));
                            c3 = (temp3->p.x * temp3->p.x / (4 * p)) + k;

                            //obliczenie przeciecia parabol temp2 i temp3
                            k = (b3 - b2) * (b3 - b2) - 4 * (a3 - a2) * (c3 - c2);
                            x2 = (b2 - b3 - sqrt(k)) / (2 * (a3 - a2));
                        }
                    }
                    if(points[pointIter].x == 11 && temp1->p.x == -10)
                        cout << "";


                    if (points[pointIter].x >= x1 && points[pointIter].x <= x2) { //found the parabola
                        //bierzemy points->parabola, wstawiamy do listy nowy punkt, usuwamy obecny circle event, obliczamy circle event
                        Parabola *newParabola = new Parabola();
                        Parabola *copy = new Parabola();

                        copy->p = temp2->p;
                        copy->right = temp2->right;
                        copy->left = newParabola;

                        newParabola->p = points[pointIter];
                        newParabola->event = nullptr;
                        newParabola->left = temp2;
                        newParabola->right = copy;

                        temp2->right->left = copy;
                        temp2->right = newParabola;

                        cout << "ZNALAZLEM PARABOLE" << endl;

                        //usuwamy obecny circle event
                        Event *oldEvent = temp2->event;
                        if(oldEvent != nullptr) {
                            if (oldEvent->prev == nullptr && oldEvent->next != nullptr) { //first
                                firstEvent = oldEvent->next;
                                firstEvent->prev = nullptr;
                                oldEvent->next = nullptr;
                                oldEvent->parabola = nullptr;
                            } else if (oldEvent->next == nullptr && oldEvent->prev != nullptr) { //last
                                oldEvent->prev->next = nullptr;
                                oldEvent->prev = nullptr;
                                oldEvent->parabola = nullptr;
                            } else if (oldEvent->next == nullptr && oldEvent->prev == nullptr) { //lone
                                firstEvent = nullptr;
                                oldEvent->parabola = nullptr;
                            } else { //in the middle
                                oldEvent->next->prev = oldEvent->prev;
                                oldEvent->prev->next = oldEvent->next;
                                oldEvent->next = nullptr;
                                oldEvent->prev = nullptr;
                                oldEvent->parabola = nullptr;
                            }
                            delete oldEvent;
                            temp2->event = nullptr;
                        }

                        //obliczamy nowy circle event temp1-temp2-new-copy(temp2)-temp3
                        a2 = (points[pointIter].x - temp2->p.x) / (temp2->p.y - points[pointIter].y);
                        b2 = (points[pointIter].y + temp2->p.y) / 2 - a2 * (points[pointIter].x + temp2->p.x) / 2;
                        a1 = (temp1->p.x - temp2->p.x) / (temp2->p.y - temp1->p.y);
                        b1 = (temp1->p.y + temp2->p.y) / 2 - a1 * (temp1->p.x + temp2->p.x) / 2;
                        a3 = (temp2->p.x - temp3->p.x) / (temp3->p.y - temp2->p.y);
                        b3 = (temp2->p.y + temp3->p.y) / 2 - a3 * (temp2->p.x + temp3->p.x) / 2;

                        if(temp1->p.y == temp2->p.y) //jesli symetralna jest pionowa
                            x1 = (temp1->p.x + temp2->p.x) / 2;
                        else
                            x1 = (b1 - b2) / (a2 - a1);
                        y1 = a2 * x1 + b2; //crossing point on the left

                        if(temp2->p.y == temp3->p.y) //jesli symetralna jest pionowa
                            x2 = (temp2->p.x + temp3->p.x) / 2;
                        else
                            x2 = (b2 - b3) / (a3 - a2);
                        y2 = a2 * x2 + b2; //crossing point on the right

                        Event *newEvent1 = new Event();
                        Event *newEvent2 = new Event();

                        newEvent1->radius = distance(Point(x1, y1), temp2->p);
                        newEvent1->p.x = x1;
                        newEvent1->p.y = y1 - newEvent1->radius;
                        newEvent1->parabola = temp2;
                        newEvent2->radius = distance(Point(x2, y2), temp2->p);
                        newEvent2->p.x = x2;
                        newEvent2->p.y = y2 - newEvent2->radius;
                        newEvent2->parabola = copy;

                        if(temp1->p.x < temp2->p.x){
                            temp2->event = newEvent1;
                            if(newEvent1->p.y <= sweepY)
                                insertEvent(&firstEvent, &newEvent1);
                            else
                                delete newEvent1;
                        }
                        else
                            delete newEvent1;


                        if(temp2->p.x < temp3->p.x){
                            copy->event = newEvent2;
                            if(newEvent2->p.y <= sweepY) {
                                if (firstEvent == nullptr) {
                                    firstEvent = newEvent2;
                                    firstEvent->next = nullptr;
                                    firstEvent->prev = nullptr;
                                } else
                                    insertEvent(&firstEvent, &newEvent2);
                            }
                            else
                                delete newEvent2;
                        }
                        else
                            delete newEvent2;

//                        if(newEvent2->p.y <= sweepY)
//                            insertEvent(firstEvent, newEvent2);
//                        else
//                            delete newEvent2;

                        break;
                    } else if (temp1->left == nullptr &&
                               points[pointIter].x < x1) { //if point is on the left of the first parabola
                        Parabola *newParabola = new Parabola();
                        Parabola *copy = new Parabola();

                        copy->p = temp1->p;
                        copy->right = temp1->right;
                        copy->left = newParabola;

                        newParabola->p = points[pointIter];
                        newParabola->event = nullptr;
                        newParabola->left = temp1;
                        newParabola->right = copy;

                        temp1->right->left = copy;
                        temp1->right = newParabola;

                        //nowy circle event temp1-new-copy(temp1)-temp2
                        a1 = (points[pointIter].x - temp1->p.x) / (temp1->p.y - points[pointIter].y);
                        b1 = (points[pointIter].y + temp1->p.y) / 2 - a1 * (points[pointIter].x + temp1->p.x) / 2;
                        a2 = (temp1->p.x - temp2->p.x) / (temp2->p.y - temp1->p.y);
                        b2 = (temp1->p.y + temp2->p.y) / 2 - a2 * (temp1->p.x + temp2->p.x) / 2;

                        x1 = (b1 - b2) / (a2 - a1);
                        y1 = a2 * x1 + b2; //crossing point
                        Event *newEvent = new Event();
                        newEvent->radius = distance(Point(x1, y1), copy->p);
                        newEvent->p.x = x1;
                        newEvent->p.y = y1 - newEvent->radius;
                        newEvent->parabola = copy;
                        copy->event = newEvent;

                        if(newEvent->p.y <= sweepY) {
                            if (firstEvent == nullptr) {
                                firstEvent = newEvent;
                                firstEvent->next = nullptr;
                                firstEvent->prev = nullptr;
                            } else
                                insertEvent(&firstEvent, &newEvent);
                        }
                        else
                            delete newEvent;

                        break;
                    } else if (temp3->right == nullptr) { //if point is on the right of the last Parabola
                        Parabola *newParabola = new Parabola();
                        Parabola *copy = new Parabola();

                        copy->p = temp3->p;
                        copy->right = temp3->right; //nullptr
                        copy->left = newParabola;

                        newParabola->p = points[pointIter];
                        newParabola->event = nullptr;
                        newParabola->left = temp3;
                        newParabola->right = copy;

                        temp3->right = newParabola;

                        //nowy circle event temp2-temp3-new-copy(temp3)
                        a1 = (temp2->p.x - temp3->p.x) / (temp3->p.y - temp2->p.y);
                        b1 = (temp2->p.y + temp3->p.y) / 2 - a1 * (temp2->p.x + temp3->p.x) / 2;
                        a2 = (points[pointIter].x - temp3->p.x) / (temp3->p.y - points[pointIter].y);
                        b2 = (points[pointIter].y + temp3->p.y) / 2 - a2 * (points[pointIter].x + temp3->p.x) / 2;

                        x1 = (b1 - b2) / (a2 - a1);
                        y1 = a2 * x1 + b2; //crossing point
                        Event *newEvent = new Event();
                        newEvent->radius = distance(Point(x1, y1), temp3->p);
                        newEvent->p.x = x1;
                        newEvent->p.y = y1 - newEvent->radius;
                        newEvent->parabola = temp3;
                        temp3->event = newEvent;

                        if(newEvent->p.y <= sweepY) {
                            if (firstEvent == nullptr) {
                                firstEvent = newEvent;
                                firstEvent->next = nullptr;
                                firstEvent->prev = nullptr;
                            } else
                                insertEvent(&firstEvent, &newEvent);
                        }
                        else
                            delete newEvent;

                        break;
                    }
                    temp1 = temp2;
                    temp2 = temp3;
                    temp3 = temp3->right;
                }
            }
            pointIter++;
            if(firstEvent == nullptr)
                cout << "JEST NULLEM" << endl;

            cout << "beachline2: ";
            par = firstParabola;
            while(par != nullptr){
                cout << par->p.x << " | ";
                par = par->right;
            }
            cout << endl;
        } else if(pointIter == size || firstEvent->p.y >= points[pointIter].y) { //circle event
            sweepY = firstEvent->p.y;

            Parabola *parabola = firstEvent->parabola;

            //usuwamy obecny circle event
            Event *oldEvent = parabola->left->event;
            if(oldEvent != nullptr) {
                if (oldEvent->prev == nullptr && oldEvent->next != nullptr) { //first
                    firstEvent = oldEvent->next;
                    firstEvent->prev = nullptr;
                    oldEvent->next = nullptr;
                    oldEvent->parabola = nullptr;
                } else if (oldEvent->next == nullptr && oldEvent->prev != nullptr) { //last
                    oldEvent->prev->next = nullptr;
                    oldEvent->prev = nullptr;
                    oldEvent->parabola = nullptr;
                } else if (oldEvent->next == nullptr && oldEvent->prev == nullptr) { //lone
                    firstEvent = nullptr;
                    oldEvent->parabola = nullptr;
                } else { //in the middle
                    oldEvent->next->prev = oldEvent->prev;
                    oldEvent->prev->next = oldEvent->next;
                    oldEvent->next = nullptr;
                    oldEvent->prev = nullptr;
                    oldEvent->parabola = nullptr;
                }
                delete oldEvent;
                parabola->left->event = nullptr;
            }
            oldEvent = parabola->right->event;
            if(oldEvent != nullptr) {
                if (oldEvent->prev == nullptr && oldEvent->next != nullptr) { //first
                    firstEvent = oldEvent->next;
                    firstEvent->prev = nullptr;
                    oldEvent->next = nullptr;
                    oldEvent->parabola = nullptr;
                } else if (oldEvent->next == nullptr && oldEvent->prev != nullptr) { //last
                    oldEvent->prev->next = nullptr;
                    oldEvent->prev = nullptr;
                    oldEvent->parabola = nullptr;
                } else if (oldEvent->next == nullptr && oldEvent->prev == nullptr) { //lone
                    firstEvent = nullptr;
                    oldEvent->parabola = nullptr;
                } else { //in the middle
                    oldEvent->next->prev = oldEvent->prev;
                    oldEvent->prev->next = oldEvent->next;
                    oldEvent->next = nullptr;
                    oldEvent->prev = nullptr;
                    oldEvent->parabola = nullptr;
                }
                delete oldEvent;
                parabola->right->event = nullptr;
            }


            parabola->left->right = parabola->right;
            parabola->right->left = parabola->left;
            parabola->left = nullptr;
            parabola->right = nullptr;
            parabola->event = nullptr;
            double radius = firstEvent->radius;
            cout << "Event: (" << firstEvent->p.x << ", " << firstEvent->p.y+radius << "), radius: " << radius << endl;


            if (radius > maxRadius)
                if (firstEvent->p.x - radius >= rectangleMinX && firstEvent->p.x + radius <= rectangleMaxX
                    && firstEvent->p.y + radius <= rectangleMaxY && firstEvent->p.y - radius >= rectangleMinY) {
                    maxRadius = radius;
                    maxCircleX = firstEvent->p.x;
                    maxCircleY = firstEvent->p.y;
                }

            Event *temp = firstEvent;

            if (firstEvent->next != nullptr) {
                firstEvent = firstEvent->next;
                firstEvent->prev = nullptr;
                temp->next = nullptr;
                temp->parabola = nullptr;
            } else
                firstEvent = nullptr;

            delete temp;
            delete parabola;
        }
    }

    //cout << "Parabola: " << firstParabola->p.x << " " << firstParabola->p.y << endl;
    cout << "\nCircle: (" << maxCircleX << ", " << maxCircleY << "), R = " << maxRadius << endl;

    while (firstParabola != nullptr) {
        Parabola *temp = firstParabola;
        firstParabola = firstParabola->right;
        temp->left = nullptr;
        temp->right = nullptr;
        delete temp;
    }

    while (firstEvent != nullptr) {
        Event *temp = firstEvent;
        firstEvent = firstEvent->next;
        temp->prev = nullptr;
        temp->next = nullptr;
        delete temp;
    }

    //brute-force
    Point p1, p2, p3;
    double a1, a2, a3, b1, b2, b3, x1, y1, radius;
    maxRadius = 0;
    for(int i = 0; i < size; i++){
        p1 = points[i];
        for(int j = i+1; j < size; j++){
            p2 = points[j];
            for(int k = j+1; k < size; k++){
                p3 = points[k];

                if((p1.x == p2.x && p2.x == p3.x) || (p1.y == p2.y && p2.y == p3.y))
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
                        for(int z = 0; z < size; z++) {
                            if(z == i || z == j || z == k)
                                continue;
                            double zx = points[z].x - x1;
                            double zy = points[z].y - y1;
                            if(zx * zx + zy * zy < radius * radius){
                                isEmpty = false;
                                break;
                            }
                        }
                        if(isEmpty){
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