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

#define SIZE 10
////////////////////////////////////////////////////////////////
// TODO - need to clarify these events
#define ARRIVAL 0
#define TIMESLICE 1
#define DEPARTURE 2     // occurs when using algo 2, the process has used all its CPU time and is added back to the ready queue

enum State {NEW = 1, READY = 2, WAITING = 3, RUNNING = 4, TERMINATED = 5};

////////////////////////////////////////////////////////////////     //event structure


struct process {
    int pid;            // process ID
    int time;           // arrival time
    int burst;          // service time
    State state;        // process state
    int remainingTime;  // time left for execution
    // add stats
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
event newEvent(int, int, float);
int process_event2(struct event* eve);

////////////////////////////////////////////////////////////////
//Global variables
event* head; // head of event queue
event* readyq;
float _clock; // simulation clock, added underscore to make unique from system clock
//int process_count;

////////////////////////////////////////////////////////////////
void init()
{
    // initialize all variables, states, and end conditions
    head = new event;
    head->time = 0;
    head->type = ARRIVAL;
    head->next = nullptr;
//    ///
//    /// NOTE - not sure if this can be an array when initializing or a linked list
//    /// so this is commented out below, but we may use it
//// add a process ID
//// process arrives at a certain time
//    event processes[SIZE];
//    for (int i = 0; i < SIZE; ++i) {
//        processes[i].time = i;
//        processes[i].burst = genexp(0.06);
//    }

    ////
    //// This is creating the processes in a linked list
    ////
//    event *cursor;
//    cursor = head;
//    for (int i = 0; i < SIZE; ++i) {
//        cursor->time = i;
//        cursor->burst = genexp(0.06);
//        cursor->next = new event;
//        cursor = cursor->next;
//    }
//    cursor->next = nullptr;

    // schedule first events
//    schedule_event(head);
}
////////////////////////////////////////////////////////////////
void generate_report()
{
    // output statistics
}
//////////////////////////////////////////////////////////////// //schedules an event in the future
int schedule_event(event *newEvent)
{
    // insert event in the event queue in its order of time

    /**
     * This is supposed to add an event to the queue based on the time
     * It still needs work
    event* cursor;
    cursor = head;

    bool inserted = false;

    if (head == nullptr)
        head = new event;
    while (!inserted) {
        if (cursor->time < newEvent->time)
        {
            if (cursor->next == nullptr) {  // list length is 1
                cursor->next = newEvent;
                return 0;
            }
            else if (cursor->next->time > newEvent->time)
            {
                event* temp = new event;
            }
        }
    }
     **/
    event *temp = head;
    // this appends the new even to the queue, needs to be adjusted for priority,
    newEvent->next = nullptr;
    while (temp->next != nullptr) {
        temp = temp->next;
    }
    temp->next = newEvent;
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
    event *eve;
    int p_count = 0;
    int p_completed = 0;
    process p_table[SIZE];
    while (p_completed < SIZE)
    {
        eve = head;
        cout << "eve->time: " << eve->time << endl;
        _clock = eve->time;
        switch (eve->type)
        {
            case ARRIVAL:
            {
                process p = newProcess(p_count);    // make new process
                p_table[p_count] = p;               // add new process to process table
                eve->pid = p.pid;                   // set event id to new process id
                p_count++;                          // increment total process count

                event *newEvent;                    // schedule next arrival
                newEvent = new event;
                newEvent->time = eve->time + 3;     // TODO - i put 3 here but this should be random
                schedule_event(newEvent);
                break;
            }
            case TIMESLICE:
            {
                // under FCFS, we know exactly when this process would finish, so we can schedule a
                // completion event in the future and place it in the Event Queue
                p_table[eve->pid].state = RUNNING;
                p_table[eve->pid].remainingTime = 0;

                event newEvent = newEvent(eve->pid, DEPARTURE, _clock + p_table[eve->pid].burst);
//                event *newEvent;                    // schedule next arrival
//                newEvent = new event;
//                newEvent->time = _clock + p_table[eve->pid].burst;
//                newEvent->pid = eve->pid;
//                newEvent->type = DEPARTURE;
                //schedule_event(newEvent);
                break;
            }
            case DEPARTURE:
            {
                if (p_table[eve->pid].remainingTime == 0) {
                    p_table[eve->pid].state = TERMINATED;
                    delete_event(eve);
                } else {
                    p_table[eve->pid].state = READY;

                    event *newEvent = newEvent(eve->pid, TIMESLICE, _clock);
//                    newEvent = new event;
//                    newEvent->pid = eve->pid;
//                    newEvent->type = TIMESLICE;
//                    newEvent->time = _clock;

                    schedule_event(newEvent);
                }
                p_completed++;                      // increment processes completed (end condition)
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

event newEvent(int pid, int type, float time)
{
    event newEvent;
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
    clog << "starting\n";
    if (argc < 3) {
        show_usage();
        return 1;
    }
    // RR requires the quantum arg
    if (*argv[2] == 3 && argc < 5) {
        cerr << "Expected 4 arguments and got " << argc << endl;
        show_usage();
        return 1;
    }
    int scheduler = stoi(argv[1]);
    float lambda = 1 / (stoi(argv[2]));   // 1 / argument is the arrival process time
    float avgServiceTime = stof(argv[3]);
    if (argc == 5)
        float quantum = stof(argv[4]);

    init();
    run_sim();
    generate_report();
    return 0;
}

