/** \file header.h
    \brief header file for sim.cpp.

    Authors: Kevin Funderburg, Rob Murray
*/

#ifndef HEADER_H
#define HEADER_H

#include <deque>
#include <list>
using namespace std;
#define MAX_PROCESSES 50

////////////////////////////////////////////////////////////////
enum State {READY = 0, RUNNING = 1, TERMINATED = 3};
enum Scheduler {_FCFS = 1, _SRTF = 2, _RR = 3};
////////////////////////////////////////////////////////////////
struct process
{
    int pid;                // process ID
    State state;            // process state
    float arrivalTime,      // arrival time
          startTime,        // time started in CPU
          reStartTime,      // time back in CPU
          burst,            // service time
          remainingTime,    // time remaining until completion
          completionTime;   // process completion
};

struct cpuNode
{
    float clock;
    bool cpuBusy;
    int pid;
    struct procListNode *pLink;
};

struct readyQNode
{
    struct procListNode *pLink;
    struct readyQNode *rNext;
};

struct procListNode
{
    process p;
    struct procListNode *pNext;
};


////////////////////////////////////////////////////////////////
// Global variables
//event* head; // head of event queue
priority_queue<event*,
        vector<event *, allocator<event*> >,
        eventComparator> eventQueue;    ///< priority queue of events

deque<process> readyQ;
list <process> pList;
process p_table[MAX_PROCESSES + 200];

event *lastArrival;
Scheduler scheduler;

cpuNode *cpuHead;
readyQNode *readyQHead;
procListNode *pHead;

int lambda,
    p_completed,        // end condition
    lastid;             // id of last process
float _clock,           // simulation clock
        avgArrivalTime,
        avgServiceTime;
bool CPUbusy;
////////////////////////////////////////////////////////////////


/* Scheduling Algorithms */
void FCFS();
void SRTF();
void RR();
////////////////////////////////////////////////////////////////
// function definitions
void parseArgs(char *[]);
void init();
int run_sim();
void generate_report();
float urand();
float genexp(float);
process newProcess(int);
event* newEvent(int, int, float);

void scheduleArrival();
void scheduleDeparture();
void scheduleAllocation();
void handleAllocation();
void schedulePreemption();
void handleArrival();

void addToEventQ(event *newEvent);

#endif //HEADER_H
