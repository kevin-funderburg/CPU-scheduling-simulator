/***
 * CPU Scheduler Simulation
 * Authors: Kevin Funderburg, Rob Murrat
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
#include "list.h"
#include "header.h"
using namespace std;


void parseArgs(int argc, char *argv[])
{
    scheduler = static_cast<Scheduler>(stoi(argv[1]));  // set scheduler algorithm
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
    p_completed = 0;

    cpuHead = new cpuNode;
    cpuHead->clock = 0.0;
    cpuHead->cpuBusy = false;

    process p;
    p.pid = 0;
    p.arrivalTime = genexp((float)lambda);
    p.startTime = p.reStartTime = p.completionTime = 0.0;
    p.burst = genexp(avgServiceTime);
    p.remainingTime = p.burst;
    pList.push_back(p);
    lastid = 0;
//    pHead = new procListNode;
//    pHead->p.arrivalTime = genexp((float)lambda);
//    pHead->p.startTime = pHead->p.reStartTime =pHead->p.completionTime = 0.0;
//    pHead->p.burst = genexp(avgServiceTime);
//    pHead->p.remainingTime = pHead->p.burst;
//    pHead->pNext = nullptr;

    event *newArrival = new event;
    newArrival->time = p.arrivalTime;
    newArrival->type = ARRIVE;
    schedule_event_eventQ(newArrival);
}


////////////////////////////////////////////////////////////////
void generate_report()
{
    // output statistics
    clog << "outputting stats\n";
}


int genID() { return lastid++; }


void schedule_event_eventQ(event *newEvent)
{
    clog << "event: ";
    switch (newEvent->type) {
        case ARRIVE: clog << "ARRIVE\n"; break;
        case DEPARTURE: clog << "DEPARTURE\n"; break;
        case LEAVE_CPU: clog << "LEAVE_CPU\n"; break;
        case COMPLETION: clog << "COMPLETION\n"; break;
        default: cerr << "invalid type";
    }
    eventQueue.push(newEvent);
}


////////////////////////////////////////////////////////////////
// returns a random number between 0 and 1
float urand()
{
    return( (float) rand()/RAND_MAX );
}


/////////////////////////////////////////////////////////////
// returns a random number that follows an exp distribution
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
    process p;
    p.pid = genID();
    p.arrivalTime = genexp((float)lambda);
    p.startTime = p.reStartTime = p.completionTime = 0.0;
    p.burst = genexp(avgServiceTime);
    p.remainingTime = p.burst;
    pList.push_back(p);
//    procListNode *pIt = pHead;
//    while (pIt->pNext != NULL)
//        pIt = pIt->pNext;
//    pIt->pNext = new procListNode;
//    pIt->pNext->p.arrivalTime = pIt->p.arrivalTime + genexp((float)lambda);
//    pHead->p.startTime = pHead->p.reStartTime =pHead->p.completionTime = 0.0;
//    pIt->pNext->p.burst = genexp(avgServiceTime);
//    pIt->pNext->p.remainingTime = pIt->pNext->p.burst;
//    pIt->pNext->pNext = NULL;

    event *newArrival = new event;
    newArrival->time = p.arrivalTime;
    newArrival->type = ARRIVE;
    schedule_event_eventQ(newArrival);
}


void handleArrival()
{
    //TODO - update process fields
    process p;
    readyQ.push(p);     // add process to end of ready queue
    eventQueue.pop();   // remove first event
}


void scheduleDeparture()
{
    event *newDeparture = new event;
    newDeparture->type = DEPARTURE;

    if (scheduler == _FCFS)
        newDeparture->time = cpuHead->pLink->p.startTime + cpuHead->pLink->p.remainingTime;
    else if (scheduler == _SRTF)
    {
        // TODO
    }

    schedule_event_eventQ(newDeparture);
}


void handleDeparture()
{
    cpuHead->clock = eventQueue.top()->time;
    cpuHead->pLink->p.completionTime = cpuHead->clock;
    pHead->p.completionTime = cpuHead->pLink->p.completionTime;

    cpuHead->pLink->p.completionTime = 0.0;
    cpuHead->pLink = nullptr;

    cpuHead->cpuBusy = false;

    eventQueue.pop();
}


void scheduleAllocation()
{
    event *newAllocation = new event;
//    procListNode *nextProcess;
    process nextProcess;
    switch (scheduler)
    {
        case _FCFS:
            nextProcess = pList.back();
//            nextProcess = readyQHead->pLink;
            break;
        case _SRTF:
            break;
        case _RR:
            break;
        default:
            cerr << "invalid scheduler\n";
    }
    if (cpuHead->clock < nextProcess.arrivalTime)
        newAllocation->time = nextProcess.arrivalTime;
    else
        newAllocation->time = cpuHead->clock;

    newAllocation->type = LEAVE_CPU;
    schedule_event_eventQ(newAllocation);

}


void handleAllocation()
{

}


void schedulePreemption()
{

}


////////////////////////////////////////////////////////////
int run_sim()
{
    switch (scheduler)
    {
        case _FCFS: cout << "Scheduling as FCFS\n\n";  FCFS(); break;
        case _SRTF: cout << "Scheduling as SRTF\n\n";  SRTF(); break;
        case _RR:   cout << "Scheduling as RR\n\n";    RR();   break;
        default:    cerr << "invalid scheduler\n";     return 1;
    }
    return 0;
}


void FCFS() {
    int departureCount = 0;
    int p_count = 0;
    int allocationCount = 0;

    while (p_completed < MAX_PROCESSES)
    {
        if (!cpuHead->cpuBusy)
        {
            scheduleArrival();
//            if (readyQHead == nullptr)
            if (readyQ.size() == 0)
                scheduleAllocation();
        } else {
            scheduleDeparture();
        }

        switch (eventQueue.top()->type)
        {
            case ARRIVE:
                handleArrival();
                p_count++;
                break;
            case DEPARTURE:
                handleDeparture();
                departureCount++;
                break;
            case LEAVE_CPU:
                handleAllocation();
                allocationCount++;
                break;
            case COMPLETION:
                break;
            default:
                cerr << "invalid type";
        }
    }
}


void SRTF()
{

}


void RR()
{

}


////////////////////////////////////////////////////////////////
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

