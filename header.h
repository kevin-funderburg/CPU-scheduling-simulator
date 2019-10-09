/** \file header.h
    \brief header file for sim.cpp.

    Authors: Kevin Funderburg, Rob Murray
*/

#ifndef HEADER_H
#define HEADER_H

#include <deque>
#include <list>
using namespace std;
#define MAX_PROCESSES 10000

////////////////////////////////////////////////////////////////
enum State {READY = 0, RUNNING = 1, TERMINATED = 3};
enum Scheduler {_FCFS = 1, _SRTF = 2, _RR = 3};
////////////////////////////////////////////////////////////////
//struct process
////{
////    int pid;                // process ID
////    State state;            // process state
////    float arrivalTime,      // arrival time
////          startTime,        // time started in CPU
////          reStartTime,      // time back in CPU
////          burst,            // service time
////          remainingTime,    // time remaining until completion
////          completionTime;   // process completion
////};

struct procListNode
{
    float arrivalTime;
    float startTime;
    float reStartTime;
    float finishTime;
    float burst;
    float remainingTime;
    struct procListNode *pNext;
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

struct eventQNode
{
    float time;
    int type;

    struct eventQNode *eNext;
    struct procListNode *pLink;
};

//struct procListNode
//{
//    process p;
//    struct procListNode *pNext;
//};


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

Scheduler schedulerType;

// Global Variables
float avgArrivalTime;
int lambda;
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

//int lambda,
//    p_completed,        // end condition
//    lastid;             // id of last process
//float _clock,           // simulation clock
//        avgArrivalTime,
//        avgServiceTime;
//bool CPUbusy;
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


void scheduleArrival();
void scheduleDeparture();
void scheduleDispatch();
void handleDispatch();
void schedulePreemption();
void handleArrival();
void debugging(event *newEvent);
void addToEventQ(event *newEvent);
// Initializations
void insertIntoEventQ(eventQNode *);
void popEventQHead();
void popReadyQHead();

#endif //HEADER_H
