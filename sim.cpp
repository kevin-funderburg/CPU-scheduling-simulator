#include <iostream>
#include <cstdlib>
#include <cmath>
//#include <ctime>
using namespace std;

#define SIZE 10000
////////////////////////////////////////////////////////////////
// TODO - need to clarify these events
#define ARRIVE 1
#define COMPLETE 2
#define SLICE 3

// .. add more events
// NOTE - not sure if this is the best way
enum State {NEW = 1, READY = 2, WAITING = 3, RUNNING = 4, TERMINATED = 5};

////////////////////////////////////////////////////////////////     //event structure
struct event{
    float time;
    int   type;
    int   priority;
    int   burst;
    struct event* next;
};

////////////////////////////////////////////////////////////////
// function definition
void init();
int run_sim();
void generate_report();
int schedule_event(struct event*);
int delete_event(struct event* eve);
int process_event2(struct event* eve);

////////////////////////////////////////////////////////////////
//Global variables
struct event* head; // head of event queue
float _clock; // simulation clock, added underscore to make unique from system clock
int process_count;

////////////////////////////////////////////////////////////////
void init()
{
    // initialize all variables, states, and end conditions
    head = new event;
    head->time = 0.0;
    event processes[SIZE];
    process_count = 0;    // end condition, ends when it equals 10000
    // schedule first events
    schedule_event(head);
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
    event* temp;
    temp = head;

    // this appends to the list, not sure if that's what's needed
    newEvent->next = nullptr;
    while(temp->next != nullptr)
        temp = temp->next;
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
//    while (!end_condition)
    while(process_count < 10000)
    {
        eve = head;
        cout << "eve->time: " << eve->time << endl;
        _clock = eve->time;
        switch (eve->type)
        {
            case ARRIVE:
                schedule_event(eve);
                break;
            case COMPLETE:
                delete_event(eve);
                break;

                // add more events

            default:
                cerr << "invalid event type\n";   // error
        }

        head = eve->next;
        delete eve;
        eve = nullptr;
        process_count++;
    }
    return 0;
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
    int lambda = stoi(argv[2]);
    float avgServiceTime = stof(argv[3]);
    if (argc == 5)
        float quantum = stof(argv[4]);

    init();
    run_sim();
    generate_report();
    return 0;
}

