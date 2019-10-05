/***
 * CPU Scheduler Simulation
 * Authors: Kevin Funderburg, Rob Murrat
 */
/////////////////////////////////////////////////
// TODO - Add a Process Ready Queue
// ASKPROF - can we use priority queue from stdlib?
// Use 105 ms for the preemptive _SRTF
/////////////////////////////////////////////////
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <queue>
#include "event.h"
#include "list.h"
#include "header.h"
using namespace std;
///////////////////////////////////////////////////////////////

// NOTES
// avgnumprocesses 1 / lambda * num_proceesses i think



////////////////////////////////////////////////////////////////
// Global variables
priority_queue<event*,
        vector<event *, allocator<event*> >,
        eventComparator> eventQueue;
//event* head; // head of event queue
float _clock; // simulation clock, added underscore to make unique from system clock
Scheduler scheduler;
bool CPUbusy;
int lambda;
float avgArrivalTime;
float avgServiceTime;
cpuNode *cpuHead;
readyQNode *readyQHead;
////////////////////////////////////////////////////////////////

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
    cpuHead = new cpuNode;
    cpuHead->clock = 0.0;
    cpuHead->cpuBusy = false;
}
////////////////////////////////////////////////////////////////
void generate_report()
{
    // output statistics
    clog << "outputting stats\n";
}

void schedule_event_eventQ(event *newEvent)
{
    eventQueue.push(newEvent);
};

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

}

void scheduleDeparture()
{

}

void scheduleAllocation()
{
    event *newAllocation = new event;
    procListNode *nextProcess;
    switch (scheduler)
    {
        case _FCFS:
            nextProcess = readyQHead->pLink;
            break;
        case _SRTF:
            break;
        case _RR:
            break;
        default:
            cerr << "invalid scheduler\n";
    }
    if (cpuHead->clock < nextProcess->p.arrivalTime)
        newAllocation->time = nextProcess->p.arrivalTime;
    else
        newAllocation->time = cpuHead->clock;

    newAllocation->type = LEAVE_CPU;
    schedule_event_eventQ(newAllocation);

}

void handleArrival()
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
        case _FCFS:
            cout << "Scheduling as FCFS\n\n";
            FCFS();
            break;
        case _SRTF:
            cout << "Scheduling as SRTF\n\n";
            SRTF();
            break;
        case _RR:
            cout << "Scheduling as RR\n\n";
            RR();
            break;
        default:
            cerr << "invalid scheduler\n";
            return 1;
    }
    return 0;
}

process newProcess(int index)
{
    process p;
    p.pid = index;
    p.arrivalTime = index;
    p.burst = p.remainingTime = genexp(lambda);
    p.state = READY;
    return p;
}

event* newEvent(int pid, EventType type, float time)
{
    event *newEvent = new event;
    newEvent->type = type;
    newEvent->time = time;
    return newEvent;
}

void FCFS() {
    int p_completed = 0;
    int p_count = 0;
    int allocationCount = 0;

    while (p_completed < MAX_PROCESSES)
    {
        if (!cpuHead->cpuBusy)
        {
            scheduleArrival();
            if (readyQHead == nullptr)
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
            case IN_CPU:
                ;
                break;
            case LEAVE_CPU:
                ;
                break;
            case COMPLETION:
                ;
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

