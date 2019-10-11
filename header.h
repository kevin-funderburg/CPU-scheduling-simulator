/** \file header.h
    \brief header file for sim.cpp.

    Authors: Kevin Funderburg, Rob Murray
*/

#ifndef HEADER_H
#define HEADER_H

//#include <deque>
//#include <list>
//#include <queue>
#include "event.h"

using namespace std;
#define MAX_PROCESSES 10000

enum State {READY = 0, RUNNING = 1, TERMINATED = 3};
enum Scheduler {_FCFS = 1, _SRTF = 2, _RR = 3};

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

EventQueue eventQ;
event *eq_head;
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

//void insertIntoEventQ(event *);
//void popEventQHead();
void popReadyQHead();

#endif //HEADER_H
