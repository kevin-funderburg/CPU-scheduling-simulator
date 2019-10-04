//
// Created by Kevin Funderburg on 2019-10-04.
//

#define PROCESS_H
#ifndef PROCESS_H

#include <iostream>

enum State {READY = 1, RUNNING = 2, TERMINATED = 3};

class Process {
public:
    Process(int, float, state, float, float, float);
    float getRemainingTime();
    void output();

    int pid;
    float arrivalTime;
    State state;
    float burst;
    float remainingTime;
    float departureTime;
};


#endif //PROCESS_H
