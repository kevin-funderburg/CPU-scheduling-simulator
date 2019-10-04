/***
 * CPU Scheduler Simulation
 * Authors: Kevin Funderburg, Rob Murrat
 */
// TODO - Use 105 ms for the preemptive SRTF
/////////////////////////////////////////////////
#include <iostream>
#include <cstdlib>
#include <queue>
#include <list>
#include <cmath>

#include "event.h"
#include "process.h"
//#include "list.h"
using namespace std;
////////////////////////////////////////////////////////////////
#define MAX_PROCESSES 10000
////////////////////////////////////////////////////////////////
enum SchedulerType {FCFS = 1, SRTF = 2, RR = 3};
////////////////////////////////////////////////////////////////
//// function definition
void init();
int run_sim();
//void generate_report();

////event* newEvent(int, int, float);
void schedule_event(event * newEvent);


////int schedule_departure(struct event*);
//////////////////////////////////////////////////////////////////
//// Global variables
//event* head; // head of event queue
float _clock; // simulation clock, added underscore to make unique from system clock

class Scheduler
{
public:
    Scheduler();
    Scheduler(SchedulerType);
    void schedule(Process p, float clock);
    void analyze();
    void check();
    void removeFromCPU(Process);

    // data members
    Process CPU;
    SchedulerType schedulerType;
    typedef std::queue<Process, std::list<Process,
            std::allocator<Process> > >
            Pqueue;
    Pqueue readyQ;
    Process process_table[MAX_PROCESSES];
//    queue <Process> readyQ;
};

Scheduler::Scheduler() {}

Scheduler::Scheduler(SchedulerType st) : schedulerType(st), readyQ() {}

/* Prepares to schedule CPU */
void Scheduler::analyze()
{
    clog << "\t\tanalyze ready queue: " << CPU;
    if (readyQ.size() > 0) {
        clog << "\t(8)\tscheduling: " << readyQ.front().pid;
        schedule(readyQ.front(), _clock);
    }
}

/* Prepares to schedule CPU */
void Scheduler::check()
{
    clog << "\t\tcheck CPU: " << CPU;
    if (CPU != nullptr)
        analyze();
    else if (CPU->completed_CPU_time <= 0)
}

/* removes process from ready queue, sends to CPU */
void Scheduler::schedule(Process p, float clock)
{
    readyQ.pop();
    float currentTime = clock;
    p.departureTime = currentTime + p.burst;
    p.state = RUNNING;
    // scheduleDeparture
    CPU = &p;
}

void Scheduler::removeFromCPU(Process p)
{
    clog << "removing from CPU";
    // log departure information
}

class simulation
{
public:
    simulation(float a_rate, float a_st);
    void init();
    void run();
    float urand();
    float genexp(float lambda);
    int genID();
    void  scheduleEvent(event * newEvent);
    void scheduleArrival();
    void dispatchProcess();
    void scheduleDeparture();
//    void scheduleDeparture(Process p);

    // data members
    float clock;
    Scheduler scheduler;
    float arrivalRate;
    float avgServiceTime;
    event *lastArrival;
    int lastid;
    int p_count;
    int p_completed;
    Process p_table[MAX_PROCESSES];
    bool running;
protected:
    priority_queue<event*,
            vector<event *, allocator<event*> >,
            eventComparator> eventQueue;
};

simulation::simulation(float a_rate, float a_st)
        : clock(0.0), arrivalRate(a_rate), lastArrival(nullptr),
          lastid(0), p_count(0), p_completed(0), avgServiceTime(a_st),
          running(true), eventQueue()
{
    scheduler = Scheduler(FCFS);  // create scheduler
}

void simulation::init()
{
    // schedule first events
    scheduleArrival();
}

void simulation::scheduleEvent(event * newEvent) { eventQueue.push(newEvent); }

int simulation::genID() { return ++lastid; }

void simulation::scheduleArrival()
{
    float timeOffset = 0.0;
    if (lastArrival != nullptr)
        timeOffset = lastArrival->time;

    float newEventTime = genexp(arrivalRate) + timeOffset;
    clog << "event: ARRIVAL\t" << "newEventTime: [" << newEventTime
         << "]\tcurrent time: [" << clock
         << "]\tdiff: [" << newEventTime - clock << "]\n";

    dispatchProcess();
//    p_table[p_count] = p;

    event * newEvent = new event(ARRIVAL, newEventTime);
    lastArrival = newEvent;
    eventQueue.push(newEvent);
}

/* Remove from ready queue and give to CPU */
void simulation::dispatchProcess()
{
    clog << "event: DISPATCH\tcurrent time: [" << clock << "]\n";
    Process p = scheduler.readyQ.front();
    scheduler.CPU = &p;
    scheduler.readyQ.pop();

    event * newEvent = new event(IN_CPU, clock);

}

void simulation::scheduleDeparture()
//void simulation::scheduleDeparture(Process p)
{
    // this is a copy and paste from scheduler::schedule function
    // to see if it can work here
    Process p = scheduler.CPU;
    float currentTime = clock;
    p.state = RUNNING;
    p.departureTime = currentTime + p.burst;

    EventType tmp;
    if (p.remainingTime() > 0)
        tmp = LEAVE_CPU;
    else
        tmp = TERMINATED;

    event * newEvent = new event(tmp, p.departureTime);
    eventQueue.push(newEvent);
}

void simulation::run()
{
    init();
    event *nextEvent;
    nextEvent = eventQueue.top();
    clog << "nextEvent: " << nextEvent->type << "\t|\tevent queue length: [" << eventQueue.size() << "]\n";
    while (p_completed < MAX_PROCESSES)
    {
        switch (nextEvent->type)
        {
            case ARRIVAL:
            {   // create process and put into ready queue

                // schedule next process arrival
                scheduleArrival();
                break;
            }
            case IN_CPU:
            {
                scheduleDeparture();
                break;
            }
            case LEAVE_CPU:
            {
                event * newEvent = new event(IN_CPU, clock + scheduler.CPU.remainingTime());
                eventQueue.push(newEvent);

                Process p = scheduler.CPU;
                scheduler.readyQ.push(p);
                break;
            }
            case TERMINATED:
            {
                ;
                break;
            }
            default:
                cerr << "invalid event type";
        }
        eventQueue.pop();
        clog << "event removed\t|\tevent queue length: [" << eventQueue.size() << "]\n";
        clock = nextEvent->time;

//        event * nextEvent = eventQueue.top();
//        eventQueue.pop();
//        clock = nextEvent->time;
//        nextEvent->processEvent();
//        delete nextEvent;
    }
}

////////////////////////////////////////////////////////////////
// returns a random number between 0 and 1
float simulation::urand()
{
    return( (float) rand()/RAND_MAX );
}
/////////////////////////////////////////////////////////////
// returns a random number that follows an exp distribution
float simulation::genexp(float lambda)
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

//
static void show_usage()
{
    cerr << "Usage: sim [123] [average arrival rate] [average service time] [quantum interval]\n\n"
         << "Options:\n"
         << "\t1 : First-Come First-Served (FCFS)\n"
         << "\t2 : Shortest Remaining Time First (SRTF)\n"
         << "\t3 : Round Robin, with different quantum values (RR) (requires 4 arguments)\n";
}
//////////////////////////////////////////////////////////////////
int main(int argc, char *argv[] )
{
    if (argc < 3)
    {
        show_usage();
        return 1;
    }
    if (*argv[2] == 3 && argc < 5)      // RR requires the quantum arg
    {
        cerr << "Expected 4 arguments and got " << argc << endl;
        show_usage();
        return 1;
    }
    Scheduler scheduler(static_cast<SchedulerType>(stoi(argv[1])));// set scheduler algorithm
//    scheduler = static_cast<Scheduler>(stoi(argv[1]));  // set scheduler algorithm
    float lambda = 1 / (stof(argv[2]));                 // 1 / argument is the arrival process time
    float avgServiceTime = stof(argv[3]);
    if (argc == 5)
        float quantum = stof(argv[4]);

//    init();
    simulation sim(lambda,  avgServiceTime);
    sim.run();
//    run_sim();
//    generate_report();
    return 0;
}
//
