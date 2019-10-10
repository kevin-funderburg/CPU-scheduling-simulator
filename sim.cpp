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
//    lambda = (float)1.0 / stof(argv[2]);
    avgServiceTime = stof(argv[3]);
    if (argc == 5)
        float quantum = stof(argv[4]);
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
    quantumClock = 0.0;
    lastid = 0;

    avgArrivalTime = 1.0/(float)lambda;

    cpu_head = new cpuNode;
    cpu_head->clock = 0.0;
    cpu_head->busy = false;
    cpu_head->p_link = nullptr;

    pl_head = new procListNode;
    pl_head->pid = lastid;
    pl_head->arrivalTime = genexp(avgArrivalTime);
    pl_head->startTime = 0.0;
    pl_head->reStartTime = 0.0;
    pl_head->finishTime = 0.0;
    pl_head->burst = genexp(avgServiceTime);
    pl_head->remainingTime = pl_head->burst;
    pl_head->pl_next = nullptr;

    eq_head = new eventQNode;
    eq_head->time = pl_head->arrivalTime;
    eq_head->type = ARRIVE;
    eq_head->eq_next = nullptr;
    eq_head->p_link = pl_head;
}


////////////////////////////////////////////////////////////////
void generate_report()
{
    ofstream data("sim.data",  ios::out | ios::app);
    if (data.is_open())
    {
        if (lambda == 1)
        {
            data << "---------------------------------------------------------------------------------------------------------------\n"
                 << "lambda\tAvgTurnaround\tThroughput\n"
                 << "---------------------------------------------------------------------------------------------------------------\n";
        }
        data << lambda << "\t\t" << getAvgTurnaround() << "\t\t\t" << getTotalThroughput() << endl;

        data.close();
    }
    else cout << "Unable to open file";
}


int genID() { return ++lastid; }


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
    procListNode *pl_cursor = pl_head;
    while (pl_cursor->pl_next != nullptr)
        pl_cursor = pl_cursor->pl_next;

    pl_cursor->pl_next = new procListNode;
    pl_cursor->pl_next->pid = pl_cursor->pid + 1;
    pl_cursor->pl_next->arrivalTime = pl_cursor->arrivalTime + genexp(avgArrivalTime);
    pl_cursor->pl_next->startTime = 0.0;
    pl_cursor->pl_next->reStartTime = 0.0;
    pl_cursor->pl_next->finishTime = 0.0;
    pl_cursor->pl_next->burst = genexp(avgServiceTime);
    pl_cursor->pl_next->remainingTime = pl_cursor->pl_next->burst;
    pl_cursor->pl_next->pl_next = nullptr;

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
        if (cpu_head->clock > rq_head->p_link->arrivalTime)
            nextProc = getSRTProcess();
        else
            nextProc = rq_head->p_link;
    }

    else if (scheduler == _RR)
        nextProc = getHRRProcess();

    dispatch->time = cpu_head->clock < nextProc->arrivalTime ? nextProc->arrivalTime : cpu_head->clock;

    dispatch->type = DISPATCH;
    dispatch->eq_next = nullptr;
    dispatch->p_link = nextProc;

    insertIntoEventQ(dispatch);
}

void handleDispatch()
{
    cpu_head->p_link = eq_head->p_link;  //assign process to CPU

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

    cpu_head->busy = true;    //CPU now busy executing process

    //set clock to time of process arrival
    if (cpu_head->clock < cpu_head->p_link->arrivalTime)
        cpu_head->clock = cpu_head->p_link->arrivalTime;

    if (cpu_head->p_link->startTime == 0)
        cpu_head->p_link->startTime = cpu_head->clock;
    else
        cpu_head->p_link->reStartTime = cpu_head->clock;
}

void scheduleDeparture()
{
    eventQNode *departure = new eventQNode;
    departure->type = DEPARTURE;
    departure->eq_next = nullptr;
    departure->p_link = cpu_head->p_link;

    if (scheduler == _FCFS || scheduler == _RR)
        departure->time = cpu_head->p_link->startTime + cpu_head->p_link->remainingTime;  //FCFS will process to completion

    else if (scheduler == _SRTF)
    {
        if (cpu_head->p_link->reStartTime == 0)
            departure->time = cpu_head->p_link->startTime + cpu_head->p_link->remainingTime;
        else
            departure->time = cpu_head->p_link->reStartTime + cpu_head->p_link->remainingTime;
    }

    insertIntoEventQ(departure);
}

void handleDeparture()
{
    cpu_head->clock = eq_head->time;   //set clock to head of event queue

    cpu_head->p_link->finishTime = cpu_head->clock;    //log finishTime
    pl_head->finishTime = cpu_head->p_link->finishTime;

    cpu_head->p_link->remainingTime = 0.0;    //clear remainingTime
    cpu_head->p_link = nullptr;

    cpu_head->busy = false;   //CPU ready for next process

    popEventQHead();    //remove departure event
}

void schedulePreemption()
{
    //TODO
}


////////////////////////////////////////////////////////////
int run_sim()
{
    switch (scheduler)
    {
        case _FCFS: cout << "Scheduling as FCFS\n\n"; FCFS();
            break;
        case _SRTF: cout << "Scheduling as SRTF\n\n"; SRTF();
            break;
        case _RR: cout << "Scheduling as RR\n\n"; RR();
            break;
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
        if (!cpu_head->busy)
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
            default:
                cerr << "invalid event type\n";
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

void SRTF()
{
    //TODO
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
        if (!cpu_head->busy)
        {
            scheduleArrival();
            if (rq_head != nullptr) scheduleQuantumDispatch();
        }
        else
        {
            if (cpuEstFinishTime() < getNextQuantumClockTime())
                scheduleQuantumDeparture();
            else
            {
                if (rq_head != nullptr)
                {
                    if (rq_head->p_link->arrivalTime > cpuEstFinishTime())
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
                if (rq_head != nullptr && (rq_head->p_link->arrivalTime < cpu_head->clock))
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
        if (rq_head->p_link->arrivalTime < cpu_head->clock)
            dispatch->time = cpu_head->clock;
        else
        {
            cpu_head->clock = rq_head->p_link->arrivalTime;

            float nextQuantumTime = quantumClock;
            while (nextQuantumTime < cpu_head->clock)
                nextQuantumTime += quantum;
            quantumClock = nextQuantumTime;

            dispatch->time = getNextQuantumDispatchTime();
        }
    }
    else
        cerr << "Error in scheduleQuantumDispatch()\n";

    dispatch->type = DISPATCH;
    dispatch->eq_next = nullptr;
    dispatch->p_link = nextProc;

    insertIntoEventQ(dispatch);
}

void handleQuantumDispatch()
{
    cpu_head->p_link = eq_head->p_link;
    cpu_head->busy = true;

    if (cpu_head->p_link->startTime == 0)
        cpu_head->p_link->startTime;
    else
        cpu_head->p_link->reStartTime = eq_head->time;

    popReadyQHead();
    popEventQHead();
}

void scheduleQuantumDeparture()
{
    eventQNode *nuDeparture = new eventQNode;
    nuDeparture->type = DEPARTURE;
    nuDeparture->eq_next = nullptr;
    nuDeparture->p_link = cpu_head->p_link;

    if (cpu_head->p_link->reStartTime == 0)
        nuDeparture->time = cpu_head->p_link->startTime + cpu_head->p_link->remainingTime;
    else
        nuDeparture->time = cpu_head->p_link->reStartTime + cpu_head->p_link->remainingTime;

    insertIntoEventQ(nuDeparture);
}

void handleQuantumDeparture()
{
    cpu_head->p_link->finishTime = eq_head->time;
    cpu_head->p_link->remainingTime = 0.0;
    cpu_head->p_link = nullptr;
    cpu_head->clock = eq_head->time;
    cpu_head->busy = false;

    popEventQHead();
}

void scheduleQuantumPreemption()
{
    eventQNode *preemption = new eventQNode;
    preemption->type = PREEMPT;
    preemption->eq_next = nullptr;

    cpu_head->clock = rq_head->p_link->arrivalTime;

    float nextQuantumTime = quantumClock;
    while (nextQuantumTime < cpu_head->clock)
        nextQuantumTime += quantum;

    quantumClock = nextQuantumTime;

    preemption->time = getNextQuantumClockTime();

    preemption->p_link = rq_head->p_link;

    insertIntoEventQ(preemption);
}

void handleQuantumPreemption()
{
    procListNode *preemptedProcPtr = cpu_head->p_link;

    cpu_head->p_link->remainingTime = cpuEstFinishTime() - eq_head->time;

    cpu_head->p_link = eq_head->p_link;
    cpu_head->clock = eq_head->time;

    cpu_head->p_link->startTime == 0.0 ? cpu_head->p_link->startTime : cpu_head->p_link->reStartTime = eq_head->time;

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
    float startTime = cpu_head->p_link->startTime;
    float reStartTime = cpu_head->p_link->reStartTime;
    float remainingTime = cpu_head->p_link->remainingTime;

    estFinish = (reStartTime == 0 ? startTime : reStartTime) + remainingTime;

    return estFinish;
}

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

// HRRN Helper Function
float getResponseRatioValue(procListNode *thisProc)
{
    float HRR = ((cpu_head->clock - thisProc->arrivalTime) + thisProc->burst) / thisProc->burst;
    return HRR;
}


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

