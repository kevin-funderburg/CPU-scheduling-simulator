/***
 * CPU Scheduler Simulation
 * Authors: Kevin Funderburg, Rob Murrat
 */
/////////////////////////////////////////////////
// TODO - Add a Process Ready Queue
// ASKPROF - can we use priority queue from stdlib?
// Use 105 ms for the preemptive SRTF
/////////////////////////////////////////////////
#include <iostream>
#include <cstdlib>
#include <queue>
#include <cmath>

#include "event.h"
#include "process.h"
//#include "list.h"
using namespace std;
////////////////////////////////////////////////////////////////
#define MAX_PROCESSES 10000
////////////////////////////////////////////////////////////////
//enum EventType {PROCESS_CREATION = 0, DISPATCHED = 1, COMPLETION = 2, PREEMPTED = 3};
//enum State {READY = 1, RUNNING = 2, TERMINATED = 3};
enum Scheduler {FCFS = 1, SRTF = 2, RR = 3};
////////////////////////////////////////////////////////////////
//// function definition
void init();
int run_sim();
//void generate_report();
//int schedule_event(event event);
float urand();
float genexp(float);
////event* newEvent(int, int, float);
void schedule_event(event * newEvent);
////int schedule_departure(struct event*);
//////////////////////////////////////////////////////////////////
//// Global variables
//event* head; // head of event queue
priority_queue<event*,
        std::vector<event *, std::allocator<event*> >,
        eventComparator> eventQueue;
float _clock; // simulation clock, added underscore to make unique from system clock
Scheduler scheduler;
//
//////////////////////////////////////////////////////////////////
void init()
{
//    // initialize all variables, states, and end conditions
//    event newEvent = event(PROCESS_CREATION, 0.0);
//    eventQueue.push(&newEvent);
//    schedule_event(&newEvent);

}
//////////////////////////////////////////////////////////////////
//void generate_report()
//{
//    // output statistics
//    clog << "outputting stats\n";
//}
//
//void analyze()
//{
//
//}
////////////////////////////////////////////////////////////////// //schedules an event in the future
void schedule_event(event * newEvent)
{
    eventQueue.push(newEvent);
}

class simulation
{
public:
    simulation () : clock(0), eventQueue()
    {}
    void run();
    void  scheduleEvent(event * newEvent) {
        eventQueue.push(newEvent);
    }
    float clock;
protected:
    std::priority_queue<event*,
            std::vector<event *, std::allocator<event*> >,
            eventComparator> eventQueue;
};

void simulation::run()
{
    while (!eventQueue.empty()) {
        event * nextEvent = eventQueue.top();
        eventQueue.pop();
        clock = nextEvent->time;
        nextEvent->processEvent();
        delete nextEvent;
    }
}

//
//int delete_event(event *eve)
//{
//    return 0;
//}
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

////int schedule_departure(event *event)
////{
////    return 0;
////}
//////////////////////////////////////////////////////////////
int run_sim()
{
    int p_count = 0,            // total count of processes
        p_completed = 0;        // count of processes who have completed processing

    event *lastArrival = nullptr;

    event *eve;
    Process p_table[MAX_PROCESSES+1000];      // table containing process data

    while (p_completed < MAX_PROCESSES)
    {
//        eve = head;
        _clock = eventQueue.top()->time;
        cout << "MILLISECONDS: " << _clock << endl;
//        switch (eve->type)
//        {
//            case PROCESS_CREATION:
//            {
//                cout << "\tevent type:\t\t\tPROCESS_CREATION\n";
//
//                float timeOffset = 0.0;
//                if (lastArrival != nullptr)
//                    timeOffset = lastArrival->time;
//
//                Process p;
//                p.pid = p_count;
//                p.arrivalTime = _clock;
//                p.burst = p.remainingTime = genexp(0.06);
//                p.state = READY;
//                ready_q[p_count] = p;
//                clog << "process data:\t" << "\tid: " << p.pid << "\tstate: " << p.state << endl;
//
//                p_table[p_count] = p;                       // add new process to process table
//                p_count++;                                  // increment total process count
//
//                event *dispatch = new event;                // make next creation event
//                dispatch->type = DISPATCHED;                // set type of next event
//                dispatch->pid = p.pid;                      // set id of process to be dispatched
//                dispatch->time = _clock + 1;                // generate dispatch time of next event's creation
//                schedule_event(dispatch);                   // schedule creation into event queue
//
//                event *creation = new event;                // make next creation event
//                creation->type = PROCESS_CREATION;          // set type of next event
//                creation->time = timeOffset + genexp(0.06);     // generate creation time of next event's creation
//                schedule_event(creation);                   // schedule creation into event queue
//
//                lastArrival = creation;
//                break;
//            }
//            case DISPATCHED:
//            {
//                cout << "\tevent type:\t\t\tDISPATCHED\n";
//
//                p_table[eve->pid].state = RUNNING;          // set process in process table state to running
//
//                switch (scheduler)
//                {
//                    case FCFS:
//                    {
//                        for (int i = 0; i < MAX_PROCESSES; ++i) {
//                            if (p_table[i].state == READY)
//                                process pr = p_table[i];
//                        }
//                        /**
//                         * under FCFS, we know exactly when this process would finish, so we can schedule a
//                         * completion event in the future and place it in the Event Queue
//                         **/
//
//                        p_table[eve->pid].remainingTime = 0;    // FCFS processes to completion, so process has no remaining time
//
//                        event *completion = new event;                // create new event
//                        completion->pid = eve->pid;                   // set event id to process id
//                        completion->time = _clock + p_table[eve->pid].burst;  // event time is the current time + burst time of process
//                        completion->type = COMPLETION;                         // set event type to leave the CPU
//                        schedule_event(completion);
//
//                        event *dispatch = new event;                // make next creation event
//                        dispatch->pid = p_count + 1;
//                        dispatch->type = DISPATCHED;                // set type of next event
//                        // the dispatch time of the next ready process is 1 ms after this process has completed
//                        dispatch->time = _clock + p_table[eve->pid].burst + 1;
//                        schedule_event(dispatch);                   // schedule creation into event queue
//
//                        _clock += p_table[eve->pid].burst;      // clock will be set to the amount of time it takes to complete processing
//                    }
//                        break;
//                    case SRTF:
//                        // something
//                        break;
//                    case RR:
//                        // something
//                        break;
//                    default:
//                        cerr << "invalid algorithm" << endl;
//                        return 1;
//                }
//
//                break;
//            }
//            case COMPLETION:
//            {
//                cout << "\tevent type:\t\t\tCOMPLETION\n";
//
//                if (p_table[eve->pid].remainingTime == 0)   // remaining processing time is 0
//                {
//                    p_table[eve->pid].state = TERMINATED;
//                    p_table[eve->pid].departureTime = _clock;
//                    p_completed++;  // end condition
////                    delete_event(eve);
//                }
//                else    // remaining process time > 0, so put back into ready queue
//                {
//                    p_table[eve->pid].state = READY;
//
//                    event *newEvent;
//                    newEvent = new event;
//                    newEvent->pid = eve->pid;
//                    newEvent->type = DISPATCHED;
//                    newEvent->time = _clock;
//
//                    schedule_event(newEvent);
//                }
//                break;
//            }
//            case PREEMPTED:
//            {
//                // schedule a dispatch event
//            }
//            default:
//                cerr << "invalid event type\n";   // error
//                return 1;
//        }
//        head = eve->next;
//        delete eve;
//        eve = nullptr;
//
//        cout << "\tPROCESSES MADE\t\t[" << p_count << "]\n\tPROCESSES COMPLETED\t[" << p_completed << "]\n";
    }
    return 0;
}

//int schedule_departure(event *eve)
//{
//    return 0;   // PLACEHOLDER
//}
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
    scheduler = static_cast<Scheduler>(stoi(argv[1]));  // set scheduler algorithm
    float lambda = 1 / (stoi(argv[2]));                 // 1 / argument is the arrival process time
    float avgServiceTime = stof(argv[3]);
    if (argc == 5)
        float quantum = stof(argv[4]);

//    init();
    simulation sim;
    sim.run();
//    run_sim();
//    generate_report();
    return 0;
}
//
