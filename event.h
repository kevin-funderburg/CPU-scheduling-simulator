/**
 * Representation of a event in the system.
 */

#ifndef EVENT_H
#define EVENT_H

enum EventType {ARRIVE = 0, DEPARTURE = 1, ALLOCATION = 2, COMPLETION = 3};
// representation of an event
struct event
{
    float time;
    EventType type;
    struct event* next;
    int pid;
};

struct eventComparator {
    bool operator() (const event * left, const event * right) const {
        return left->time > right->time;
    }
};

#endif

