/** \file header.h
    \brief header file for sim.cpp.

    Authors: Kevin Funderburg, Rob Murray
*/

#ifndef HEADER_H
#define HEADER_H

#include <deque>
#include <list>

using namespace std;
#define MAX_PROCESSES 100

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

struct cpuNode
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


////////////////////////////////////////////////////////////////
// Global variables
//event* head; // head of event queue
//priority_queue<event*,
//        vector<event *, allocator<event*> >,
//        eventComparator> eventQ;    ///< priority queue of events
//
//deque<process> readyQ;
//list <process> pList;
//process p_table[MAX_PROCESSES + 200];

Scheduler scheduler;

// Global Variables
float avgArrivalTime;
int lambda;
//float lambda;
int lastid;
float avgServiceTime;
float quantum;
float mu = 0.0;
float quantumClock;
eventQNode *eq_head;
procListNode *pl_head;
readyQNode *rq_head;
cpuNode *cpu_head;
int countSomething = 0;
///////////////////////////////////////////////////////////////

/* Scheduling Algorithms */
void FCFS();
void SRTF();
void RR();

// function definitions
void parseArgs(char *[]);
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
void schedulePreemption();
void handleArrival();

void scheduleQuantumDispatch();
void handleQuantumDispatch();
void scheduleQuantumDeparture();
void handleQuantumDeparture();
void scheduleQuantumPreemption();
void handleQuantumPreemption();
float getNextQuantumDispatchTime();
float getNextQuantumClockTime();

// Initializations
void insertIntoEventQ(eventQNode *);
void popEventQHead();
void popReadyQHead();

#endif //HEADER_H
