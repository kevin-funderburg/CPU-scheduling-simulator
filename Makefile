sim: sim.o
	g++ -std=c++11 sim.o -o sim
sim.o: sim.cpp
	g++ -Wall -ansi -pedantic -std=c++11 -c sim.cpp
clean:
	@rm -rf sim.o
cleanall:
	@rm -rf sim.o sim
FCFS:
	./sim 1 5 0.06 0.01
SRTF:
	./sim 2 5 0.06 0.01
RR1:
	./sim 3 10 0.06 0.01
RR2:
	./sim 3 10 0.06 0.2