#include <iostream>
#include <cstdlib>
#include <cmath>
//#include <ctime>

using namespace std;
// TODO - add states of the CPU to the system
////////////////////////////////////////////////////////////////
// sample events
/// TODO - need to define what kind of events there are
#define EVENT1 1
#define EVENT2 2
// .. add more events

////////////////////////////////////////////////////////////////     //event structure
struct event{
    float time;
    int   type;
    // add more fields
    struct event* next;
};

////////////////////////////////////////////////////////////////
// function definition
void init();
int run_sim();
void generate_report();
int schedule_event(struct event*);
int process_event1(struct event* eve);
int process_event2(struct event* eve);

////////////////////////////////////////////////////////////////
//Global variables
struct event* head; // head of event queue
float _clock; // simulation clock, added underscore to make unique from system clock
int end_condition;

////////////////////////////////////////////////////////////////
void init()
{
    // initialize all variables, states, and end conditions
    head->time = 0.0;   // set queue time and clock to 0
    end_condition = 10000;      // 10000 processes

    // schedule first events
    schedule_event(head);
}
////////////////////////////////////////////////////////////////
void generate_report()
{
    // output statistics
}
//////////////////////////////////////////////////////////////// //schedules an event in the future
int schedule_event(struct event*)
{
// insert event in the event queue in its order of time
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
    struct event* eve;
    while (!end_condition)
    {
        eve = head;
        _clock = eve->time;
        switch (eve->type)
        {
            case EVENT1:
                process_event1(eve);
                break;
            case EVENT2:
                process_event2(eve);
                break;

                // add more events

            default:
                cerr << "invalid event type";   // error
        }

        head = eve->next;
        delete eve;
        eve = NULL;
    }
    return 0;
}

int process_event1(struct event* eve)
{
    // need to have arrival time
    return 0;   // PLACEHOLDER
}

int process_event2(struct event* eve)
{
    return 0;   // PLACEHOLDER
}
////////////////////////////////////////////////////////////////
int main(int argc, char *argv[] )
{
    cout << "starting";
    if (argc < 1)
    {
        cerr << "[123] [average arrival rate] [average service time] [quantum interval]\n\n"
             << "1 : FCFS\n"
             << "2 : SRTF\n"
             << "3 : RR";
    }
    init();
    run_sim();
    generate_report();
    return 0;
}

