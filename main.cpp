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

void removeEvent(Event **beg, Event **act) {
    if ((*beg) == nullptr || (*act) == nullptr)
        return;

    (*act)->edge1->event = nullptr;
    (*act)->edge2->event = nullptr;
    (*act)->edge1 = nullptr;
    (*act)->edge2 = nullptr;

    if ((*beg)->next == nullptr) { //jedyny element
        delete (*beg);
        (*beg) = nullptr;
        return;
    }

    if ((*act) == (*beg)) { //pierwszy
        (*beg) = (*beg)->next;
        delete (*beg)->prev;
        (*beg)->prev = nullptr;
    } else if ((*act)->next == nullptr) { //ostatni
        (*act)->prev->next = nullptr;
        (*act)->prev = nullptr;
        delete (*act);
        (*act) = nullptr;
    } else { //w srodku
        (*act)->prev->next = (*act)->next;
        (*act)->next->prev = (*act)->prev;
        (*act)->prev = nullptr;
        (*act)->next = nullptr;
        delete (*act);
        (*act) = nullptr;
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
    firstParabola->left = nullptr;
    firstParabola->right = nullptr;

    while (1) {     //main loop
        if ((pointIter == size && firstEvent == nullptr) || sweepY < rectangleMinY)
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
            } else { //mamy wiecej niz jedna parabole
                Parabola *temp1 = firstParabola;
                Edge *leftEdge = temp1->right;
                Parabola *temp2 = leftEdge->right;
                Edge *rightEdge = temp2->right;
                Parabola *temp3 = rightEdge->right;

                //TODO przypadek gdy pierwsze dwa punkty maja takie same y

                double a0, b0, a1, b1, c1, a2, b2, c2, a3, b3, c3, k, p, delta, x1, y1, x2, y2, x2prev;
                cout << "wincyj paraboli" << endl;

                while (1) { //szukamy paraboli
                    /*if (temp1 == firstParabola) { //pierwsze przejscie petli
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
                            x1 = leftEdge->a; //jest pionowy

                        delta = (b2 - rightEdge->a) * (b2 - rightEdge->a) - 4 * a2 * (c2 - rightEdge->b);
                        if (rightEdge->xDirection < 0) //promien leci w lewo
                            x2 = (rightEdge->a - b2 - sqrt(delta)) / (2 * a2); //punkt przeciecia na lewo
                        else if (rightEdge->xDirection > 0)
                            x2 = (rightEdge->a - b2 + sqrt(delta)) / (2 * a2); //punkt przeciecia na prawo
                        else
                            x2 = rightEdge->a; //jest pionowy
                    } else {*/ //przynajmniej druga iteracja
                        //jesli istnieja dwa dalej to lecimy co druga parabole
                        //jesli nie istnieje to znaczy ze koniec, czyli lecimy o jedna i sprawdzamy
                        //czy lezy w srodkowej czy na koncowej
                        //UWAGA - jest tez mozliwosc, ze nie istnieje juz nawet nastepny
                        k = (temp2->p.y + sweepY) / 2;
                        p = (temp2->p.y - sweepY) / 2;
                        a2 = 1 / (4 * p);
                        b2 = -(temp2->p.x / (2 * p));
                        c2 = (temp2->p.x * temp2->p.x / (4 * p)) + k;

                        x2prev = x2;

                        delta = (b2 - leftEdge->a) * (b2 - leftEdge->a) - 4 * a2 * (c2 - leftEdge->b);
                        if (leftEdge->xDirection < 0) //promien leci w lewo
                            x1 = (leftEdge->a - b2 - sqrt(delta)) / (2 * a2); //punkt przeciecia na lewo
                        else if (leftEdge->xDirection > 0)
                            x1 = (leftEdge->a - b2 + sqrt(delta)) / (2 * a2); //punkt przeciecia na prawo
                        else
                            x1 = leftEdge->a; //jest pionowy

                        delta = (b2 - rightEdge->a) * (b2 - rightEdge->a) - 4 * a2 * (c2 - rightEdge->b);
                        if (rightEdge->xDirection < 0) //promien leci w lewo
                            x2 = (rightEdge->a - b2 - sqrt(delta)) / (2 * a2); //punkt przeciecia na lewo
                        else if (rightEdge->xDirection > 0)
                            x2 = (rightEdge->a - b2 + sqrt(delta)) / (2 * a2); //punkt przeciecia na prawo
                        else
                            x2 = rightEdge->a; //jest pionowy

                        //sprawdzamy czy trafilismy na odpowiednia parabole:
                        if (points[pointIter].x > x1 && points[pointIter].x < x2) { //znalazlem parabole (temp2)
                            //dodajemy nowa parabole i edge
                            Parabola *newParabola = new Parabola();
                            Parabola *copyParabola = new Parabola;
                            Edge *newLeftEdge = new Edge();
                            Edge *newRightEdge = new Edge();

                            cout << "ZNALAZLEM PARABOLE" << endl;

                            copyParabola->p = temp2->p;
                            copyParabola->right = temp2->right;
                            copyParabola->left = newRightEdge;

                            newParabola->p = points[pointIter];
                            newParabola->left = newLeftEdge;
                            newParabola->right = newRightEdge;

                            temp2->right->left = copyParabola;
                            temp2->right = newLeftEdge;


                            a0 = (temp2->p.x - points[pointIter].x) / (points[pointIter].y - temp2->p.y);
                            b0 = (temp2->p.y + points[pointIter].y) / 2 - a0 * (temp2->p.x + points[pointIter].x) / 2;

                            y1 = a0 * points[pointIter].x + b0;

                            newLeftEdge->p = newRightEdge->p = Point(points[pointIter].x, y1);
                            newLeftEdge->a = newRightEdge->a = a0;
                            newLeftEdge->b = newRightEdge->b = b0;
                            newLeftEdge->xDirection = -1;
                            newLeftEdge->event = nullptr;
                            newLeftEdge->left = temp2;
                            newLeftEdge->right = newParabola;
                            newRightEdge->xDirection = 1;
                            newRightEdge->event = nullptr;
                            newRightEdge->left = newParabola;
                            newRightEdge->right = copyParabola;

                            //sprawdzenie eventu po prawej stronie:
                            //przeciecie sasiednich promieni:
                            x1 = (newRightEdge->b - rightEdge->b) / (rightEdge->a - newRightEdge->a);
                            y1 = newRightEdge->a * x1 + newRightEdge->b;


                            Event *event = rightEdge->event;

                            double radius = distance(Point(x1, y1), points[pointIter]);

//                            if((rightEdge->xDirection > 0 && rightEdge->a < 0 && y1 < rightEdge->p.y) ||
//                                    (rightEdge->xDirection < 0 && rightEdge->a > 0 && y1 < rightEdge->p.y) ||
//                                    (rightEdge->xDirection < 0 && rightEdge->a < 0 && y1 < rightEdge->p.y)) {
                            if((rightEdge->xDirection > 0 && x1 > rightEdge->p.x) ||
                               (rightEdge->xDirection < 0 && x1 < rightEdge->p.x)){
                                if (event == nullptr && (y1 - radius + 0.00001) < sweepY) { //przypisujemy swoj event
                                    event = new Event();
                                    event->radius = radius;
                                    event->p = Point(x1, y1 - radius);
                                    event->edge1 = newRightEdge;
                                    event->edge2 = rightEdge;
                                    newRightEdge->event = event;
                                    rightEdge->event = event;
                                    insertEvent(&firstEvent, &event);
                                }
                                    //najpierw usuwamy event (jesli mozemy), potem przypisujemy swoj
                                else if (event != nullptr && (y1 - radius + 0.00001) < sweepY){ // && y1 > (event->p.y + event->radius)) {
                                    if(distance(Point(event->p.x, event->p.y + event->radius), points[pointIter]) < event->radius) {
                                        removeEvent(&firstEvent, &event);

                                        Event *newEvent = new Event();
                                        newEvent->radius = radius;
                                        newEvent->p = Point(x1, y1 - radius);
                                        newEvent->edge1 = newRightEdge;
                                        newEvent->edge2 = rightEdge;
                                        newRightEdge->event = newEvent;
                                        rightEdge->event = newEvent;
                                        insertEvent(&firstEvent, &newEvent);
                                    }
                                }
                            }


                            //sprawdzenie eventu po lewej stronie:
                            //przeciecie sasiednich promieni:
                            x1 = (leftEdge->b - newLeftEdge->b) / (newLeftEdge->a - leftEdge->a);
                            y1 = leftEdge->a * x1 + leftEdge->b;

                            event = leftEdge->event;

                            radius = distance(Point(x1, y1), points[pointIter]);

//                            if((leftEdge->xDirection > 0 && leftEdge->a > 0 && y1 > leftEdge->p.y) ||
//                               (leftEdge->xDirection > 0 && leftEdge->a < 0 && y1 < leftEdge->p.y) ||
//                               (leftEdge->xDirection < 0 && leftEdge->a < 0 && y1 < leftEdge->p.y)) {
                            if((leftEdge->xDirection > 0 && x1 > leftEdge->p.x) ||
                                    (leftEdge->xDirection < 0 && x1 < leftEdge->p.x)){
                                if (event == nullptr && (y1 - radius + 0.00001) < sweepY) { //przypisujemy swoj event
                                    event = new Event();
                                    event->radius = radius;
                                    event->p = Point(x1, y1 - radius);
                                    event->edge1 = leftEdge;
                                    event->edge2 = newLeftEdge;
                                    newLeftEdge->event = event;
                                    leftEdge->event = event;
                                    insertEvent(&firstEvent, &event);
                                }
                                    //najpierw usuwamy event (jesli mozemy), potem przypisujemy swoj
                                else if (event != nullptr && (y1 - radius + 0.00001) < sweepY){ //&& y1 > (event->p.y + event->radius)) {
                                    if(distance(Point(event->p.x, event->p.y + event->radius), points[pointIter]) < event->radius) {
                                        removeEvent(&firstEvent, &event);
                                        //TODO ten distance powyzej moze byc zle

                                        Event *newEvent = new Event();
                                        newEvent->radius = radius;
                                        newEvent->p = Point(x1, y1 - radius);
                                        newEvent->edge1 = leftEdge;
                                        newEvent->edge2 = newLeftEdge;
                                        newLeftEdge->event = newEvent;
                                        leftEdge->event = newEvent;
                                        insertEvent(&firstEvent, &newEvent);
                                    }
                                }
                            }

                            break;
                        } else if (points[pointIter].x < x1) { //znalazlem parabole (temp1)
                            Parabola *newParabola = new Parabola();
                            Parabola *copyParabola = new Parabola();
                            Edge *newLeftEdge = new Edge();
                            Edge *newRightEdge = new Edge();

                            //TODO - PRZYPADKI GDY SYMETRALNA JEST PIONOWA

                            copyParabola->p = temp1->p;
                            copyParabola->right = temp1->right;
                            copyParabola->left = newRightEdge;

                            newParabola->p = points[pointIter];
                            newParabola->left = newLeftEdge;
                            newParabola->right = newRightEdge;

                            temp1->right->left = copyParabola;
                            temp1->right = newLeftEdge;

                            k = (temp1->p.y + sweepY) / 2;
                            p = (temp1->p.y - sweepY) / 2;
                            a2 = 1 / (4 * p);
                            b2 = -(temp1->p.x / (2 * p));
                            c2 = (temp1->p.x * temp1->p.x / (4 * p)) + k;

                            a0 = (temp1->p.x - points[pointIter].x) / (points[pointIter].y - temp1->p.y);
                            b0 = (temp1->p.y + points[pointIter].y) / 2 - a0 * (temp1->p.x + points[pointIter].x) / 2;

                            y1 = a0 * points[pointIter].x + b0;

                            newLeftEdge->p = newRightEdge->p = Point(points[pointIter].x, y1);
                            newLeftEdge->a = newRightEdge->a = a0;
                            newLeftEdge->b = newRightEdge->b = b0;
                            newLeftEdge->xDirection = -1;
                            newLeftEdge->event = nullptr;
                            newLeftEdge->left = temp1;
                            newLeftEdge->right = newParabola;
                            newRightEdge->xDirection = 1;
                            newRightEdge->event = nullptr;
                            newRightEdge->left = newParabola;
                            newRightEdge->right = copyParabola;

                            //sprawdzenie eventu po prawej stronie:
                            //przeciecie sasiednich promieni:
                            x1 = (newRightEdge->b - leftEdge->b) / (leftEdge->a - newRightEdge->a);
                            y1 = newRightEdge->a * x1 + newRightEdge->b;

                            Event *event = leftEdge->event;

                            double radius = distance(Point(x1, y1), points[pointIter]);

//                            if((leftEdge->xDirection > 0 && leftEdge->a < 0 && y1 < leftEdge->p.y) ||
//                               (leftEdge->xDirection < 0 && leftEdge->a > 0 && y1 < leftEdge->p.y) ||
//                               (leftEdge->xDirection < 0 && leftEdge->a < 0 && y1 < leftEdge->p.y)) {
                            if((leftEdge->xDirection > 0 && x1 > leftEdge->p.x) ||
                                   (leftEdge->xDirection < 0 && x1 < leftEdge->p.x)){
                                if (event == nullptr && (y1 - radius + 0.00001) < sweepY) { //przypisujemy swoj event
                                    event = new Event();
                                    event->radius = radius;
                                    event->p = Point(x1, y1 - radius);
                                    event->edge1 = newRightEdge;
                                    event->edge2 = leftEdge;
                                    newRightEdge->event = event;
                                    leftEdge->event = event;
                                    insertEvent(&firstEvent, &event);
                                }
                                    //najpierw usuwamy event (jesli mozemy), potem przypisujemy swoj
                                else if (event != nullptr && (y1 - radius + 0.00001) < sweepY){ // && y1 > (event->p.y + event->radius)) {
                                    if(distance(Point(event->p.x, event->p.y + event->radius), points[pointIter]) < event->radius) {
                                        removeEvent(&firstEvent, &event);

                                        Event *newEvent = new Event();
                                        newEvent->radius = radius;
                                        newEvent->p = Point(x1, y1 - radius);
                                        newEvent->edge1 = newRightEdge;
                                        newEvent->edge2 = leftEdge;
                                        newRightEdge->event = newEvent;
                                        leftEdge->event = event;
                                        insertEvent(&firstEvent, &newEvent);
                                    }
                                }
                            }

                            //sprawdzenie eventu po lewej stronie, o ile istnieje (nie firstParabola)
                            if(temp1->left != nullptr){
                                Edge *edge = temp1->left;

                                x1 = (edge->b - newLeftEdge->b) / (newLeftEdge->a - edge->a);
                                y1 = newLeftEdge->a * x1 + newLeftEdge->b;

                                event = edge->event;

                                radius = distance(Point(x1, y1), points[pointIter]);

//                                if((edge->xDirection > 0 && edge->a > 0 && y1 > edge->p.y) ||
//                                   (edge->xDirection > 0 && edge->a < 0 && y1 < edge->p.y) ||
//                                   (edge->xDirection < 0 && edge->a < 0 && y1 < edge->p.y)) {
                                if((edge->xDirection > 0 && x1 > edge->p.x) ||
                                   (edge->xDirection < 0 && x1 < edge->p.x)){
                                    if (event == nullptr && (y1 - radius + 0.00001) < sweepY) { //przypisujemy swoj event
                                        event = new Event();
                                        event->radius = radius;
                                        event->p = Point(x1, y1 - radius);
                                        event->edge1 = edge;
                                        event->edge2 = newLeftEdge;
                                        newLeftEdge->event = event;
                                        edge->event = event;
                                        insertEvent(&firstEvent, &event);
                                    }
                                        //najpierw usuwamy event (jesli mozemy), potem przypisujemy swoj
                                    else if (event != nullptr && (y1 - radius + 0.00001) < sweepY){ // && y1 > (event->p.y + event->radius)) {
                                        if(distance(Point(event->p.x, event->p.y + event->radius), points[pointIter]) < event->radius) {
                                            removeEvent(&firstEvent, &event);

                                            Event *newEvent = new Event();
                                            newEvent->radius = radius;
                                            newEvent->p = Point(x1, y1 - radius);
                                            newEvent->edge1 = edge;
                                            newEvent->edge2 = newLeftEdge;
                                            newLeftEdge->event = newEvent;
                                            edge->event = newEvent;
                                            insertEvent(&firstEvent, &newEvent);
                                        }
                                    }
                                }
                            }

                            break;

                        } else if (temp3->right == nullptr && points[pointIter].x > x2) { //znalazlem parabole (temp3)
                            Parabola *newParabola = new Parabola();
                            Parabola *copyParabola = new Parabola();
                            Edge *newLeftEdge = new Edge();
                            Edge *newRightEdge = new Edge();

                            copyParabola->p = temp3->p;
                            copyParabola->right = temp3->right;
                            copyParabola->left = newRightEdge;

                            newParabola->p = points[pointIter];
                            newParabola->left = newLeftEdge;
                            newParabola->right = newRightEdge;

                            temp3->right = newLeftEdge;

                            k = (temp3->p.y + sweepY) / 2;
                            p = (temp3->p.y - sweepY) / 2;
                            a2 = 1 / (4 * p);
                            b2 = -(temp3->p.x / (2 * p));
                            c2 = (temp3->p.x * temp3->p.x / (4 * p)) + k;

                            a0 = (temp3->p.x - points[pointIter].x) / (points[pointIter].y - temp3->p.y);
                            b0 = (temp3->p.y + points[pointIter].y) / 2 - a0 * (temp3->p.x + points[pointIter].x) / 2;

                            y1 = a0 * points[pointIter].x + b0;

                            newLeftEdge->p = newRightEdge->p = Point(points[pointIter].x, y1);
                            newLeftEdge->a = newRightEdge->a = a0;
                            newLeftEdge->b = newRightEdge->b = b0;
                            newLeftEdge->xDirection = -1;
                            newLeftEdge->event = nullptr;
                            newLeftEdge->left = temp3;
                            newLeftEdge->right = newParabola;
                            newRightEdge->xDirection = 1;
                            newRightEdge->event = nullptr;
                            newRightEdge->left = newParabola;
                            newRightEdge->right = copyParabola;

                            //sprawdzenie eventu po lewej stronie:
                            //przeciecie sasiednich promieni:
                            x1 = (rightEdge->b - newLeftEdge->b) / (newLeftEdge->a - rightEdge->a);
                            y1 = rightEdge->a * x1 + rightEdge->b;

                            Event *event = rightEdge->event;

                            double radius = distance(Point(x1, y1), points[pointIter]);

//                            if((rightEdge->xDirection > 0 && rightEdge->a > 0 && y1 > rightEdge->p.y) ||
//                                    (rightEdge->xDirection > 0 && rightEdge->a < 0 && y1 < rightEdge->p.y) ||
//                                    (rightEdge->xDirection < 0 && rightEdge->a < 0 && y1 < rightEdge->p.y)) {
                            if((rightEdge->xDirection > 0 && x1 > rightEdge->p.x) ||
                               (rightEdge->xDirection < 0 && x1 < rightEdge->p.x)){
                                if (event == nullptr && (y1 - radius + 0.00001) < sweepY) { //przypisujemy swoj event
                                    event = new Event();
                                    event->radius = radius;
                                    event->p = Point(x1, y1 - radius);
                                    event->edge1 = rightEdge;
                                    event->edge2 = newLeftEdge;
                                    newLeftEdge->event = event;
                                    rightEdge->event = event;
                                    insertEvent(&firstEvent, &event);
                                }
                                    //najpierw usuwamy event (jesli mozemy), potem przypisujemy swoj
                                else if (event != nullptr && (y1 - radius + 0.00001) < sweepY){ // && y1 > (event->p.y + event->radius)) {
                                    if(distance(Point(event->p.x, event->p.y + event->radius), points[pointIter]) < event->radius) {
                                        removeEvent(&firstEvent, &event);

                                        Event *newEvent = new Event();
                                        newEvent->radius = radius;
                                        newEvent->p = Point(x1, y1 - radius);
                                        newEvent->edge1 = rightEdge;
                                        newEvent->edge2 = newLeftEdge;
                                        newLeftEdge->event = newEvent;
                                        rightEdge->event = newEvent;
                                        insertEvent(&firstEvent, &newEvent);
                                    }
                                }
                            }

                            break;
                        }
                    //}

                    if(temp3->right == nullptr) //jesli koniec
                        break;
                    else if(temp3->right != nullptr){
                        if(temp3->right->right->right != nullptr) //sa dwa wolne
                            temp1 = temp3;
                        else //jest tylko jedno wolne
                            temp1 = temp2;
                        leftEdge = temp1->right;
                        temp2 = leftEdge->right;
                        rightEdge= temp2->right;
                        temp3 = rightEdge->right;
                    }
                }
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
        else if(pointIter >= size || firstEvent->p.y >= points[pointIter].y){ //circle event
            sweepY = firstEvent->p.y;

            Edge *leftEdge = firstEvent->edge1;
            Edge *rightEdge = firstEvent->edge2;
            Parabola *temp1 = leftEdge->left;
            Parabola *temp2 = rightEdge->left;
            Parabola *temp3 = rightEdge->right;

            //usuwamy temp2 i rightEdge, zmieniamy parametry leftEdge
            //ostatecznie: temp1 - leftEdge - temp3

            //obliczanie kierunku nowego promienia
            int direction;
//            if(leftEdge->xDirection == rightEdge->xDirection)
//                direction = leftEdge->xDirection;
//            else{
//                double dir = distance(temp1->p, temp2->p) * abs(leftEdge->a) * leftEdge->xDirection + distance(temp2->p, temp3->p) * abs(rightEdge->a) * rightEdge->xDirection;
//                direction = (dir < 0 ? -1 : 1);
//            }
            double a = (temp1->p.y - temp3->p.y) / (temp1->p.x - temp3->p.x);
            double b = temp1->p.y - a * temp1->p.x;
            double yP = a * temp2->p.x + b;
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
            cout << "CIRCLE event: (" << firstEvent->p.x << ", " << firstEvent->p.y+radius << "), radius: " << radius << endl;

            if (radius > maxRadius)
                if (firstEvent->p.x - radius >= rectangleMinX && firstEvent->p.x + radius <= rectangleMaxX
                    && firstEvent->p.y + 2 * radius <= rectangleMaxY && firstEvent->p.y >= rectangleMinY) {
                    maxRadius = radius;
                    maxCircleX = firstEvent->p.x;
                    maxCircleY = firstEvent->p.y + radius;
                }

            //obliczenie nowych parametrow edge
            double a0 = (temp1->p.x - temp3->p.x) / (temp3->p.y - temp1->p.y);
            double b0 = (temp1->p.y + temp3->p.y) / 2 - a0 * (temp1->p.x + temp3->p.x) / 2;

            if(a0 > 0){ //TODO nie ma 100% pewnosci ze to dobrze
                if(y2 < yP)
                    direction = 1;
                else
                    direction = -1;
            }
            else{
                if(y2 < yP)
                    direction = -1;
                else
                    direction = 1;
            }

            leftEdge->a = a0;
            leftEdge->b = b0;
            leftEdge->p = Point(firstEvent->p.x, firstEvent->p.y + radius);
//            leftEdge->xDirection = (a0 > 0 ? -1 : 1);
            leftEdge->xDirection = direction;
            leftEdge->event = nullptr;

            //usuniecie wykorzystanego circle event
            Point point = Point(firstEvent->p.x, firstEvent->p.y + firstEvent->radius);
            removeEvent(&firstEvent, &firstEvent);

            //obliczanie nowych circle event:
            Edge *middleEdge = leftEdge;
            rightEdge = temp3->right;
            leftEdge = temp1->left;

            //sprawdzenie eventu po lewej stronie:
            //przeciecie sasiednich promieni:
            if(leftEdge != nullptr) {
                double x1 = (leftEdge->b - middleEdge->b) / (middleEdge->a - leftEdge->a);
                double y1 = middleEdge->a * x1 + middleEdge->b;

                Event *event = leftEdge->event;
                radius = distance(Point(x1, y1), temp1->p);

//                if ((leftEdge->xDirection > 0 && leftEdge->a > 0 && y1 > leftEdge->p.y) ||
//                    (leftEdge->xDirection > 0 && leftEdge->a < 0 && y1 < leftEdge->p.y) ||
//                    (leftEdge->xDirection < 0 && leftEdge->a < 0 && y1 < leftEdge->p.y)) {

                //TODO sprawdzac jakos czy middleEdge zajete - jak dotad to sa sytuacje, ze bez sensu je nadpisuje
                if((leftEdge->xDirection > 0 && x1 > leftEdge->p.x) ||
                   (leftEdge->xDirection < 0 && x1 < leftEdge->p.x)){
                    if (event == nullptr && (y1 - radius + 0.00001) < sweepY) { //przypisujemy swoj event
                        if(middleEdge->event == nullptr || (middleEdge->event != nullptr &&
                                distance(Point(middleEdge->event->p.x, middleEdge->event->p.y + middleEdge->event->radius), point) < middleEdge->event->radius)) {

                            if(middleEdge->event != nullptr)
                                removeEvent(&firstEvent, &middleEdge->event);

                            event = new Event(); //TODO to sprawdzanie middleEdge jest dziwne
                            event->radius = radius;
                            event->p = Point(x1, y1 - radius);
                            event->edge1 = leftEdge;
                            event->edge2 = middleEdge;
                            middleEdge->event = event;
                            leftEdge->event = event;
                            insertEvent(&firstEvent, &event);
                        }
                    }
                        //najpierw usuwamy event (jesli mozemy), potem przypisujemy swoj
                    else if (event != nullptr && (y1 - radius + 0.00001) < sweepY){ // && y1 > (event->p.y + event->radius)) {
                        if(middleEdge->event == nullptr || (middleEdge->event != nullptr &&
                                distance(Point(middleEdge->event->p.x, middleEdge->event->p.y + middleEdge->event->radius), point) < middleEdge->event->radius)) {
                            if(distance(Point(event->p.x, event->p.y + event->radius), point) < event->radius) {

                                if(middleEdge->event != nullptr)
                                    removeEvent(&firstEvent, &middleEdge->event);

                                removeEvent(&firstEvent, &event);

                                Event *newEvent = new Event();
                                newEvent->radius = radius;
                                newEvent->p = Point(x1, y1 - radius);
                                newEvent->edge1 = leftEdge;
                                newEvent->edge2 = middleEdge;
                                middleEdge->event = newEvent;
                                leftEdge->event = newEvent;
                                insertEvent(&firstEvent, &newEvent);
                            }
                        }
                    }
                }
            }
            //sprawdzenie eventu po prawej stronie:
            if(rightEdge != nullptr) {
                double x1 = (rightEdge->b - middleEdge->b) / (middleEdge->a - rightEdge->a);
                double y1 = middleEdge->a * x1 + middleEdge->b;

                Event *event = rightEdge->event;

                radius = distance(Point(x1, y1), temp3->p);

//                if ((leftEdge->xDirection > 0 && leftEdge->a < 0 && y1 < leftEdge->p.y) ||
//                    (leftEdge->xDirection < 0 && leftEdge->a > 0 && y1 < leftEdge->p.y) ||
//                    (leftEdge->xDirection < 0 && leftEdge->a < 0 && y1 < leftEdge->p.y)) {
                if((rightEdge->xDirection > 0 && x1 > rightEdge->p.x) ||
                   (rightEdge->xDirection < 0 && x1 < rightEdge->p.x)){
                    if (event == nullptr && (y1 - radius + 0.00001) < sweepY) { //przypisujemy swoj event
                        if(middleEdge->event == nullptr || (middleEdge->event != nullptr &&
                                                            distance(Point(middleEdge->event->p.x, middleEdge->event->p.y + middleEdge->event->radius), point) < middleEdge->event->radius)) {

                            if (middleEdge->event != nullptr)
                                removeEvent(&firstEvent, &middleEdge->event);
                            //TODO tutaj tez jest to dziwne sprawdzanie middleEdge
                            //TODO trzeba chyba to jednak pousuwac...

                            event = new Event();
                            event->radius = radius;
                            event->p = Point(x1, y1 - radius);
                            event->edge1 = middleEdge;
                            event->edge2 = rightEdge;
                            middleEdge->event = event;
                            rightEdge->event = event;
                            insertEvent(&firstEvent, &event);
                        }
                    }
                        //najpierw usuwamy event (jesli mozemy), potem przypisujemy swoj
                    else if (event != nullptr && (y1 - radius + 0.00001) < sweepY){ // && y1 > (event->p.y + event->radius) ) {
                        if(middleEdge->event == nullptr || (middleEdge->event != nullptr &&
                                                            distance(Point(middleEdge->event->p.x, middleEdge->event->p.y + middleEdge->event->radius), point) < middleEdge->event->radius)) {
                            if(distance(Point(event->p.x, event->p.y + event->radius), point) < event->radius) {

                                if(middleEdge->event != nullptr)
                                    removeEvent(&firstEvent, &middleEdge->event);

                                removeEvent(&firstEvent, &event);

                                Event *newEvent = new Event();
                                newEvent->radius = radius;
                                newEvent->p = Point(x1, y1 - radius);
                                newEvent->edge1 = middleEdge;
                                newEvent->edge2 = rightEdge;
                                middleEdge->event = newEvent;
                                rightEdge->event = newEvent;
                                insertEvent(&firstEvent, &newEvent);
                            }
                        }
                    }
                }
            }
        }
    }

    cout << "\nCircle: (" << maxCircleX << ", " << maxCircleY << "), R = " << maxRadius << endl;

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