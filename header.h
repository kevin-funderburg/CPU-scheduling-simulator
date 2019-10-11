/** \file header.h
    \brief header file for sim.cpp.

    Authors: Kevin Funderburg, Rob Murray
*/

#ifndef HEADER_H
#define HEADER_H

#include <deque>
#include <list>
#include <queue>

using namespace std;
#define MAX_PROCESSES 10000

enum State {READY = 0, RUNNING = 1, TERMINATED = 3};
enum Scheduler {_FCFS = 1, _SRTF = 2, _RR = 3};
enum EventType {ARRIVE = 1, DISPATCH = 2, DEPARTURE = 3, PREEMPT = 4};

struct procListNode
{
    int pid;
    float arrivalTime;
    float startTime;
    float reStartTime;
    float finishTime;
    float burst;
    float remainingTime;
    struct procListNode *pl_next;
};

struct CPU
{
    float clock;
    bool busy;
    struct procListNode *p_link;
};

struct readyQNode
{
    struct procListNode *p_link;
    struct readyQNode *rq_next;
};

struct eventQNode
{
    float time;
    EventType type;

    struct eventQNode *eq_next;
    struct procListNode *p_link;
};


class Queue
{ public:
    Queue() {eq_head = NULL;}	// if front is NULL, the queue is empty, whatever the value of rear may be
    eventQNode* top();
    void pop();
    void push(eventQNode *);

private:
    eventQNode* eq_head, *eq_tail;
};

eventQNode* Queue::top()
{
    return eq_head;
}
void Queue::pop()
{
    eventQNode *tempPtr = eq_head;
    eq_head = eq_head->eq_next;
    delete tempPtr;
}

void Queue::push(eventQNode *newEvent)
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
        eventQNode *eq_cursor = eq_head;
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
    bool operator() (const eventQNode * left, const eventQNode * right) const {
        return left->time > right->time;
    }
};
////////////////////////////////////////////////////////////////
// Global variables
//event* head; // head of event queue
//priority_queue<eventQNode*,
//        vector<eventQNode *, allocator<eventQNode*> >,
//        eventComparator> eventQ;    ///< priority queue of events
////
//
////deque<process> readyQ;
////list <process> pList;
////process p_table[MAX_PROCESSES + 200];


Scheduler scheduler;

// Global Variables
float totalTurnaroundTime;
float completionTime;
float cpuBusyTime;
float totalWaitingTime;

int lambda;
int lastid;
float avgServiceTime = 0.0;
float quantum;
float quantumClock;
Queue eventQ;
eventQNode *eq_head;
procListNode *pl_head;
procListNode *pl_tail;
readyQNode *rq_head;
CPU *cpu;

/* Scheduling Algorithms */
void FCFS();
void SRTF();
void RR();

void parseArgs(int, char *[]);
static void show_usage();
void init();
int run_sim();
void generate_report();
float urand();
float genexp(float);

float getAvgTurnaround();
float getTotalThroughput();
float cpuEstFinishTime();

void scheduleArrival();
void scheduleDeparture();
void scheduleDispatch();
void handleDispatch();
bool isPreemptive();
void schedulePreemption();
void handlePreemption();
void handleArrival();

void scheduleQuantumDispatch();
void handleQuantumDispatch();
void scheduleQuantumDeparture();
void handleQuantumDeparture();
void scheduleQuantumPreemption();
void handleQuantumPreemption();
float getNextQuantumDispatchTime();
float getNextQuantumClockTime();

procListNode *getSRTProcess();
procListNode *getHRRProcess();
float getResponseRatioValue(procListNode *);

void insertIntoEventQ(eventQNode *);
void popEventQHead();
void popReadyQHead();

#endif //HEADER_H
