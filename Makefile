CC=gcc
CFLAGS=-I.

simmake: sim.cpp
     $(CC) -o sim hellomake.o hellofunc.o