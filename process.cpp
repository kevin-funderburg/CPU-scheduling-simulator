//
// Created by Kevin Funderburg on 2019-10-04.
//

#include "process.h"

Process::Process(int id, float a_time, State s, float b, float r_time, float d_time)
        : pid(id), arrivalTime(a_time), state(s), burst(b), remainingTime(r_time), departureTime(d_time)
{

}

Process::getRemainingTime()
{
    return remainingTime;
}

Process::output()
{
    cout << "id: " << pid
         << "\narrival: " << arrivalTime
         << "\nstate: " << state
         << "\nburst: " << burst
         << "\nremaining: " << remainingTime
         << "\ndeparture: " << departureTime << endl;
}