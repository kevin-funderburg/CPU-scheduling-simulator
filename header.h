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
priority_queue<event*,
        vector<event *, allocator<event*> >,
eventComparator> eventQueue;
typedef queue<process, list<process,
        allocator<process> > >
Pqueue;
Pqueue readyQ;
list <process> pList;
//event* head; // head of event queue
Scheduler scheduler;
int lambda,
        p_completed,        // end condition
        lastid;             // id of last process
float _clock,           // simulation clock
        avgArrivalTime,
        avgServiceTime;
bool CPUbusy;
cpuNode *cpuHead;
readyQNode *readyQHead;
procListNode *pHead;
////////////////////////////////////////////////////////////////


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

void schedule_event_eventQ(event *newEvent);

#endif //HEADER_H
