//
// Created by Kevin Funderburg on 2019-10-04.
//

#ifndef HEADER_H
#define HEADER_H

#define MAX_PROCESSES 10000

////////////////////////////////////////////////////////////////
enum State {READY = 0, RUNNING = 1, TERMINATED = 3};
enum Scheduler {_FCFS = 1, _SRTF = 2, _RR = 3};
////////////////////////////////////////////////////////////////
struct process
{
    int pid;                // process ID
    int arrivalTime;        // arrival time
    float burst;            // service time
    State state;            // process state
    float remainingTime;    // time left for execution
    // TODO - add stats
};

struct cpuNode
{
    float clock;
    bool cpuBusy;
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

/* Scheduling Algorithms */
void FCFS();
void SRTF();
void RR();
////////////////////////////////////////////////////////////////
// function definition
void parseArgs(char *[]);
void init();
int run_sim();
void generate_report();
//int schedule_event(struct event*);
float urand();
float genexp(float);
process newProcess(int);
event* newEvent(int, int, float);
int process_event2(struct event* eve);

void scheduleArrival();
void scheduleDeparture();
void scheduleAllocation();
void schedulePreemption();
void handleArrival();

void schedule_event_eventQ(event *newEvent);

#endif //HEADER_H
