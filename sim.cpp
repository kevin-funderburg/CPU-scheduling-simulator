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
#include "list.h"
#include "header.h"
using namespace std;
/////////////////////////////////////////////////
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
    p_table[0] = p;
    lastid = 0;

    event *newArrival = new event;
    newArrival->time = p.arrivalTime;
    newArrival->type = ARRIVE;
    newArrival->pid = 0;
    lastArrival = newArrival;
    scheduleEvent(newArrival);
}


////////////////////////////////////////////////////////////////
void generate_report()
{
    // output statistics
    clog << "outputting stats\n";
}


int genID() { return ++lastid; }


void scheduleEvent(event *newEvent)
{
    clog << "time: " << cpuHead->clock << "\tevent: ";
    switch (newEvent->type) {
        case ARRIVE: clog << "ARRIVE\n"; break;
        case DEPARTURE: clog << "DEPARTURE\n"; break;
        case LEAVE_CPU: clog << "LEAVE_CPU\n"; break;
        case COMPLETION: clog << "COMPLETION\n"; break;
        default: cerr << "invalid type";
    }
    eventQueue.push(newEvent);
    cout << "\t\tqueue sizes:\n"
         << "\t\tevent: [" << eventQueue.size()
         << "]\tready: [" << readyQ.size()
         << "] process list: " << pList.size() << endl;

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
    process p;
    p.pid = genID();
    p.arrivalTime = lastArrival->time + genexp((float)lambda);
    p.startTime = p.reStartTime = p.completionTime = 0.0;
    p.burst = genexp(avgServiceTime);
    p.remainingTime = p.burst;
    p_table[p.pid] = p;
    pList.push_back(p);

    event *newArrival = new event;
    newArrival->time = p.arrivalTime;
    newArrival->type = ARRIVE;
    newArrival->pid = p.pid;
    lastArrival = newArrival;
    scheduleEvent(newArrival);
}


void handleArrival()
{
    //TODO - update process fields
    process p = p_table[eventQueue.top()->pid];
    readyQ.push(p);     // add process to end of ready queue
    eventQueue.pop();   // remove first event
}


void scheduleDeparture()
{
    event *newDeparture = new event;
    newDeparture->type = DEPARTURE;

    if (scheduler == _FCFS)
    {
        process &p = p_table[cpuHead->pid];
        newDeparture->time = p.startTime + p.remainingTime;
        newDeparture->pid = p.pid;
    }
    else if (scheduler == _SRTF)
    {
        // TODO
    }

    scheduleEvent(newDeparture);
}


void handleDeparture()
{
    process &p = p_table[eventQueue.top()->pid];
    cpuHead->clock = eventQueue.top()->time;
    p.completionTime = cpuHead->clock;

    // i don't think this is necessary when using the table
    // because im passing the process by reference
//    pHead->p.completionTime = cpuHead->pLink->p.completionTime;

    p.remainingTime = 0.0;
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
            nextProcess = p_table[lastid];
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
    newAllocation->pid = nextProcess.pid;
    scheduleEvent(newAllocation);
}


void handleAllocation()
{
    process &p = p_table[eventQueue.top()->pid];
    cpuHead->pid = p.pid;
    if (scheduler == _SRTF || scheduler == _RR)
    {
        //TODO
    }

    eventQueue.pop();
    readyQ.pop();

    cpuHead->cpuBusy = true;

    if (cpuHead->clock < p.arrivalTime)
        cpuHead->clock = p.arrivalTime;

    if (p.startTime == 0)
        p.startTime = cpuHead->clock;
    else
        p.reStartTime = cpuHead->clock;
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

    while (departureCount < MAX_PROCESSES)
    {
        // if CPU is idle, then an arrival event can be scheduled
        if (!cpuHead->cpuBusy)
        {
            scheduleArrival();
            if (readyQ.empty())
                scheduleAllocation();
        }
        // if not idle, the process in the CPU can be scheduled for departure
        else
            scheduleDeparture();

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
    //TODO
}


void RR()
{
    //TODO
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

