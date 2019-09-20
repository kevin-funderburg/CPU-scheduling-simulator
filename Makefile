sim: sim.o
	g++ sim.o -o sim
sim.o: sim.cpp
     g++ -Wall -ansi -pedantic -std=c++11 -c sim.cpp
go:
	./sim 1 5 0.06 0.01
clean:
	@rm -rf sim.o
cleanall:
	@rm -rf sim.o sim