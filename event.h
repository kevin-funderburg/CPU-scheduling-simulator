/**
 * Representation of a event in the system.
 */

#ifndef EVENT_H
#define EVENT_H

enum EventType {ARRIVE = 1, DISPATCH = 2, DEPARTURE = 3, PREEMPT = 4};

struct event
{
    float time;
    EventType type;

    struct event *eq_next;
    struct procListNode *p_link;
};


class EventQueue
{ public:
    EventQueue() { eq_head = eq_tail = nullptr; }
    event* top();
    void pop();
    void push(event *);

private:
    event* eq_head, *eq_tail;
};

event* EventQueue::top() { return eq_head; }

void EventQueue::pop()
{
    event *tempPtr = eq_head;
    eq_head = eq_head->eq_next;
    delete tempPtr;
}

void EventQueue::push(event *newEvent)
{
    if (eq_head == nullptr)  //empty list
        eq_head = newEvent;
    else if (eq_head->time > newEvent->time)   //add to front
    {
        newEvent->eq_next = eq_head;
        eq_head = newEvent;
    }
    else
    {
        event *eq_cursor = eq_head;
        while (eq_cursor != nullptr)
        {
            if ((eq_cursor->time < newEvent->time) && (eq_cursor->eq_next == nullptr))  //add to tail
            {
                eq_cursor->eq_next = newEvent;
                break;
            }
            else if ((eq_cursor->time < newEvent->time) && (eq_cursor->eq_next->time > newEvent->time))   //add inside
            {
                newEvent->eq_next = eq_cursor->eq_next;
                eq_cursor->eq_next = newEvent;
                break;
            }
            else
                eq_cursor = eq_cursor->eq_next;
        }
    }
}

struct eventComparator {
    bool operator() (const event * left, const event * right) const {
        return left->time > right->time;
    }
};
#endif

