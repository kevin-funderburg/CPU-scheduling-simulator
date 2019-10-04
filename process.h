//
// Created by Kevin Funderburg on 2019-10-04.
//

#ifndef PROJECT1_PROCESS_H
#define PROJECT1_PROCESS_H
#include <iostream>

enum State {READY = 1, RUNNING = 2, TERMINATED = 3};

class Process {

public:
    Process();
    Process(int, float, State, float, float, float);
    float getRemainingTime();
    void output();

    int pid;
    float arrivalTime;
    State state;
    float burst;
    float remainingTime;
    float departureTime;
};

Process::Process()
        : pid(0), arrivalTime(0.0), state(READY), burst(0.0), remainingTime(0.0), departureTime(0.0)
{

}

Process::Process(int id, float a_time, State s, float b, float r_time, float d_time)
        : pid(id), arrivalTime(a_time), state(s), burst(b), remainingTime(r_time), departureTime(d_time)
{

}

float Process::getRemainingTime()
{
    return remainingTime;
}

void Process::output()
{
    std::cout << "id: " << pid
         << "\narrival: " << arrivalTime
         << "\nstate: " << state
         << "\nburst: " << burst
         << "\nremaining: " << remainingTime
         << "\ndeparture: " << departureTime << std::endl;
}

#endif //PROJECT1_PROCESS_H
