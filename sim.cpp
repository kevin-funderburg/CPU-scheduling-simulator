/***
 * CPU Scheduler Simulation
 * @file sim.cpp
 * @authors: Kevin Funderburg, Rob Murray
 */

/////////////////////////////////////////////////
// NOTES
// avgnumprocesses 1 / lambda * num_proceesses i think
// Use 105 ms for the preemptive _SRTF
/////////////////////////////////////////////////
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <list>
#include <queue>
//#include "event.h"
#include "header.h"
using namespace std;
/////////////////////////////////////////////////
void parseArgs(int argc, char *argv[])
{
    scheduler = static_cast<Scheduler>(stoi(argv[1]));  // set scheduler
    lambda = stoi(argv[2]);
    avgServiceTime = stof(argv[3]);
    if (argc == 5) quantum = stof(argv[4]);
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
    lastid = 0;

    avgArrivalTime = 1.0/(float)lambda;

    cpu = new CPU;
    cpu->clock = 0.0;
    cpu->busy = false;
    cpu->p_link = nullptr;

    pl_head = new procListNode;
    pl_head->pid = lastid;
    pl_head->arrivalTime = genexp(lambda);
    pl_head->startTime = 0.0;
    pl_head->reStartTime = 0.0;
    pl_head->finishTime = 0.0;
    pl_head->burst = genexp(avgServiceTime);
    pl_head->remainingTime = pl_head->burst;
    pl_head->pl_next = nullptr;
    pl_tail = pl_head;

    eq_head = new eventQNode;
    eq_head->time = pl_head->arrivalTime;
    eq_head->type = ARRIVE;
    eq_head->eq_next = nullptr;
    eq_head->p_link = pl_head;
}

////////////////////////////////////////////////////////////////
void generate_report()
{
    float avgTurnaroundTime = totalTurnaroundTime / MAX_PROCESSES;
    float totalThroughput = MAX_PROCESSES / completionTime;
    float cpuUtil = cpuBusyTime / completionTime;
    float avgWaitingTime = totalWaitingTime / MAX_PROCESSES;
    float avgQlength = 1.0/(float)lambda * avgWaitingTime;
    string _scheduler;
    switch (scheduler) {
        case _FCFS: _scheduler = "FCFS"; break;
        case _SRTF: _scheduler = "SRTF"; break;
        case _RR: _scheduler = "RR"; break;
    }
    ofstream data("sim.data",  ios::out | ios::app);
    ofstream xcel("sim.csv",  ios::out | ios::app);
    if (data.is_open())
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


            xcel << "Scheduler,lambda,AvgTurnaround,Throughput,CPU Utilization,AvgReadyQ\n";
//            data << "-------------------------------------------------------------------------------------------\n"
//                 << "Scheduler\tlambda\tAvgTurnaround\tThroughput\t\tCPU Utilization\t\tAvgReadyQ\n"
//                 << "-------------------------------------------------------------------------------------------\n";
        }
//        data << _scheduler << "\t\t" << lambda << "\t\t" << avgTurnaroundTime << "\t\t\t" << totalThroughput << "\t\t" << cpuUtil << "\t\t\t" << avgQlength << endl;
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
float urand() { return( (float) rand()/RAND_MAX ); }

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

void scheduleArrival()
{
    procListNode *pl_cursor = pl_tail;
    int count = 0;
    while (pl_cursor->pl_next != nullptr)
    {
        count++;
        pl_cursor = pl_cursor->pl_next;
    }

    pl_cursor->pl_next = new procListNode;
    pl_cursor->pl_next->pid = pl_cursor->pid + 1;
    pl_cursor->pl_next->arrivalTime = pl_cursor->arrivalTime + genexp(avgArrivalTime);
    pl_cursor->pl_next->startTime = 0.0;
    pl_cursor->pl_next->reStartTime = 0.0;
    pl_cursor->pl_next->finishTime = 0.0;
    pl_cursor->pl_next->burst = genexp(avgServiceTime);
    pl_cursor->pl_next->remainingTime = pl_cursor->pl_next->burst;
    pl_cursor->pl_next->pl_next = nullptr;
    pl_tail = pl_tail->pl_next;

    eventQNode *arrival = new eventQNode;
    arrival->time = pl_cursor->pl_next->arrivalTime;
    arrival->type = ARRIVE;
    arrival->p_link = pl_cursor->pl_next;
    arrival->eq_next = nullptr;

    insertIntoEventQ(arrival);
}

//Push process into ready queue and remove event from event queue
void handleArrival()
{
    readyQNode *ready = new readyQNode;
    ready->p_link = eq_head->p_link;
    ready->rq_next = nullptr;

    if (rq_head == nullptr)  //empty queue
        rq_head = ready;
    else
    {
        readyQNode *rq_cursor = rq_head;
        while (rq_cursor->rq_next != nullptr)
            rq_cursor = rq_cursor->rq_next;

        rq_cursor->rq_next = ready;
    }
    popEventQHead();
}

void scheduleDispatch()
{
    eventQNode *dispatch = new eventQNode;

    procListNode *nextProc;
    if (scheduler == _FCFS)
        nextProc = rq_head->p_link;
    else if (scheduler == _SRTF)
    {
        if (cpu->clock > rq_head->p_link->arrivalTime)
            //SRTF will traverse the process list to locate the
            //process with the shortest remaining time
            nextProc = getSRTProcess();
        else
            nextProc = rq_head->p_link;
    }
    else if (scheduler == _RR)
        nextProc = getHRRProcess();

    dispatch->time = cpu->clock < nextProc->arrivalTime ?
            nextProc->arrivalTime : cpu->clock;

    dispatch->type = DISPATCH;
    dispatch->eq_next = nullptr;
    dispatch->p_link = nextProc;

    insertIntoEventQ(dispatch);
}

/*
 * Take a process off the ready queue and give to the CPU
 * for processing
 */
void handleDispatch()
{
    //Link the process from event queue to the CPU
    cpu->p_link = eq_head->p_link;

    if (scheduler == _SRTF || scheduler == _RR)
    {
        readyQNode *rq_cursor = rq_head->rq_next;
        readyQNode *rq_precursor = rq_head;
        if (rq_precursor->p_link->arrivalTime != eq_head->p_link->arrivalTime)
        {
            while (rq_cursor != nullptr)
            {
                if (rq_cursor->p_link->arrivalTime == eq_head->p_link->arrivalTime)
                {
                    rq_precursor->rq_next = rq_cursor->rq_next;
                    rq_cursor->rq_next = rq_head;
                    rq_head = rq_cursor;
                    break;
                }
                rq_cursor = rq_cursor->rq_next;
                rq_precursor = rq_precursor->rq_next;
            }
        }
    }

    popReadyQHead();
    popEventQHead();

    cpu->busy = true;    //CPU now busy executing process

    //set clock to time of process arrival
    if (cpu->clock < cpu->p_link->arrivalTime)
        cpu->clock = cpu->p_link->arrivalTime;

    if (cpu->p_link->startTime == 0)
        cpu->p_link->startTime = cpu->clock;
    else
        cpu->p_link->reStartTime = cpu->clock;
}

/*
 * Add a departure event to the event queue
 */
void scheduleDeparture()
{
    eventQNode *departure = new eventQNode;
    departure->type = DEPARTURE;
    departure->eq_next = nullptr;
    departure->p_link = cpu->p_link;    //link process on CPU to the departure event

    if (scheduler == _FCFS || scheduler == _RR)
        //FCFS will process to completion, so its departure time will
        //be the completion time
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

    insertIntoEventQ(departure);
}

void handleDeparture()
{
    cpu->clock = eq_head->time;   //set clock to time of event at head of event queue

    cpu->p_link->finishTime = cpu->clock;   //set finish time of process to be departed to current time
//    pl_head->finishTime = cpu->p_link->finishTime;

    cpu->p_link->remainingTime = 0.0;    //clear remainingTime

    cpuBusyTime += cpu->p_link->burst;
    totalTurnaroundTime += (cpu->p_link->finishTime - cpu->p_link->arrivalTime);
    completionTime = cpu->p_link->finishTime;
    totalWaitingTime += (cpu->p_link->finishTime - cpu->p_link->arrivalTime - cpu->p_link->burst);

    cpu->p_link = nullptr;
    cpu->busy = false;   //CPU ready for next process

    popEventQHead();    //remove departure event
}

/***
 * Preemption will occur when the new event's time occurs BEFORE
 * the cpu completes its execution, AND the remaining time of the
 * event's process to complete is shorter than than the remaining time
 * of the current process to complete
 * */
bool isPreemptive()
{
    float cpuFinishTime = cpuEstFinishTime();
    float cpuRemainingTime = cpuFinishTime - eq_head->time;

    return (eq_head->time < cpuFinishTime) && (eq_head->p_link->remainingTime < cpuRemainingTime);
}

void schedulePreemption()
{
    eventQNode *preemption = new eventQNode;
    preemption->time = eq_head->time;
    preemption->type = PREEMPT;
    preemption->eq_next = nullptr;
    preemption->p_link = eq_head->p_link;

    popEventQHead();

    insertIntoEventQ(preemption);
}

void handlePreemption()
{
    procListNode *preemptedProcPtr = cpu->p_link;

    cpu->p_link->remainingTime =
            cpuEstFinishTime() - eq_head->time;

    cpu->p_link = eq_head->p_link;
    cpu->clock = eq_head->time;
    if (cpu->p_link->reStartTime == 0.0)
        cpu->p_link->startTime = eq_head->time;
    else
        cpu->p_link->reStartTime = eq_head->time;

    eventQNode *preemptedProcArrival = new eventQNode;
    preemptedProcArrival->time = eq_head->time;
    preemptedProcArrival->type = ARRIVE;
    preemptedProcArrival->eq_next = nullptr;
    preemptedProcArrival->p_link = preemptedProcPtr;

    popEventQHead();

    insertIntoEventQ(preemptedProcArrival);
}

////////////////////////////////////////////////////////////
int run_sim()
{
    cout << "Starting simulation with using scheduler: ";
    switch (scheduler)
    {
        case _FCFS: cout << "FCFS\n\n"; FCFS(); break;
        case _SRTF: cout << "SRTF\n\n"; SRTF(); break;
        case _RR: cout << "RR\n\n"; RR(); break;
        default: cerr << "invalid scheduler\n"; return 1;
    }
    return 0;
}

void FCFS()
{
    int arrivalCount = 0;
    int departureCount = 0;
    int dispatchCount = 0;

    while (departureCount < MAX_PROCESSES)
    {
        if (!cpu->busy)
        {
            scheduleArrival();
            if (rq_head != nullptr) scheduleDispatch();
        }
        else
            scheduleDeparture();

        switch (eq_head->type)
        {
            case ARRIVE:
                handleArrival();
                arrivalCount++;
                break;
            case DISPATCH:
                handleDispatch();
                dispatchCount++;
                break;
            case DEPARTURE:
                handleDeparture();
                departureCount++;
                break;
            default: cerr << "invalid event type\n";
        }
    }
    cout << "Arrival Count: " << arrivalCount << endl;
    cout << "Dispatch Count: " << dispatchCount << endl;
    cout << "Departure Count: " << departureCount << endl;
}

// The event queue is treated as a priorq_cursory queue by sorting
// the events based on time.
void insertIntoEventQ(eventQNode *newEvent)
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

void SRTF()
{
    int arrivalCount = 0;
    int departureCount = 0;
    int allocationCount = 0;

    while (departureCount < MAX_PROCESSES)
    {
        cout << "arrivalCount: " << arrivalCount << endl;
        cout << "departureCount: " << departureCount << endl;
        if (arrivalCount < (MAX_PROCESSES * 1.20))
        {
            scheduleArrival();
            arrivalCount++;
        }
        if (!cpu->busy)
        {
            if (rq_head != nullptr)
                scheduleDispatch();
        }
        else
        {
            if (eq_head->type == ARRIVE)
            {
                //If the current event's time occurs after the process
                //currently on the CPU completes, it won't be preempted so it can
                //be scheduled normally
                if (eq_head->time > cpuEstFinishTime())
                {
                    cout << "estimated finish time: " << cpuEstFinishTime();
                    scheduleDeparture();
                }
                else if (isPreemptive())
                    schedulePreemption();
            }
        }

        switch (eq_head->type)
        {
            case ARRIVE:
                handleArrival();
                break;
            case DISPATCH:
                handleDispatch();
                allocationCount++;
                break;
            case DEPARTURE:
                handleDeparture();
                departureCount++;
                break;
            case PREEMPT:
                handlePreemption();
                break;
            default: cerr << "invalid event type\n";
        }

    }
    cout << "Arrival Count: " << arrivalCount << endl;
    cout << "Departure Count: " << departureCount << endl;
    cout << "Allocation Count: " << allocationCount << endl;
}

void RR()
{
    int arrivalCount = 0;
    int departureCount = 0;
    while (departureCount < MAX_PROCESSES)
    {
        if (arrivalCount < (MAX_PROCESSES * 1.20))
        {
            scheduleArrival();
            arrivalCount++;
        }
        if (!cpu->busy)
        {
            scheduleArrival();
            if (rq_head != nullptr) scheduleQuantumDispatch();
        }
        else
        {
            float estCompletionTime = 0;
            estCompletionTime = cpu->p_link->reStartTime == 0 ?
                    cpu->p_link->remainingTime : cpu->p_link->startTime;

//            nextQuantum
            if (estCompletionTime < getNextQuantumClockTime())
                scheduleQuantumDeparture();
            else
            {
                if (rq_head != nullptr)
                {
                    if (rq_head->p_link->arrivalTime > estCompletionTime)
                        scheduleQuantumDeparture();
                    else
                        scheduleQuantumPreemption();
                }
            }
        }
        switch (eq_head->type)
        {
            case ARRIVE:
                handleArrival();
                break;
            case DISPATCH:
                handleQuantumDispatch();
                break;
            case DEPARTURE:
                handleQuantumDeparture();
                departureCount++;
                if (rq_head != nullptr && (rq_head->p_link->arrivalTime < cpu->clock))
                    scheduleQuantumDispatch();
                break;
            case PREEMPT:
                handleQuantumPreemption();
            default: cerr << "invalid event type\n";
        }
    }
}

void scheduleQuantumDispatch()
{
    eventQNode *dispatch = new eventQNode;

    procListNode *nextProc;
    nextProc = rq_head->p_link;

    if (rq_head != nullptr)
    {
        if (rq_head->p_link->arrivalTime < cpu->clock)
            dispatch->time = cpu->clock;
        else
        {
            cpu->clock = rq_head->p_link->arrivalTime;

            float nextQuantumTime = quantumClock;
            while (nextQuantumTime < cpu->clock)
                nextQuantumTime += quantum;
            quantumClock = nextQuantumTime;

            dispatch->time = getNextQuantumDispatchTime();
        }
    }
    else cerr << "Error in scheduleQuantumDispatch()\n";

    dispatch->type = DISPATCH;
    dispatch->eq_next = nullptr;
    dispatch->p_link = nextProc;

    insertIntoEventQ(dispatch);
}

void handleQuantumDispatch()
{
    cpu->p_link = eq_head->p_link;
    cpu->busy = true;

    cpu->p_link->startTime == 0 ?
    cpu->p_link->startTime : cpu->p_link->reStartTime = eq_head->time;

    popReadyQHead();
    popEventQHead();
}

void scheduleQuantumDeparture()
{
    eventQNode *departure = new eventQNode;
    departure->type = DEPARTURE;
    departure->eq_next = nullptr;
    departure->p_link = cpu->p_link;

    if (cpu->p_link->reStartTime == 0)
        departure->time = cpu->p_link->startTime + cpu->p_link->remainingTime;
    else
        departure->time = cpu->p_link->reStartTime + cpu->p_link->remainingTime;

    insertIntoEventQ(departure);
}

void handleQuantumDeparture()
{
    cpu->p_link->finishTime = eq_head->time;
    cpu->p_link->remainingTime = 0.0;
    cpu->p_link = nullptr;
    cpu->clock = eq_head->time;
    cpu->busy = false;

    popEventQHead();
}

void scheduleQuantumPreemption()
{
    eventQNode *preemption = new eventQNode;
    preemption->type = PREEMPT;
    preemption->eq_next = nullptr;

    cpu->clock = rq_head->p_link->arrivalTime;

    float nextQuantumTime = quantumClock;
    while (nextQuantumTime < cpu->clock)
        nextQuantumTime += quantum;

    quantumClock = nextQuantumTime;

    preemption->time = getNextQuantumClockTime();

    preemption->p_link = rq_head->p_link;

    insertIntoEventQ(preemption);
}

void handleQuantumPreemption()
{
    procListNode *preemptedProcPtr = cpu->p_link;

    cpu->p_link->remainingTime = cpuEstFinishTime() - eq_head->time;

    cpu->p_link = eq_head->p_link;
    cpu->clock = eq_head->time;

    cpu->p_link->startTime == 0.0 ? cpu->p_link->startTime : cpu->p_link->reStartTime = eq_head->time;

    float nextQuantumTime = quantumClock;
    while (nextQuantumTime < eq_head->time)
        nextQuantumTime += quantum;

    quantumClock = nextQuantumTime;

    eventQNode *preemptedProcArrival = new eventQNode;
    preemptedProcArrival->time = eq_head->time;
    preemptedProcArrival->type = ARRIVE;
    preemptedProcArrival->eq_next = nullptr;
    preemptedProcArrival->p_link = preemptedProcPtr;

    popEventQHead();
    popReadyQHead();

    insertIntoEventQ(preemptedProcArrival);
}

float getNextQuantumClockTime(){ return quantumClock + quantum; }

float getNextQuantumDispatchTime()
{
    float nextQuantumTime = quantumClock;
    while (nextQuantumTime < rq_head->p_link->arrivalTime)
        nextQuantumTime += quantum;

    return nextQuantumTime;
}

float cpuEstFinishTime()
{
    float estFinish;
    float startTime = cpu->p_link->startTime;
    float reStartTime = cpu->p_link->reStartTime;
    float remainingTime = cpu->p_link->remainingTime;

    estFinish = (reStartTime == 0 ? startTime : reStartTime) + remainingTime;

    return estFinish;
}

//Sub routine to locate the process with the shortest remaining time
procListNode *getSRTProcess()
{
    readyQNode *rq_cursor = rq_head;
    procListNode *srtProc = rq_cursor->p_link;
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
    cout << "SRT process ID: " << srtProc->pid << endl;
    return srtProc;
}

procListNode *getHRRProcess()
{
    readyQNode *rq_cursor = rq_head;
    procListNode *hrrProc = rq_cursor->p_link;
    float hrr = getResponseRatioValue(hrrProc);

    while (rq_cursor != nullptr)
    {
        if (getResponseRatioValue(rq_cursor->p_link) > hrr)
        {
            hrr = getResponseRatioValue(rq_cursor->p_link);
            hrrProc = rq_cursor->p_link;
        }
        rq_cursor = rq_cursor->rq_next;
    }

    return hrrProc;
}

void popEventQHead()
{
    eventQNode *tempPtr = eq_head;
    eq_head = eq_head->eq_next;
    delete tempPtr;
}

void popReadyQHead()
{
    readyQNode *tempPtr = rq_head;
    rq_head = rq_head->rq_next;
    delete tempPtr;
}
// HRRN Helper Function
float getResponseRatioValue(procListNode *thisProc)
{
    float HRR = ((cpu->clock - thisProc->arrivalTime) + thisProc->burst) / thisProc->burst;
    return HRR;
}

/*
float getAvgTurnaround()
{
    float totalTurnaround = 0.0,
            avgTurnaround = 0.0;

    if (pl_head == nullptr)  //empty queue
        cerr << "empty queue";
    else
    {
        procListNode *pl_cursor = pl_head;
        while (pl_cursor->finishTime != 0)
        {

//                float tmp = pl_cursor->finishTime - pl_cursor->arrivalTime;
//                cout << pl_cursor->pid << ": " << "pl_cursor->finishTime - pl_cursor->arrivalTime: "
//                    << pl_cursor->finishTime << " - " << pl_cursor->arrivalTime << " = " << tmp << endl;
                totalTurnaround += (pl_cursor->finishTime - pl_cursor->arrivalTime);
                //            cout << "totalTurnaround: " << totalTurnaround << endl;


            pl_cursor = pl_cursor->pl_next;
        }
//        while (pl_cursor->pl_next != nullptr)
//        {
//            if (pl_cursor->finishTime != 0)
//            {
////                float tmp = pl_cursor->finishTime - pl_cursor->arrivalTime;
////                cout << pl_cursor->pid << ": " << "pl_cursor->finishTime - pl_cursor->arrivalTime: "
////                    << pl_cursor->finishTime << " - " << pl_cursor->arrivalTime << " = " << tmp << endl;
//                totalTurnaround += (pl_cursor->finishTime - pl_cursor->arrivalTime);
//                //            cout << "totalTurnaround: " << totalTurnaround << endl;
//
//            }
//            pl_cursor = pl_cursor->pl_next;
//        }
    }
//    cout << "totalTurnaround: " << totalTurnaround << endl;
    return (totalTurnaround / MAX_PROCESSES);
}

float getTotalThroughput()
{
    procListNode *pl_cursor = pl_head;
    float finTime = 0.0;
    int count = 0;

    while (pl_cursor->pl_next != nullptr)
    {
        pl_cursor->finishTime == 0 ? (count++) : (finTime = pl_cursor->finishTime);

        pl_cursor = pl_cursor->pl_next;
    }
//    cout << "totalTime: " << finTime << endl;
    return ((float)MAX_PROCESSES / finTime);
}
 */
///////////////////////////////////////////////////////////////
int main(int argc, char *argv[] )
{
    if (argc < 3)
    {
        show_usage();
        return 1;
    }
    if (*argv[2] == 3 && argc < 5)      // _RR requires the quantum arg
    {
        cerr << "Expected 4 arguments and got " << argc << endl;
        show_usage();
        return 1;
    }
    parseArgs(argc, argv);
    init();
    run_sim();
    generate_report();
    return 0;
}

