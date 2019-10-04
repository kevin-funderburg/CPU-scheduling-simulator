# Discrete Time Simulator

Authors: Kevin Funderburg, Rob Murray

<details>
  <summary><strong>Table of Contents</strong> (click to expand)</summary>
<!-- TOC depthFrom:2 -->

- [About](#about)
- [Licensing & thanks](#licensing--thanks)
- [Changelog](#changelog)

<!-- /TOC -->
</details>

## About
In this project, we are going to build a discrete-time event simulator for a number of CPU scheduling algorithms on a single CPU system. The goal of this project is to compare and assess the impact of different scheduling algorithms on different performance metrics, and across multiple workloads.

## Usage
Make commands
```shell
make
make clean
make cleanall
```

Command line structure:

```shell
sim [123] [average arrival rate] [average service time] [quantum interval]

1 : First-Come First-Served (_FCFS)
2 : Shortest Remaining Time First (_SRTF)
3 : Round Robin, with different quantum values (_RR) 
```





## Licensing & Thanks

This project is released under the [MIT License][mit].

## Changelog

- v1.0.0

[mit]: https://github.com/kevin-funderburg/discrete-time-event-simulator/blob/master/LICENSE.txt
