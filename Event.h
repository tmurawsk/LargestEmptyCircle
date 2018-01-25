#ifndef LEC_EVENT_H
#define LEC_EVENT_H

#include "Point.h"
#include "Edge.h"

typedef struct Event {
    Point p;    //trigger point
    double radius;
    Edge *edge1;
    Edge *edge2;
    Event *next;
    Event *prev;

    static void insertEvent(Event **beg, Event **act) {
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
                    (*beg) = (*act);
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

    static void removeEvent(Event **beg, Event **act) {
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


} Event;
#endif //LEC_EVENT_H
