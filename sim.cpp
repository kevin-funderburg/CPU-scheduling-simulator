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
#include <cstdlib>
#include <cmath>
#include <list>
#include <queue>
#include "event.h"
#include "header.h"
using namespace std;
/////////////////////////////////////////////////
void parseArgs(int argc, char *argv[])
{
    schedulerType = static_cast<Scheduler>(stoi(argv[1]));  // set schedulerType
    lambda = 1.0 / stof(argv[2]);
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
//    mu = (float)1.0 / avgServiceTime;
    quantumClock = 0.0;

    cpu_head = new cpuNode;
    cpu_head->clock = 0.0;
    cpu_head->cpuBusy = false;
    cpu_head->pLink = NULL;

    pl_head = new procListNode;
    pl_head->arrivalTime = genexp((float)lambda);
    pl_head->startTime = 0.0;
    pl_head->reStartTime = 0.0;
    pl_head->finishTime = 0.0;
    pl_head->burst = genexp(mu);
    pl_head->remainingTime = pl_head->burst;
    pl_head->pNext = NULL;

    eq_head = new eventQNode;
    eq_head->time = pl_head->arrivalTime;
    eq_head->type = ARRIVE;
    eq_head->eNext = NULL;
    eq_head->pLink = pl_head;
}


////////////////////////////////////////////////////////////////
void generate_report()
{
    // output statistics
    clog << "outputting stats\n";
}


int genID() { return ++lastid; }


void addToEventQ(event *newEvent)
{
    debugging(newEvent);
//    eventQ.push(newEvent);
}


void debugging(event *newEvent)
{
    clog << "DEBUG [" << cpu_head->clock << "] - adding event: ";
    switch (newEvent->type) {
        case ARRIVE: clog << "ARRIVE"; break;
        case DEPARTURE: clog << "DEPARTURE"; break;
        case DISPATCH: clog << "DISPATCH"; break;
        default: cerr << "invalid type";
    }
    clog << "\n\twill happen at time: [" << newEvent->time << "]\n"
         << "\tattached to process id: [" << newEvent->pid << "]\n";

//    clog << "\n\tQUEUE SIZES:"
//         << "\tevent: [" << eventQ.size()
//         << "]\tready: [" << readyQ.size()
//         << "] process list: [" << pList.size() << "]\n";
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

    procListNode *pl_cursor = pl_head;
    while (pl_cursor->pNext != NULL)
        pl_cursor = pl_cursor->pNext;

    pl_cursor->pNext = new procListNode;
    pl_cursor->pNext->arrivalTime = pl_cursor->arrivalTime + genexp((float)lambda);
    pl_cursor->pNext->startTime = 0.0;
    pl_cursor->pNext->reStartTime = 0.0;
    pl_cursor->pNext->finishTime = 0.0;
    pl_cursor->pNext->burst = genexp(mu);
    pl_cursor->pNext->remainingTime = pl_cursor->pNext->burst;
    pl_cursor->pNext->pNext = NULL;

    eventQNode *arrival = new eventQNode;
    arrival->time = pl_cursor->pNext->arrivalTime;
    arrival->type = ARRIVE;
    arrival->pLink = pl_cursor->pNext;
    arrival->eNext = NULL;

    insertIntoEventQ(arrival);
}

//Push process into ready queue and remove event from event queue
void handleArrival()
{
    readyQNode *ready = new readyQNode;
    ready->pLink = eq_head->pLink;
    ready->rNext = NULL;

    if (rq_head == NULL)  //empty queue
        rq_head = ready;
    else
    {
        readyQNode *rq_cursor = rq_head;
        while (rq_cursor->rNext != NULL)
            rq_cursor = rq_cursor->rNext;

        rq_cursor->rNext = ready;
    }

    popEventQHead();
}


void scheduleDispatch()
{

    eventQNode *nuAllocation = new eventQNode;

    procListNode *nextProc;
    if (schedulerType == _FCFS)
        nextProc = rq_head->pLink;
    else if (schedulerType == _SRTF)
    {
        if (cpu_head->clock > rq_head->pLink->arrivalTime)
        {
//            nextProc = getSRTProcess();
        }
        else
        {
            nextProc = rq_head->pLink;
        }
    }
    else if (schedulerType == _RR)
    {
//        nextProc = getHRRProcess();
    }

    if (cpu_head->clock < nextProc->arrivalTime)
        nuAllocation->time = nextProc->arrivalTime;

    else
        nuAllocation->time = cpu_head->clock;

    nuAllocation->type = DISPATCH;
    nuAllocation->eNext = NULL;
    nuAllocation->pLink = nextProc;

    insertIntoEventQ(nuAllocation);
}

void handleDispatch()
{
    cpu_head->pLink = eq_head->pLink;  //assign process to CPU

    if (schedulerType == _SRTF || schedulerType == _RR)
    {

        readyQNode *rq_cursor = rq_head->rNext;
        readyQNode *rq_precursor = rq_head;
        if (rq_precursor->pLink->arrivalTime != eq_head->pLink->arrivalTime)
        {
            while (rq_cursor != NULL)
            {
                if (rq_cursor->pLink->arrivalTime == eq_head->pLink->arrivalTime)
                {
                    rq_precursor->rNext = rq_cursor->rNext;
                    rq_cursor->rNext = rq_head;
                    rq_head = rq_cursor;
                    break;
                }
                rq_cursor = rq_cursor->rNext;
                rq_precursor = rq_precursor->rNext;
            }
        }
    }

    popReadyQHead();
    popEventQHead();

    cpu_head->cpuBusy = true;    //CPU now busy executing process

    if (cpu_head->clock < cpu_head->pLink->arrivalTime)
        cpu_head->clock = cpu_head->pLink->arrivalTime;

    if (cpu_head->pLink->startTime == 0)
        cpu_head->pLink->startTime = cpu_head->clock;
    else
        cpu_head->pLink->reStartTime = cpu_head->clock;
}

void scheduleDeparture()
{
    eventQNode *nuDeparture = new eventQNode;
    nuDeparture->type = DEPARTURE;
    nuDeparture->eNext = NULL;
    nuDeparture->pLink = cpu_head->pLink;

    if (schedulerType == _FCFS || schedulerType == _RR)
        nuDeparture->time = cpu_head->pLink->startTime + cpu_head->pLink->remainingTime;  //FCFS will process to completion

    else if (schedulerType == _SRTF)
    {
        if (cpu_head->pLink->reStartTime == 0)
            nuDeparture->time = cpu_head->pLink->startTime + cpu_head->pLink->remainingTime;
        else
            nuDeparture->time = cpu_head->pLink->reStartTime + cpu_head->pLink->remainingTime;
    }

    insertIntoEventQ(nuDeparture);
}

void handleDeparture()
{
    cpu_head->clock = eq_head->time;   //set clock to head of event queue

    cpu_head->pLink->finishTime = cpu_head->clock;    //log finishTime
    pl_head->finishTime = cpu_head->pLink->finishTime;

    cpu_head->pLink->remainingTime = 0.0;    //clear remainingTime
    cpu_head->pLink = NULL;

    cpu_head->cpuBusy = false;   //CPU ready for next process

    popEventQHead();    //remove departure event
}

void schedulePreemption()
{
    //TODO
}


////////////////////////////////////////////////////////////
int run_sim()
{
    switch (schedulerType)
    {
        case _FCFS: cout << "Scheduling as FCFS\n\n";
            FCFS();
            break;
        case _SRTF: cout << "Scheduling as SRTF\n\n";
            SRTF();
            break;
        case _RR: cout << "Scheduling as RR\n\n";
            RR();
            break;
        default: cerr << "invalid schedulerType\n"; return 1;
    }
    return 0;
}


void FCFS()
{
    int p_count = 0;
    int departureCount = 0;
    int allocationCount = 0;

    while (departureCount < MAX_PROCESSES)
    {
        if (!cpu_head->cpuBusy)
        {
            scheduleArrival();
            if (rq_head != NULL)
                scheduleDispatch();
        }
        else
            scheduleDeparture();

        switch (eq_head->type)
        {
            case ARRIVE:
                handleArrival();
                p_count++;
                break;
            case DISPATCH:
                handleDispatch();
                allocationCount++;
                break;
            case DEPARTURE:
                handleDeparture();
                departureCount++;
                break;
            default:
                cerr << "invalid event type\n";
        }
    }
    cout << "Arrival Count: " << p_count << endl;
    cout << "Departure Count: " << departureCount << endl;
    cout << "Allocation Count: " << allocationCount << endl;
}

// The event queue is treated as a priority queue by sorting
// the events based on time.
void insertIntoEventQ(eventQNode *newEvent)
{
    if (eq_head == NULL)  //empty list
        eq_head = newEvent;
    else if (eq_head->time > newEvent->time)   //add to front
    {
        newEvent->eNext = eq_head;
        eq_head = newEvent;
    }
    else
    {
        eventQNode *eq_cursor = eq_head;
        while (eq_cursor != NULL)
        {
            if ((eq_cursor->time < newEvent->time) && (eq_cursor->eNext == NULL))  //add to tail
            {
                eq_cursor->eNext = newEvent;
                break;
            }
            else if ((eq_cursor->time < newEvent->time) && (eq_cursor->eNext->time > newEvent->time))   //add inside
            {
                newEvent->eNext = eq_cursor->eNext;
                eq_cursor->eNext = newEvent;
                break;
            }
            else
                eq_cursor = eq_cursor->eNext;
        }
    }
}
// Helper Function
void popEventQHead()
{
    eventQNode *tempPtr = eq_head;
    eq_head = eq_head->eNext;
    delete tempPtr;
}

// Helper Function
void popReadyQHead()
{
    readyQNode *tempPtr = rq_head;
    rq_head = rq_head->rNext;
    delete tempPtr;
}

void SRTF()
{
    //TODO
}


void RR()
{
    //TODO
}

float getTurnaroundTime()
{
    float totalTurnaround = 0.0,
            avgTurnaround = 0.0;

    if (pl_head == NULL)  //empty queue
        cerr << "empty queue";
    else {
        procListNode *pl_cursor = pl_head;
        while (pl_cursor->pNext != NULL)
        {
            totalTurnaround += (pl_cursor->finishTime - pl_cursor->arrivalTime);
            pl_cursor = pl_cursor->pNext;
        }
    avgTurnaround = MAX_PROCESSES / totalTurnaround;
    return avgTurnaround;
    }
}

////////////////////////////////////////////////////////////////
int main(int argc, char *argv[] )
{
    clog << "hello";

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

