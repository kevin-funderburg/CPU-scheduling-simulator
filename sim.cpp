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
#include <cmath>

#include "event.h"
#include "process.h"
//#include "list.h"
using namespace std;
////////////////////////////////////////////////////////////////
#define SIZE 10
#define MAX_PROCESSES 10000
//#define PROCESS_CREATION 0
//#define DISPATCHED 1
//#define COMPLETION 2
//#define PREEMPTED 3
////////////////////////////////////////////////////////////////
//enum EventType {PROCESS_CREATION = 0, DISPATCHED = 1, COMPLETION = 2, PREEMPTED = 3};
//enum State {READY = 1, RUNNING = 2, TERMINATED = 3};
enum Scheduler {FCFS = 1, SRTF = 2, RR = 3};
////////////////////////////////////////////////////////////////
//struct process {
//    int pid;                // process ID
//    float arrivalTime;             // arrival time
//    float burst;            // service time
//    State state;            // process state
//    float remainingTime;    // time left for execution
//    float departureTime;
//};
// representation of a task
//struct event {
//    float time;
//    int   pid;
//    EventType   type;
//    struct event* next;
//};
////////////////////////////////////////////////////////////////
// function definition
void init();
int run_sim();
void generate_report();
int schedule_event(struct event*);
int delete_event(struct event* eve);
float urand();
float genexp(float);
//process newProcess(int);
//event* newEvent(int, int, float);
//int schedule_departure(struct event*);
////////////////////////////////////////////////////////////////
// Global variables
event* head; // head of event queue
float _clock; // simulation clock, added underscore to make unique from system clock
Scheduler scheduler;
////////////////////////////////////////////////////////////////
void init()
{
    // initialize all variables, states, and end conditions
    event *newEvent = new event;                // make next event
    newEvent->time = 0;                         // generate arrival time of next event
    newEvent->type = PROCESS_CREATION;
    schedule_event(newEvent);                   // schedule first event into event queue

//    event *dispatch = new event;                // make next creation event
//    dispatch->type = DISPATCHED;                // set type of next event
//    dispatch->pid = 0;                      // set id of process to be dispatched
//    dispatch->time = _clock + 1;                // generate dispatch time of next event's creation
//    schedule_event(dispatch);                   // schedule creation into event queue
}
////////////////////////////////////////////////////////////////
void generate_report()
{
    // output statistics
    clog << "outputting stats\n";
}

void analyze()
{

}
//////////////////////////////////////////////////////////////// //schedules an event in the future
int schedule_event(event *newEvent)
{
    // insert event in the event queue in its order of time
//    string evetype;
//    switch (newEvent->type) {
//        case PROCESS_CREATION:
//            evetype = "PROCESS_CREATION";
//            break;
//        case DISPATCHED:
//            evetype = "DISPATCHED";
//            break;
//        case COMPLETION:
//            evetype = "COMPLETION";
//            break;
//        case PREEMPTED:
//            evetype = "PREEMPTED";
//            break;
//        default:
//            break;
//    }
//    clog << "scheduling event: " << evetype << endl;

    if (head == nullptr || head->time >= newEvent->time)
    {
        newEvent->next = head;
        head = newEvent;
    }
    else
    {
        event *cursor = head;
        while (cursor->next != nullptr && cursor->next->time < newEvent->time)
            cursor = cursor->next;
        newEvent->next = cursor->next;
        cursor->next = newEvent;
    }
    return 0;
}

int delete_event(event *eve)
{
    return 0;
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

//int schedule_departure(event *event)
//{
//    return 0;
//}
////////////////////////////////////////////////////////////
int run_sim()
{
    int p_count = 0,            // total count of processes
        p_completed = 0;        // count of processes who have completed processing

    event *lastArrival = nullptr;

    event *eve;
    process p_table[MAX_PROCESSES+1000];      // table containing process data

    while (p_completed < MAX_PROCESSES)
    {
        eve = head;
        _clock = eve->time;
        cout << "MILLISECONDS: " << _clock << endl;
        switch (eve->type)
        {
            case PROCESS_CREATION:
            {
                cout << "\tevent type:\t\t\tPROCESS_CREATION\n";

                float timeOffset = 0.0;
                if (lastArrival != nullptr)
                    timeOffset = lastArrival->time;

                process p;
                p.pid = p_count;
                p.arrivalTime = _clock;
                p.burst = p.remainingTime = genexp(0.06);
                p.state = READY;
                ready_q[p_count] = p;
                clog << "process data:\t" << "\tid: " << p.pid << "\tstate: " << p.state << endl;

                p_table[p_count] = p;                       // add new process to process table
                p_count++;                                  // increment total process count

                event *dispatch = new event;                // make next creation event
                dispatch->type = DISPATCHED;                // set type of next event
                dispatch->pid = p.pid;                      // set id of process to be dispatched
                dispatch->time = _clock + 1;                // generate dispatch time of next event's creation
                schedule_event(dispatch);                   // schedule creation into event queue

                event *creation = new event;                // make next creation event
                creation->type = PROCESS_CREATION;          // set type of next event
                creation->time = timeOffset + genexp(0.06);     // generate creation time of next event's creation
                schedule_event(creation);                   // schedule creation into event queue

                lastArrival = creation;
                break;
            }
            case DISPATCHED:
            {
                cout << "\tevent type:\t\t\tDISPATCHED\n";

                p_table[eve->pid].state = RUNNING;          // set process in process table state to running

                switch (scheduler)
                {
                    case FCFS:
                    {
                        for (int i = 0; i < MAX_PROCESSES; ++i) {
                            if (p_table[i].state == READY)
                                process pr = p_table[i];
                        }
                        /**
                         * under FCFS, we know exactly when this process would finish, so we can schedule a
                         * completion event in the future and place it in the Event Queue
                         **/

                        p_table[eve->pid].remainingTime = 0;    // FCFS processes to completion, so process has no remaining time

                        event *completion = new event;                // create new event
                        completion->pid = eve->pid;                   // set event id to process id
                        completion->time = _clock + p_table[eve->pid].burst;  // event time is the current time + burst time of process
                        completion->type = COMPLETION;                         // set event type to leave the CPU
                        schedule_event(completion);

                        event *dispatch = new event;                // make next creation event
                        dispatch->pid = p_count + 1;
                        dispatch->type = DISPATCHED;                // set type of next event
                        // the dispatch time of the next ready process is 1 ms after this process has completed
                        dispatch->time = _clock + p_table[eve->pid].burst + 1;
                        schedule_event(dispatch);                   // schedule creation into event queue

                        _clock += p_table[eve->pid].burst;      // clock will be set to the amount of time it takes to complete processing
                    }
                        break;
                    case SRTF:
                        // something
                        break;
                    case RR:
                        // something
                        break;
                    default:
                        cerr << "invalid algorithm" << endl;
                        return 1;
                }

                break;
            }
            case COMPLETION:
            {
                cout << "\tevent type:\t\t\tCOMPLETION\n";

                if (p_table[eve->pid].remainingTime == 0)   // remaining processing time is 0
                {
                    p_table[eve->pid].state = TERMINATED;
                    p_table[eve->pid].departureTime = _clock;
                    p_completed++;  // end condition
//                    delete_event(eve);
                }
                else    // remaining process time > 0, so put back into ready queue
                {
                    p_table[eve->pid].state = READY;

                    event *newEvent;
                    newEvent = new event;
                    newEvent->pid = eve->pid;
                    newEvent->type = DISPATCHED;
                    newEvent->time = _clock;

                    schedule_event(newEvent);
                }
                break;
            }
            case PREEMPTED:
            {
                // schedule a dispatch event
            }
            default:
                cerr << "invalid event type\n";   // error
                return 1;
        }
        head = eve->next;
        delete eve;
        eve = nullptr;

        cout << "\tPROCESSES MADE\t\t[" << p_count << "]\n\tPROCESSES COMPLETED\t[" << p_completed << "]\n";
    }
    return 0;
}

process newProcess(int index)
{
    process p;
    p.pid = p.arrivalTime = index;
    p.burst = p.remainingTime = genexp(0.06);
    p.state = READY;
    return p;
}

//event* newEvent(int pid, int type, float time)
//{
//    event *newEvent = new event;
//    newEvent->pid = pid;
//    newEvent->type = type;
//    newEvent->time = time;
//    return newEvent;
//}

int schedule_departure(event *eve)
{
    return 0;   // PLACEHOLDER
}

static void show_usage()
{
    cerr << "Usage: sim [123] [average arrival rate] [average service time] [quantum interval]\n\n"
         << "Options:\n"
         << "\t1 : First-Come First-Served (FCFS)\n"
         << "\t2 : Shortest Remaining Time First (SRTF)\n"
         << "\t3 : Round Robin, with different quantum values (RR) (requires 4 arguments)\n";
}
////////////////////////////////////////////////////////////////
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

    init();
    run_sim();
    generate_report();
    return 0;
}

