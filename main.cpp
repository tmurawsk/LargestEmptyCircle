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

void insertEvent(Event *beg, Event *act){
    if(beg == nullptr){
        beg = act;
        return;
    }

    Event *first = beg;
    while(1){
        if(first->y < act->y){
            if(first->prev == nullptr){
                first->prev = act;
                act->next = first;
                act->prev = nullptr;
                beg = act;
            }
            else{
                act->prev = first->prev;
                act->next = first;
                first->prev->next = act;
                first->prev = act;
            }
            break;
        }

        if(first->next = nullptr) {
            first->next = act;
            act->prev = first;
            act->next = nullptr;
            break;
        }
        first = first->next;
    }
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
    double rectangleMaxY = 50, rectangleMinY = -50, rectangleMaxX = 50, rectangleMinX = -50;

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
    double sweepY = rectangleMaxY;
    Parabola *firstParabola = nullptr;   //root in beachline binary tree
    Event *firstEvent = nullptr;//first circle event in a queue


    while(true){     //main loop
        if(pointIter == size && firstEvent == nullptr)
            break;

        if(firstEvent == nullptr) {  //site event
            if (firstParabola == nullptr) {    //first loop
                sweepY = points[pointIter].x;
                firstParabola = new Parabola();
                firstParabola->p = points[pointIter];
                firstParabola->event = nullptr;
                firstParabola->left = nullptr;
                firstParabola->right = nullptr;
            } else {   //not first loop
                //znajdujemy pod ktora parabola jest badany punkt (iteracja od firstParabola, wyliczanie a,b,c)
                Parabola *left = firstParabola;
                Parabola *right = firstParabola->right;
                if (right == nullptr) {
                    Parabola *parabolaCopy = new Parabola();
                    Parabola *newParabola = new Parabola();

                    parabolaCopy->p = firstParabola->p;
                    parabolaCopy->event = nullptr;
                    parabolaCopy->left = newParabola;
                    parabolaCopy->right = nullptr;

                    newParabola->p = points[pointIter];
                    newParabola->event = nullptr;
                    newParabola->left = firstParabola;
                    newParabola->right = parabolaCopy;
                } else {   //wstawiamy w srodek - szukamy przecietej paraboli; obliczamy circle event i usuwamy aktualny
                    Parabola *temp1 = firstParabola;
                    Parabola *temp2 = temp1->right;
                    Parabola *temp3 = temp2->right;
                    double a1, b1, c1, a2, b2, c2, a3, b3, c3, k, p, x1, y1, x2, y2;

                    while(temp3 != nullptr){
                        //badamy przeciecie paraboli temp1-temp2 i temp2-temp3 - moze miedzy nimi lezy nasz punkt
                        if(temp1 == firstParabola){ //beginning of the search
                            k = (temp1->p.y + sweepY) / 2;
                            p = (temp1->p.y - sweepY) / 2;
                            a1 = 1 / (4 * p);
                            b1 = -(temp1->p.x / (2*p));
                            c1 = (temp1->p.x * temp1->p.x / (4 * p)) + k;

                            k = (temp2->p.y + sweepY) / 2;
                            p = (temp2->p.y - sweepY) / 2;
                            a2 = 1 / (4 * p);
                            b2 = -(temp2->p.x / (2*p));
                            c2 = (temp2->p.x * temp2->p.x / (4 * p)) + k;

                            k = (temp3->p.y + sweepY) / 2;
                            p = (temp3->p.y - sweepY) / 2;
                            a3 = 1 / (4 * p);
                            b3 = -(temp3->p.x / (2*p));
                            c3 = (temp3->p.x * temp3->p.x / (4 * p)) + k;
                        }
                        else{ //at least second search
                            a1 = a2;
                            b1 = b2;
                            c1 = c2;
                            a2 = a3;
                            b2 = b3;
                            c2 = c3;
                            k = (temp3->p.y + sweepY) / 2;
                            p = (temp3->p.y - sweepY) / 2;
                            a3 = 1 / (4 * p);
                            b3 = -(temp3->p.x / (2*p));
                            c3 = (temp3->p.x * temp3->p.x / (4 * p)) + k;
                        }
                        //obliczenie przeciecia parabol
                        k = (b2 - b1)*(b2 - b1) - 4*(a2 - a1)*(c2 - c1);
                        x1 = (b1 - b2 - sqrt(k)) / (2 * (a2 - a1));
                        x2 = (b2 - b3 + sqrt(k)) / (2 * (a3 - a2));

                        if(points[pointIter].x >= x1 && points[pointIter].x <= x2){ //found the parabola
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

                            //usuwamy obecny circle event
                            Event *oldEvent = temp2->event;
                            if(oldEvent->prev == nullptr && oldEvent->next != nullptr){ //first
                                firstEvent = oldEvent->next;
                                firstEvent->prev = nullptr;
                                oldEvent->next = nullptr;
                                oldEvent->parabola = nullptr;
                            }
                            else if(oldEvent->next == nullptr && oldEvent->prev != nullptr){ //last
                                oldEvent->prev->next = nullptr;
                                oldEvent->prev = nullptr;
                                oldEvent->parabola = nullptr;
                            }
                            else if(oldEvent->next == nullptr && oldEvent->prev == nullptr){ //lone
                                firstEvent = nullptr;
                                oldEvent->parabola = nullptr;
                            }
                            else{ //in the middle
                                oldEvent->next->prev = oldEvent->prev;
                                oldEvent->prev->next = oldEvent->next;
                                oldEvent->next = nullptr;
                                oldEvent->prev = nullptr;
                                oldEvent->parabola = nullptr;
                            }
                            delete oldEvent;
                            temp2->event = nullptr;

                            //obliczamy nowy circle event temp1-temp2-new-copy(temp2)-temp3
                            a2 = (points[pointIter].x - temp2->p.x) / (temp2->p.y - points[pointIter].y);
                            b2 = (points[pointIter].y + temp2->p.y) / 2 - a2 * (points[pointIter].x + temp2->p.x) / 2;
                            a1 = (temp1->p.x - temp2->p.x) / (temp2->p.y - temp1->p.y);
                            b1 = (temp1->p.y + temp2->p.y) / 2 - a1 * (temp1->p.x + temp2->p.x) / 2;
                            a3 = (temp2->p.x - temp3->p.x) / (temp3->p.y - temp2->p.y);
                            b3 = (temp2->p.y + temp3->p.y) / 2 - a3 * (temp2->p.x + temp3->p.x) / 2;
                            y1 = a2 * (b1 - b2) / (a2 - a1) + b2; //crossing point on the left
                            y2 = a2 * (b2 - b3) / (a3 - a2) + b2; //crossing point on the right

                            Event *newEvent1 = new Event();
                            Event *newEvent2 = new Event();
                            newEvent1->y = y1;
                            newEvent2->y = y2;
                            newEvent1->parabola = temp2;
                            newEvent2->parabola = copy;

                            insertEvent(firstEvent, newEvent1);
                            insertEvent(firstEvent, newEvent2);

                            break;
                        }
                        else if(temp1->left == nullptr && points[pointIter].x < x1){ //if point is on the left of the first parabola
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

                            y1 = a2 * (b1 - b2) / (a2 - a1) + b2; //crossing point
                            Event *newEvent = new Event();
                            newEvent->y = y1;
                            newEvent->parabola = copy;

                            insertEvent(firstEvent, newEvent);

                            break;
                        }
                        else if(temp3->right == nullptr) { //if point is on the right of the last Parabola
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

                            y1 = a2 * (b1 - b2) / (a2 - a1) + b2; //crossing point
                            Event *newEvent = new Event();
                            newEvent->y = y1;
                            newEvent->parabola = temp3;

                            insertEvent(firstEvent, newEvent);

                            break;
                        }
                    }
                }
            }
            pointIter++;
        }
    }

    cout << firstParabola->p.x << " " << firstParabola->p.y << endl;

    while(firstParabola != nullptr){
        Parabola *temp = firstParabola;
        firstParabola = firstParabola->right;
        temp->left = nullptr;
        temp->right = nullptr;
        delete temp;
    }

    while(firstEvent != nullptr){
        Event *temp = firstEvent;
        firstEvent = firstEvent->next;
        temp->prev = nullptr;
        temp->next = nullptr;
        delete temp;
    }

    file.close();

    return 0;
}