/** \file header.h
    \brief header file for sim.cpp.

    Authors: Kevin Funderburg, Rob Murray
*/

#ifndef HEADER_H
#define HEADER_H

#include "event.h"

using namespace std;
#define MAX_PROCESSES 10000

enum Scheduler {_FCFS = 1, _SRTF = 2, _RR = 3};

struct process
{
    int pid;
    float arrivalTime;
    float startTime;
    float reStartTime;
    float finishTime;
    float burst;
    float remainingTime;
    struct process *pl_next;
};

struct CPU
{
    float clock;
    bool busy;
    struct process *p_link;
};

//class CPU
//{ public:
//    CPU() {
//        clock = 0.0;
//        busy = false;
//        p_link = nullptr;
//    };
//    float clock;
//    bool busy;
//    struct process *p_link;
//    float get_est_finish_time();
//};

//float CPU::get_est_finish_time()
//{
//    float est_finish;
//    float startTime = p_link->startTime;
//    float reStartTime = p_link->reStartTime;
//    float remainingTime = p_link->remainingTime;
//
//    est_finish = (reStartTime == 0 ? startTime : reStartTime) + remainingTime;
//
//    return est_finish;
//}

struct Ready
{
    struct process *p_link;
    struct Ready *rq_next;
};

class ReadyQueue
{ public:
    ReadyQueue() { rq_head = rq_tail = nullptr; }
    Ready* top();
    void pop();
    void push(Ready *);
    bool empty();
    process *get_srt();
    Ready* rq_head, *rq_tail;
};

Ready* ReadyQueue::top() { return rq_head; }

void ReadyQueue::pop()
{
    Ready *tempPtr = rq_head;
    rq_head = rq_head->rq_next;
    delete tempPtr;
}

bool ReadyQueue::empty() { return rq_head == nullptr; }

void ReadyQueue::push(Ready *newReady)
{
    if (rq_head == nullptr)  //empty queue
        rq_head = newReady;
    else
    {
        Ready *rq_cursor = rq_head;
        while (rq_cursor->rq_next != nullptr)
            rq_cursor = rq_cursor->rq_next;
        rq_cursor->rq_next = newReady;
    }
}

process* ReadyQueue::get_srt()
{
    Ready *rq_cursor = rq_head;
    process *srtProc = rq_cursor->p_link;
    float srt = rq_cursor->p_link->remainingTime;
    while (rq_cursor != nullptr)
    {
        if (rq_cursor->p_link->remainingTime < srt)
        {
            srt = rq_cursor->p_link->remainingTime;
            srtProc = rq_cursor->p_link;
        }
        rq_cursor = rq_cursor->rq_next;
    }
    return srtProc;
}



Scheduler scheduler;

// Global Variables
float totalTurnaroundTime;
float completionTime;
float cpuBusyTime;
float totalWaitingTime;
float avgServiceTime = 0.0;
float quantum;
float quantumClock;
int lambda;

EventQueue eventQ;
ReadyQueue readyQ;
event *eq_head;
process *pl_head;
process *pl_tail;
Ready *rq_head;
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

void sched_arrival();
void arrival();
void sched_dispatch();
void dispatch();
void sched_depart();

bool does_preempt();
void sched_preempt();
void preempt();

void sched_q_dispatch();
void q_dispatch();
void sched_q_depart();
void q_depart();
void sched_q_preempt();
void q_preempt();
float get_next_q_dispatch();
float get_next_q_clock();

//process *getSRTProcess();

void popReadyQHead();

#endif //HEADER_H
