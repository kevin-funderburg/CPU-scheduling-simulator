/**
 * Representation of a event in the system.
 */
#ifndef EVENT_H
#define EVENT_H

enum EventType {PROCESS_CREATION = 0, DISPATCHED = 1, COMPLETION = 2, PREEMPTED = 3};

// representation of a task
class Event
{
public:
    Event(EventType, float);
    float time;
    EventType type;
};

#endif

