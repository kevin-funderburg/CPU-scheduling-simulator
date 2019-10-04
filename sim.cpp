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
#include "list.h"
using namespace std;
////////////////////////////////////////////////////////////////
#define SIZE 10
#define MAX_PROCESSES 10000
#define ARRIVAL 0
#define TIMESLICE 1
#define DEPARTURE 2
////////////////////////////////////////////////////////////////
enum State {NEW = 1, READY = 2, WAITING = 3, RUNNING = 4, TERMINATED = 5};
enum Scheduler {FCFS = 1, SRTF = 2, RR = 3};
////////////////////////////////////////////////////////////////
struct process {
    int pid;                // process ID
    int time;               // arrival time
    float burst;            // service time
    State state;            // process state
    float remainingTime;    // time left for execution
    // TODO - add stats
};
////////////////////////////////////////////////////////////////
// function definition
void init();
int run_sim();
void generate_report();
int schedule_event(struct event*);
int delete_event(struct event* eve);
float urand();
float genexp(float);
process newProcess(int);
event* newEvent(int, int, float);
int process_event2(struct event* eve);
////////////////////////////////////////////////////////////////
// Global variables
event* head; // head of event queue
float _clock; // simulation clock, added underscore to make unique from system clock
Scheduler scheduler;
////////////////////////////////////////////////////////////////
void init()
{
    // initialize all variables, states, and end conditions

//    event *newEvent = newEvent(0, ARRIVAL, 0);                // make next event
    event *newEvent = new event;                // make next event
    newEvent->time = 0;                         // generate arrival time of next event
    newEvent->type = ARRIVAL;
    schedule_event(newEvent);                   // schedule first event into event queue
}
////////////////////////////////////////////////////////////////
void generate_report()
{
    // output statistics
    clog << "outputting stats\n";
}
//////////////////////////////////////////////////////////////// //schedules an event in the future
int schedule_event(event *newEvent)
{
    // insert event in the event queue in its order of time
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
////////////////////////////////////////////////////////////
int run_sim()
{
    int p_count = 0,            // total count of processes
        p_completed = 0;        // count of processes who have completed processing

    event *eve;
    process p_table[MAX_PROCESSES+100];      // table containing process data

    while (p_completed < MAX_PROCESSES)
    {
        eve = head;
        cout << "eve->time: " << eve->time << endl;
        _clock = eve->time;
        switch (eve->type)
        {
            case ARRIVAL:
            {
                cout << "event type: ARRIVAL\n";

                process p = newProcess(p_count);            // make new process
                p_table[p_count] = p;                       // add new process to process table
                eve->pid = p.pid;                           // set event id to new process id - MAYBE DELETE
                p_count++;                                  // increment total process count

                /* NOTE
                 * Seems as if a TIMESLICE events needs to be added to the queue for
                 * the process just created THEN another arrival event needs to be added to the queue
                 */
//                event *timeslice = new event;
//                timeslice->type = TIMESLICE;
//                timeslice->time = eve->time;
//                schedule_event(timeslice);

                event *arrival = new event;                // make next arrival event
                arrival->type = ARRIVAL;                   // set type of next event
                arrival->time = eve->time + genexp(0.06);  // generate arrival time of next event's arrival
                schedule_event(arrival);                   // schedule arrival into event queue
                break;
            }
            case TIMESLICE:
            {
                cout << "event type: TIMESLICE\n";

                p_table[eve->pid].state = RUNNING;          // set process in process table state to running

                event *newEvent = new event;                // create new event
                newEvent->pid = eve->pid;                   // set event id to process id

                switch (scheduler)
                {
                    case FCFS:
                        p_table[eve->pid].remainingTime = 0;    // FCFS processes to completion, so process has no remaining time
                        /**
                         * under FCFS, we know exactly when this process would finish, so we can schedule a
                         * completion event in the future and place it in the Event Queue
                         **/
                        newEvent->time = _clock + p_table[eve->pid].burst;  // event time is the current time + burst time of process
                        newEvent->type = DEPARTURE;                         // set event type to leave the CPU
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

                schedule_event(newEvent);
                break;
            }
            case DEPARTURE: //can mean either complete process or return process to ready queue
            {
                cout << "event type: DEPARTURE\n";

                if (p_table[eve->pid].remainingTime <= 0)   // remaining processing time is 0
                {
                    p_table[eve->pid].state = TERMINATED;
                    p_completed++;  // end condition
                    cout << "number of processes completed: " << p_completed << endl;
                    delete_event(eve);
                }
                else    // remaining process time > 0, so put back into ready queue
                {
                    p_table[eve->pid].state = READY;

                    event *newEvent;
                    newEvent = new event;
                    newEvent->pid = eve->pid;
                    newEvent->type = TIMESLICE;
                    newEvent->time = _clock;

                    schedule_event(newEvent);
                }
                break;
            }
            default:
                cerr << "invalid event type\n";   // error
                return 1;
        }

        head = eve->next;
        delete eve;
        eve = nullptr;
    }
    return 0;
}

process newProcess(int index)
{
    process p;
    p.pid = index;
    p.time = index;
    p.burst = p.remainingTime = genexp(0.06);
    p.state = NEW;
    return p;
}

event* newEvent(int pid, int type, float time)
{
    event *newEvent = new event;
    newEvent->pid = pid;
    newEvent->type = type;
    newEvent->time = time;
    return newEvent;
}

int process_event2(struct event* eve)
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
    // avgnumprocesses 1 / lambda * num_proceesses i think
    float lambda = 1 / (stoi(argv[2]));                 // 1 / argument is the arrival process time
    float avgServiceTime = stof(argv[3]);
    if (argc == 5)
        float quantum = stof(argv[4]);

    init();
    run_sim();
    generate_report();
    return 0;
}

