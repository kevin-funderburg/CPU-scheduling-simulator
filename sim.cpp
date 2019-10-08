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
    schedulerType = static_cast<Scheduler>(stoi(argv[1]));  // set schedulerType algorithm
    lambda = (stoi(argv[2]));                 // 1 / argument is the arrival process time
    avgArrivalTime = 1 / (float)lambda;
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
    mu = (float)1.0 / avgTs;
    quantumClock = 0.0;
    cpuHead = new cpuNode;
    cpuHead->clock = 0.0;
    cpuHead->cpuIsBusy = false;
    cpuHead->pLink = NULL;
    pHead = new procListNode;
    pHead->arrivalTime = genexp((float)lambda);
    pHead->startTime = 0.0;
    pHead->reStartTime = 0.0;
    pHead->finishTime = 0.0;
    pHead->serviceTime = genexp(mu);
    pHead->remainingTime = pHead->serviceTime;
    pHead->pNext = NULL;
    eHead = new eventQNode;
    eHead->time = pHead->arrivalTime;
    eHead->type = 1;
    eHead->eNext = NULL;
    eHead->pLink = pHead;
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
    clog  << "DEBUG [" << cpuHead->clock << "] - adding event: ";
    switch (newEvent->type) {
        case ARRIVE: clog << "ARRIVE"; break;
        case DEPARTURE: clog << "DEPARTURE"; break;
        case ALLOCATION: clog << "ALLOCATION"; break;
        case COMPLETION: clog << "COMPLETION"; break;
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

    procListNode *pIt = pHead;
    while (pIt->pNext != NULL)
    {
        pIt = pIt->pNext;
    }
    pIt->pNext = new procListNode;
    pIt->pNext->arrivalTime = pIt->arrivalTime + genexp((float)lambda);
    pIt->pNext->startTime = 0.0;
    pIt->pNext->reStartTime = 0.0;
    pIt->pNext->finishTime = 0.0;
    pIt->pNext->serviceTime = genexp(mu);
    pIt->pNext->remainingTime = pIt->pNext->serviceTime;
    pIt->pNext->pNext = NULL;

    eventQNode *nuArrival = new eventQNode;
    nuArrival->time = pIt->pNext->arrivalTime;
    nuArrival->type = 1;
    nuArrival->pLink = pIt->pNext;
    nuArrival->eNext = NULL;

    insertIntoEventQ(nuArrival);
}


void handleArrival()
{

    readyQNode *nuReady = new readyQNode;
    nuReady->pLink = eHead->pLink;
    nuReady->rNext = NULL;

    if (rHead == NULL)
        rHead = nuReady;
    else
    {
        readyQNode *rIt = rHead;
        while (rIt->rNext != 0)
        {
            rIt = rIt->rNext;
        }
        rIt->rNext = nuReady;
    }

    popEventQHead();
}


void scheduleAllocation()
{

    eventQNode *nuAllocation = new eventQNode;

    procListNode *nextProc;
    if (schedulerType == _FCFS)
        nextProc = rHead->pLink;
    else if (schedulerType == _SRTF)
    {
        if (cpuHead->clock > rHead->pLink->arrivalTime)
        {
//            nextProc = getSRTProcess();
        }
        else
        {
            nextProc = rHead->pLink;
        }
    }
    else if (schedulerType == _RR)
    {
//        nextProc = getHRRProcess();
    }

    if (cpuHead->clock < nextProc->arrivalTime)
    {
        nuAllocation->time = nextProc->arrivalTime;
    }
    else
    {
        nuAllocation->time = cpuHead->clock;
    }

    nuAllocation->type = 3;
    nuAllocation->eNext = NULL;
    nuAllocation->pLink = nextProc;

    insertIntoEventQ(nuAllocation);
}

void handleAllocation()
{

    cpuHead->pLink = eHead->pLink;

    if (schedulerType == 2 || schedulerType == 3)
    {

        readyQNode *rIt = rHead->rNext;
        readyQNode *rItPrev = rHead;
        if (rItPrev->pLink->arrivalTime != eHead->pLink->arrivalTime)
        {
            while (rIt != 0)
            {
                if (rIt->pLink->arrivalTime == eHead->pLink->arrivalTime)
                {
                    rItPrev->rNext = rIt->rNext;
                    rIt->rNext = rHead;
                    rHead = rIt;
                    break;
                }
                rIt = rIt->rNext;
                rItPrev = rItPrev->rNext;
            }
        }
    }

    popReadyQHead();
    popEventQHead();

    cpuHead->cpuIsBusy = true;

    if (cpuHead->clock < cpuHead->pLink->arrivalTime)
    {
        cpuHead->clock = cpuHead->pLink->arrivalTime;
    }

    if (cpuHead->pLink->startTime == 0)
    {
        cpuHead->pLink->startTime = cpuHead->clock;
    }
    else
    {
        cpuHead->pLink->reStartTime = cpuHead->clock;
    }
}

void scheduleDeparture()
{

    eventQNode *nuDeparture = new eventQNode;
    nuDeparture->type = 2;
    nuDeparture->eNext = 0;
    nuDeparture->pLink = cpuHead->pLink;

    if (schedulerType == 1 || schedulerType == 3)
    {
        nuDeparture->time = cpuHead->pLink->startTime + cpuHead->pLink->remainingTime;
    }
    else if (schedulerType == 2)
    {
        if (cpuHead->pLink->reStartTime == 0)
        {
            nuDeparture->time = cpuHead->pLink->startTime + cpuHead->pLink->remainingTime;
        }
        else
        {
            nuDeparture->time = cpuHead->pLink->reStartTime + cpuHead->pLink->remainingTime;
        }
    }

    insertIntoEventQ(nuDeparture);
}

void handleDeparture()
{

    cpuHead->clock = eHead->time;

    cpuHead->pLink->finishTime = cpuHead->clock;
    pHead->finishTime = cpuHead->pLink->finishTime;

    cpuHead->pLink->remainingTime = 0.0;
    cpuHead->pLink = NULL;

    cpuHead->cpuIsBusy = false;

    popEventQHead();
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
        case _FCFS: cout << "Scheduling as FCFS\n\n";  FCFS(); break;
        case _SRTF: cout << "Scheduling as SRTF\n\n";  SRTF(); break;
        case _RR:   cout << "Scheduling as RR\n\n";    RR();   break;
        default:    cerr << "invalid schedulerType\n";     return 1;
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
        if (cpuHead->cpuIsBusy == false)
        {
            scheduleArrival();
            if (rHead != NULL)
            {
                scheduleAllocation();
            }
        }
        else
            scheduleDeparture();

        if (eHead->type == 1)
        {
            handleArrival();
            p_count++;
        }
        else if (eHead->type == 2)
        {
            handleDeparture();
            departureCount++;
        }
        else if (eHead->type == 3)
        {
            handleAllocation();
            allocationCount++;
        }
    }
    cout << "Arrival Count: " << p_count << endl;
    cout << "Departure Count: " << departureCount << endl;
    cout << "Allocation Count: " << allocationCount << endl;
}

// Helper Function
void insertIntoEventQ(eventQNode *nuEvent)
{

    if (eHead == 0)
        eHead = nuEvent;
    else if (eHead->time > nuEvent->time)
    {
        nuEvent->eNext = eHead;
        eHead = nuEvent;
    }
    else
    {
        eventQNode *eIt = eHead;
        while (eIt != 0)
        {
            if ((eIt->time < nuEvent->time) && (eIt->eNext == 0))
            {
                eIt->eNext = nuEvent;
                break;
            }
            else if ((eIt->time < nuEvent->time) && (eIt->eNext->time > nuEvent->time))
            {
                nuEvent->eNext = eIt->eNext;
                eIt->eNext = nuEvent;
                break;
            }
            else
            {
                eIt = eIt->eNext;
            }
        }
    }
}
// Helper Function
void popEventQHead()
{
    eventQNode *tempPtr = eHead;
    eHead = eHead->eNext;
    delete tempPtr;
}

// Helper Function
void popReadyQHead()
{
    readyQNode *tempPtr = rHead;
    rHead = rHead->rNext;
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


////////////////////////////////////////////////////////////////
int main(int argc, char *argv[] )
clog << "hello";
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

