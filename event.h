/**
 * Representation of a event in the system.
 */
#ifndef EVENT_H
#define EVENT_H

enum EventType {PROCESS_CREATION = 0, DISPATCHED = 1, COMPLETION = 2, PREEMPTED = 3};

// representation of a task
class event
{
public:
    event(EventType, float);
    // Execute event by invoking this method.
    virtual void processEvent() = 0;
    // data type
    float time;
    EventType type;
};

struct eventComparator {
    bool operator() (const event * left, const event * right) const {
        return left->time > right->time;
    }
};

event::event(EventType evetype, float t)
        : type(evetype), time(t)
{

}

#endif

