/**
 * Representation of a event in the system.
 */

#ifndef EVENT_H
#define EVENT_H

// representation of a task
struct event {
    float time;
    int   type;
    int   pid;
    float remainingTime;
    float burst;  // service time
    struct event* next;
};

#endif

