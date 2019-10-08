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
    _clock = 0.0;

    cpuHead = new cpuNode;
    cpuHead->clock = 0.0;
    cpuHead->cpuBusy = false;
    cpuHead->pLink = nullptr;
    cpuHead->departureScheduled = false;

    process p;
    p.pid = 0;
    p.arrivalTime = genexp((float)lambda);
    p.state = READY;
    p.startTime = p.reStartTime = p.completionTime = 0.0;
    p.burst = genexp(avgServiceTime);
    p.remainingTime = p.burst;
    pList.push_back(p);
    p_table[0] = p;
    lastid = 0;

    event *newArrival = new event;
    newArrival->time = p.arrivalTime;
    newArrival->type = ARRIVE;
    newArrival->pid = p.pid;
    lastArrival = newArrival;
    addToEventQ(newArrival);
}


////////////////////////////////////////////////////////////////
void generate_report()
{
    //statistics needed:
    //total throughput
    //total
    /*
    for (int i; i <= p_table[].size(); i++)
    {
        process temp = p_table[i];
    }


     */

    // output statistics
    clog << "outputting stats\n";
}


int genID() { return ++lastid; }


void addToEventQ(event *newEvent)
{
    debugging(newEvent);
    eventQ.push(newEvent);
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

    clog << "\n\tQUEUE SIZES:"
         << "\tevent: [" << eventQ.size()
         << "]\tready: [" << readyQ.size()
         << "] process list: [" << pList.size() << "]\n";
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
//    float timeOffset = lastArrival->time + genexp((float)lambda);
    p.arrivalTime = p_table[p.pid-1].arrivalTime + genexp((float)lambda);
//    p.arrivalTime = cpuHead->clock + lastArrival->time + genexp((float)lambda);
    p.state = READY;
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
    addToEventQ(newArrival);
}


void handleArrival()
{
    clog << "DEBUG: handleArrival()\n\ttime: " << cpuHead->clock << endl;
    process &p = p_table[eventQ.top()->pid];
    readyQ.push_front(p);       // add process to end of ready queue
    eventQ.pop();               // remove first event
}


void scheduleDeparture()
{
    clog << "DEBUG [" << cpuHead->clock << "] - scheduleDeparture()\n";

    event *newDeparture = new event;
    newDeparture->type = DEPARTURE;
    newDeparture->pid = cpuHead->pid;
    cpuHead->departureScheduled = true;

    process &p = p_table[newDeparture->pid];

    if (scheduler == _FCFS)
    {
        newDeparture->time = p.startTime + p.remainingTime;
        clog << "\tnew departure event time set to: [" << newDeparture->time << "]\twith pid [" << newDeparture->pid << "]\n";
    }
    else if (scheduler == _SRTF)
    {
        // TODO
    }

    addToEventQ(newDeparture);
}


void handleDeparture()
{
    clog << "DEBUG [" << cpuHead->clock << "]: handleDeparture()\n";

    cpuHead->clock = eventQ.top()->time;

    process &p = p_table[eventQ.top()->pid];
    p.completionTime = cpuHead->clock;
    p.remainingTime = 0.0;

    cpuHead->pLink = nullptr;
    cpuHead->cpuBusy = false;
    cpuHead->pid = NULL;
    cpuHead->departureScheduled = false;

    eventQ.pop();
    clog << "\t[" << cpuHead->clock << "]\tdeparture event was popped from queue\n";
}


void scheduleAllocation()
{
    clog << "DEBUG [" << cpuHead->clock << "] - scheduleAllocation()\n";

    event *newAllocation = new event;
    process nextProcess;

//    process currentProcess = p_table[eventQ.top()->pid];
//    process nextProcess = p_table[eventQ.top()->pid + 1];
    switch (scheduler)
    {
        case _FCFS:
            if (readyQ.empty())
                nextProcess = p_table[(eventQ.top()->pid+1)];
            else
                nextProcess = readyQ[1];
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

    newAllocation->type = ALLOCATION;
    newAllocation->pid = nextProcess.pid;

    addToEventQ(newAllocation);
}


void handleAllocation()
{
    clog << "DEBUG [" << cpuHead->clock << "] - handleAllocation()\n";
    clog << "\tcpuPid before: [" << cpuHead->pid << "]\n";

    process &p = p_table[eventQ.top()->pid];

    // FIXME - this is resetting  back to old ids, biggest problem right now
    cpuHead->pid = p.pid;
    clog << "\tcpuPid after: [" << cpuHead->pid << "]\n";

    if (scheduler == _SRTF || scheduler == _RR)
    {
        //TODO
    }

    readyQ.pop_front(); // remove front ready process
    eventQ.pop();   // remove top event

    cpuHead->cpuBusy = true;

    // this represents the cpu starting to work on the process at its arrival time
    if (cpuHead->clock < p.arrivalTime)
        cpuHead->clock = p.arrivalTime;
    // the start time of processing is now set
    if (p.startTime == 0)
        p.startTime = cpuHead->clock;
    else
        p.reStartTime = cpuHead->clock;

    cout << "\t\tcpu time: " << cpuHead->clock << endl;
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


void FCFS()
{
    int p_count = 0;
    int departureCount = 0;
    int allocationCount = 0;

    while (departureCount < MAX_PROCESSES)
    {
        clog << "\n\nDEBUG [" << cpuHead->clock << "] ----------------- loop start\n\n";
        _clock = eventQ.top()->time;
        // if CPU is idle, then an arrival event can be scheduled
        if (!cpuHead->cpuBusy)
        {
            scheduleArrival();
            if (readyQ.empty())
                scheduleAllocation();   // schedule process to be given to CPU
        }
        else // process in the CPU can be scheduled for departure
        {
            if (!cpuHead->departureScheduled)
                scheduleDeparture();
        }


        switch (eventQ.top()->type)
        {
            case ARRIVE:
                handleArrival();
                p_count++;
                break;
            case DEPARTURE:
                handleDeparture();
                departureCount++;
                break;
            case ALLOCATION:
                handleAllocation();
                allocationCount++;
                break;
            default:
                cerr << "invalid type";
        }
    }
    cout << "Arrival Count: " << p_count << endl;
    cout << "Departure Count: " << departureCount << endl;
    cout << "Allocation Count: " << allocationCount << endl;
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

