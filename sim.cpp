/***
 * CPU Scheduler Simulation
 * @file sim.cpp
 * @authors: Kevin Funderburg, Rob Murray
 *
 * In this project, we are going to build a discrete-time event simulator
 * for a number of CPU scheduling algorithms on a single CPU system.
 * The goal of this project is to compare and assess the impact of
 * different scheduling algorithms on different performance metrics,
 * and across multiple workloads.
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include "header.h"
using namespace std;

void parseArgs(int argc, char *argv[])
{
    if (argc < 3)
    {
        show_usage();
        return;
    }
    if (*argv[2] == 3 && argc < 5)      // _RR requires the quantum arg
    {
        cerr << "Expected 4 arguments and got " << argc << endl;
        show_usage();
        return;
    }
    scheduler = static_cast<Scheduler>(stoi(argv[1]));
    lambda = atoi(argv[2]);
    avgServiceTime = (float)atof(argv[3]);
    if (argc == 5) quantum = (float)atof(argv[4]);
}

static void show_usage()
{
    cerr << "Usage: sim [123] [average arrival rate] [average service time] [quantum interval]\n\n"
         << "Options:\n"
         << "\t1 : First-Come First-Served (_FCFS)\n"
         << "\t2 : Shortest Remaining Time First (_SRTF)\n"
         << "\t3 : Round Robin, with different quantum values (_RR) (requires 4 arguments)\n";
}

void init()
{
    cout << "lambda: " << lambda << endl;
    quantumClock = 0.0;
    avgServiceTime = (float)1.0/avgServiceTime;

    cpu = new CPU;
    cpu->clock = 0.0;
    cpu->busy = false;
    cpu->p_link = nullptr;

    pl_head = new process;
    pl_head->pid = 0;
    pl_head->arrivalTime = genexp((float)lambda);
    pl_head->startTime = 0.0;
    pl_head->reStartTime = 0.0;
    pl_head->finishTime = 0.0;
    pl_head->burst = genexp(avgServiceTime);
    pl_head->remainingTime = pl_head->burst;
    pl_head->pl_next = nullptr;
    pl_tail = pl_head;

    eq_head = new event;
    eq_head->time = pl_head->arrivalTime;
    eq_head->type = ARRIVE;
    eq_head->eq_next = nullptr;
    eq_head->p_link = pl_head;

    readyQ = ReadyQueue();
    eventQ = EventQueue();
    eventQ.push(eq_head);
}
////////////////////////////////////////////////////////////////
void generate_report()
{
    float avgTurnaroundTime = totalTurnaroundTime / (float)MAX_PROCESSES;
    float totalThroughput = (float)MAX_PROCESSES / completionTime;
    float cpuUtil = cpuBusyTime / completionTime;
    float avgWaitingTime = totalWaitingTime / (float)MAX_PROCESSES;
    float avgQlength = 1.0/(float)lambda * avgWaitingTime;
    string _scheduler;
    switch (scheduler) {
        case _FCFS: _scheduler = "FCFS"; break;
        case _SRTF: _scheduler = "SRTF"; break;
        case _RR: _scheduler = "RR"; break;
    }
    ofstream data("sim.data",  ios::out | ios::app);    //output to text file for viewing
    ofstream xcel("sim.csv",  ios::out | ios::app);     //output to csv file for graphing in excel
    if (data.is_open() && xcel.is_open())
    {
        int w = 20;
        if (lambda == 1)
        {
            data << setfill('-') << setw(w*5) << "\n"
                    << setfill(' ')
                    << setw(w/2) << "Scheduler"
                    << setw(w/2) << "lambda"
                    << setw(w) << "AvgTurnaround"
                    << setw(w) << "Throughput"
                    << setw(w) << "CPU Utilization"
                    << setw(w) << "AvgReadyQ\n"
                    << setfill('-') << setw(w*5) << "\n";

            cout << setfill('-') << setw(w*5) << "\n"
                 << setfill(' ')
                 << setw(w/2) << "Scheduler"
                 << setw(w/2) << "lambda"
                 << setw(w) << "AvgTurnaround"
                 << setw(w) << "Throughput"
                 << setw(w) << "CPU Utilization"
                 << setw(w) << "AvgReadyQ\n"
                 << setfill('-') << setw(w*5) << "\n";

            xcel << "Scheduler,lambda,AvgTurnaround,Throughput,CPU Utilization,AvgReadyQ\n";
        }
        data << setfill(' ')
             << setw(w/2) << _scheduler
             << setw(w/2) << lambda
             << setw(w) << avgTurnaroundTime
             << setw(w) << totalThroughput
             << setw(w) << cpuUtil
             << setw(w) << avgQlength << endl;

        xcel << _scheduler << ","
             << lambda << ","
             << avgTurnaroundTime << ","
             << totalThroughput << ","
             << cpuUtil << ","
             << avgQlength << ","
             << endl;

        data.close();
    }
    else cout << "Unable to open file";
}
////////////////////////////////////////////////////////////////
/// @return a random number between 0 and 1
float urand() { return (float) rand() / RAND_MAX; }
/////////////////////////////////////////////////////////////
/// @return a random number that follows an exp distribution
float genexp(float lambda)
{
    float u,x;
    x = 0;
    while (x == 0)
    {
        u = urand();
        x = (-1/lambda)*log(u);
    }
    return(x);
}
/***
 * Create new process and schedule an arrival event for the next process
 */
void sched_arrival()
{
    process *pl_cursor = pl_tail;
    pl_cursor->pl_next = new process;
    pl_cursor->pl_next->pid = pl_cursor->pid + 1;
    pl_cursor->pl_next->arrivalTime = pl_cursor->arrivalTime + genexp((float)lambda);
    pl_cursor->pl_next->startTime = 0.0;
    pl_cursor->pl_next->reStartTime = 0.0;
    pl_cursor->pl_next->finishTime = 0.0;
    pl_cursor->pl_next->burst = genexp(avgServiceTime);
    pl_cursor->pl_next->remainingTime = pl_cursor->pl_next->burst;
    pl_cursor->pl_next->pl_next = nullptr;
    pl_tail = pl_tail->pl_next;

    event *arrival = new event;
    arrival->type = ARRIVE;
    arrival->time = pl_cursor->pl_next->arrivalTime;    //set arrival time of the next process
    arrival->p_link = pl_cursor->pl_next;   //link process to the arrival event
    arrival->eq_next = nullptr;

    eventQ.push(arrival);   //add to event queue
}

/***
 * Push process into ready queue and remove arrival event from event queue
 */
void arrival()
{
    Ready *ready = new Ready;
    ready->p_link = eventQ.top()->p_link;   //link process at head of event queue to ready queue
    ready->rq_next = nullptr;
    readyQ.push(ready);
    eventQ.pop();
}

/**
 * Schedule a process to be attached to CPU for processing
 */
void sched_dispatch()
{
    event *dispatch = new event;
    process *nextProc;
    if (scheduler == _FCFS)
        nextProc = readyQ.top()->p_link;
    else if (scheduler == _SRTF)
    {
        if (cpu->clock > readyQ.top()->p_link->arrivalTime)
            //SRTF will traverse the process list to locate the
            //process with the shortest remaining time
            nextProc = readyQ.get_srt();
        else
            nextProc = readyQ.top()->p_link;
    }

    dispatch->time = cpu->clock < nextProc->arrivalTime ?
            nextProc->arrivalTime : cpu->clock;

    dispatch->type = DISPATCH;
    dispatch->eq_next = nullptr;
    dispatch->p_link = nextProc;

    eventQ.push(dispatch);
}

/**
 * Take a process off the ready queue and give to the CPU for processing
 */
void dispatch()
{
    //Link the process at head of event queue to the CPU
    cpu->p_link = eventQ.top()->p_link;

    if (scheduler == _SRTF || scheduler == _RR)
    {
        Ready *rq_cursor = readyQ.top()->rq_next;
        Ready *rq_precursor = readyQ.top();
        if (rq_precursor->p_link->arrivalTime != eventQ.top()->p_link->arrivalTime)
        {
            while (rq_cursor != nullptr)
            {
                //if the ready process arrival time is eqal to the arrival time od
                //the current dispatch event
                if (rq_cursor->p_link->arrivalTime == eventQ.top()->p_link->arrivalTime)
                {
                    rq_precursor->rq_next = rq_cursor->rq_next;
                    rq_cursor->rq_next = readyQ.top();
                    readyQ.rq_head = rq_cursor;
                    break;
                }
                rq_cursor = rq_cursor->rq_next;
                rq_precursor = rq_precursor->rq_next;
            }
        }
    }

    readyQ.pop();
    eventQ.pop();

    cpu->busy = true;    //CPU now executing process

    //set clock to time of process arrival
    if (cpu->clock < cpu->p_link->arrivalTime)
        cpu->clock = cpu->p_link->arrivalTime;

    if (cpu->p_link->startTime == 0)
        cpu->p_link->startTime = cpu->clock;
    else
        cpu->p_link->reStartTime = cpu->clock;
}

/**
 * Add a departure event to the event queue
 */
void sched_depart()
{
    event *departure = new event;
    departure->type = DEPARTURE;
    departure->eq_next = nullptr;
    departure->p_link = cpu->p_link;    //link process on CPU to the departure event

    //FCFS will process to completion, so its departure time will be the completion time
    if (scheduler == _FCFS || scheduler == _RR)
        departure->time = cpu->p_link->startTime + cpu->p_link->remainingTime;

    else if (scheduler == _SRTF)
    {
        if (cpu->p_link->reStartTime == 0)
            //First time the process has been through the CPU
            departure->time = cpu->p_link->startTime + cpu->p_link->remainingTime;
        else
            //Process has been preempted before so time must be calculated differently
            departure->time = cpu->p_link->reStartTime + cpu->p_link->remainingTime;
    }

    eventQ.push(departure);
}

void departure()
{
    cpu->clock = eventQ.top()->time;        //set clock to time of event at head of event queue
    cpu->p_link->finishTime = cpu->clock;   //set finish time of process to be departed to current time

    cpu->p_link->remainingTime = 0.0;       //clear remainingTime

    //set metric calculation variables
    cpuBusyTime += cpu->p_link->burst;
    totalTurnaroundTime += (cpu->p_link->finishTime - cpu->p_link->arrivalTime);
    completionTime = cpu->p_link->finishTime;
    totalWaitingTime += (cpu->p_link->finishTime - cpu->p_link->arrivalTime - cpu->p_link->burst);

    cpu->p_link = nullptr;
    cpu->busy = false;   //CPU Ready for next process

    eventQ.pop();
}

/***
 * Preemption will occur when the new event's time occurs BEFORE
 * the cpu completes its execution, AND the remaining time of the
 * event's process to complete is shorter than than the remaining time
 * of the current process to complete
 * */
bool does_preempt()
{
    float cpu_fin_time = estimate_fin_time();
    float cpu_remaining_time = cpu_fin_time - eventQ.top()->time;

    return (eventQ.top()->time < cpu_fin_time) && (eventQ.top()->p_link->remainingTime < cpu_remaining_time);
}

/**
 * Schedule a preemption event
 */
void sched_preempt()
{
    event *preemption = new event;
    preemption->time = eventQ.top()->time;
    preemption->type = PREEMPT;
    preemption->eq_next = nullptr;
    preemption->p_link = eventQ.top()->p_link;  //attach process at head of event queue to preemption event

    eventQ.pop();
    eventQ.push(preemption);
}

void preemption()
{
    process *preempt_process = cpu->p_link;

    cpu->p_link->remainingTime =
            estimate_fin_time() - eventQ.top()->time;

    cpu->p_link = eventQ.top()->p_link;     //attach process at event queue to CPU
    cpu->clock = eventQ.top()->time;        //advance the clock to time of event
    if (cpu->p_link->reStartTime == 0.0)
        cpu->p_link->startTime = eventQ.top()->time;
    else
        cpu->p_link->reStartTime = eventQ.top()->time;

    event *preempt_arrival = new event;
    preempt_arrival->time = eventQ.top()->time;     //set time of preemption
    preempt_arrival->type = ARRIVE;
    preempt_arrival->eq_next = nullptr;
    preempt_arrival->p_link = preempt_process;      //link preempted process to preempt arrival event

    eventQ.pop();
    eventQ.push(preempt_arrival);
}

/*
 * Start simulation
 */
int run_sim()
{
    cout << "Starting simulation using scheduler: ";
    switch (scheduler)
    {
        case _FCFS: cout << "FCFS\n\n"; FCFS(); break;
        case _SRTF: cout << "SRTF\n\n"; SRTF(); break;
        case _RR: cout << "RR\n\n"; RR(); break;
        default: cerr << "invalid scheduler\n"; return 1;
    }
    return 0;
}

/**
 * First Come First Serve
 */
void FCFS()
{
    int arrivalCount = 0;
    int departureCount = 0;
    int dispatchCount = 0;

    while (departureCount < MAX_PROCESSES)
    {
        if (!cpu->busy)
        {
            sched_arrival();
            if (readyQ.top() != nullptr) sched_dispatch();
        }
        else
            sched_depart();

        switch (eventQ.top()->type)
        {
            case ARRIVE:
                arrival();
                arrivalCount++;
                break;
            case DISPATCH:
                dispatch();
                dispatchCount++;
                break;
            case DEPARTURE:
                departure();
                departureCount++;
                break;
            default: cerr << "invalid event type\n";
        }
    }
}

/**
 * Shortest Remaining Task First
 */
void SRTF()
{
    int arrivalCount = 0;
    int departureCount = 0;
    int allocationCount = 0;

    while (departureCount < MAX_PROCESSES)
    {
        if (arrivalCount < (MAX_PROCESSES * 1.20))
        {
            sched_arrival();
            arrivalCount++;
        }
        if (!cpu->busy)
        {
            if (readyQ.top() != nullptr) sched_dispatch();
        }
        else
        {
            if (eventQ.top()->type == ARRIVE)
            {
                //If the current event's time occurs after the process
                //currently on the CPU completes, it won't be preempted so it can
                //be scheduled normally
                if (eventQ.top()->time > estimate_fin_time())
                    sched_depart();
                else if (does_preempt())
                    sched_preempt();
            }
        }

        switch (eventQ.top()->type)
        {
            case ARRIVE:
                arrival();
                break;
            case DISPATCH:
                dispatch();
                allocationCount++;
                break;
            case DEPARTURE:
                departure();
                departureCount++;
                break;
            case PREEMPT:
                preemption();
                break;
            default: cerr << "invalid event type\n";
        }

    }
}

/**
 * Round Robin
 */
void RR()
{
    int arrivalCount = 0;
    int departureCount = 0;
    while (departureCount < MAX_PROCESSES)
    {
        if (arrivalCount < (MAX_PROCESSES * 1.20))
        {
            sched_arrival();
            arrivalCount++;
        }
        if (!cpu->busy)
        {
            sched_arrival();
            if (readyQ.top() != nullptr) sched_q_dispatch();
        }
        else
        {
            float estCompletionTime = 0;
            estCompletionTime = cpu->p_link->reStartTime == 0 ?
                    cpu->p_link->remainingTime : cpu->p_link->startTime;

            if (estCompletionTime < get_next_q_clock())
                sched_q_depart();
            else
            {
                if (readyQ.top() != nullptr)
                {
                    if (readyQ.top()->p_link->arrivalTime > estCompletionTime)
                        sched_q_depart();
                    else
                        sched_q_preempt();
                }
            }
        }
        switch (eventQ.top()->type)
        {
            case ARRIVE:
                arrival();
                break;
            case DISPATCH:
                q_dispatch();
                break;
            case DEPARTURE:
                q_depart();
                departureCount++;
                if (readyQ.top() != nullptr && (readyQ.top()->p_link->arrivalTime < cpu->clock))
                    sched_q_dispatch();
                break;
            case PREEMPT:
                q_preempt();
                break;
            default: cerr << "invalid event type\n";
        }
    }
    }

/**
 * Schedule quantum dispatch event
 */
void sched_q_dispatch()
{
    event *dispatch = new event;
    process *nextProc;
    nextProc = readyQ.top()->p_link;

    if (readyQ.top() != nullptr)
    {
        //advance clock to arrival time to simulate time progression
        if (readyQ.top()->p_link->arrivalTime < cpu->clock)
            dispatch->time = cpu->clock;
        else
        {
            cpu->clock = readyQ.top()->p_link->arrivalTime;
            //determine the time when the process will be preempted by the quantum
            float nextQuantumTime = quantumClock;
            while (nextQuantumTime < cpu->clock)
                nextQuantumTime += quantum;
            quantumClock = nextQuantumTime;

            dispatch->time = get_next_q_dispatch();
        }
    }
    else cerr << "Error in sched_q_dispatch()\n";

    dispatch->type = DISPATCH;
    dispatch->eq_next = nullptr;
    dispatch->p_link = nextProc;

    eventQ.push(dispatch);
}

/**
 * Perform quantum dispatch
 */
void q_dispatch()
{
    cpu->p_link = eventQ.top()->p_link;
    cpu->busy = true;

    if (cpu->p_link->startTime == 0)
        cpu->p_link->startTime = eventQ.top()->time;
    else
        cpu->p_link->reStartTime = eventQ.top()->time;

    readyQ.pop();
    eventQ.pop();
}

/**
 * Schedule quantum departure event
 */
void sched_q_depart()
{
    event *departure = new event;
    departure->type = DEPARTURE;
    departure->eq_next = nullptr;
    departure->p_link = cpu->p_link;    //link cpu process to departure event

    if (cpu->p_link->reStartTime == 0)
        departure->time = cpu->p_link->startTime + cpu->p_link->remainingTime;
    else
        departure->time = cpu->p_link->reStartTime + cpu->p_link->remainingTime;

    eventQ.push(departure);
}

/**
 * Quantum departure
 */
void q_depart()
{
    cpu->p_link->finishTime = eventQ.top()->time;   //set finish time of processing
    cpu->p_link->remainingTime = 0.0;
    cpu->clock = eventQ.top()->time;    //advance clock
    cpu->busy = false;
    //get metric calculations
    cpuBusyTime += cpu->p_link->burst;
    totalTurnaroundTime += (cpu->p_link->finishTime - cpu->p_link->arrivalTime);
    completionTime = cpu->p_link->finishTime;
    totalWaitingTime += (cpu->p_link->finishTime - cpu->p_link->arrivalTime - cpu->p_link->burst);

    cpu->p_link = nullptr;
    eventQ.pop();
}

/**
 * Schedule quantum preempt event
 */
void sched_q_preempt()
{
    event *preemption = new event;
    preemption->type = PREEMPT;
    preemption->eq_next = nullptr;

    cpu->clock = readyQ.top()->p_link->arrivalTime; //advance clock

    float nextQuantumTime = quantumClock;
    while (nextQuantumTime < cpu->clock)
        nextQuantumTime += quantum;

    quantumClock = nextQuantumTime;
    preemption->time = get_next_q_clock();
    preemption->p_link = readyQ.top()->p_link;
    eventQ.push(preemption);
}

/**
 * Perform quantum peremption
 */
void q_preempt()
{
    process *preempted_pr = cpu->p_link;

    cpu->p_link->remainingTime = estimate_fin_time() - eventQ.top()->time;

    cpu->p_link = eventQ.top()->p_link;
    cpu->clock = eventQ.top()->time;

    cpu->p_link->startTime == 0.0 ? cpu->p_link->startTime : cpu->p_link->reStartTime = eventQ.top()->time;

    float nextQuantumTime = quantumClock;
    while (nextQuantumTime < eventQ.top()->time)
        nextQuantumTime += quantum;

    quantumClock = nextQuantumTime;

    event *preempted_p_arrival = new event;
    preempted_p_arrival->time = eventQ.top()->time;
    preempted_p_arrival->type = ARRIVE;
    preempted_p_arrival->eq_next = nullptr;
    preempted_p_arrival->p_link = preempted_pr;
    
    eventQ.pop();
    readyQ.pop();
    eventQ.push(preempted_p_arrival);

}

/**
 * Get the value of the next quantum clock
 * @return quantum
 */
float get_next_q_clock(){ return quantumClock + quantum; }

float get_next_q_dispatch()
{
    float nextQuantumTime = quantumClock;
    while (nextQuantumTime < readyQ.top()->p_link->arrivalTime)
        nextQuantumTime += quantum;

    return nextQuantumTime;
}

float estimate_fin_time()
{
    float est_fin_time;
    float start = cpu->p_link->startTime;
    float reStart = cpu->p_link->reStartTime;
    float remaining = cpu->p_link->remainingTime;

    est_fin_time = (reStart == 0 ? start : reStart) + remaining;
    return est_fin_time;
}
///////////////////////////////////////////////////////////////
int main(int argc, char *argv[] )
{
    parseArgs(argc, argv);
    init();
    run_sim();
    generate_report();
    return 0;
}


