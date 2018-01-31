CC = gcc
CFLAGS=-std=c11 -Wall
LDLIBS=-lm
DEBUG=-g -DDEBUG
PROGRAM=galsim
OBJS=galaxy.o


all: galsim galaxy.o

galaxy.o: galaxy.c
	$(CC) $(CFLAGS) -c galaxy.c

galsim:	$(OBJS) galaxy.c
	$(CC) $(OBJS) $(LDLIBS) -o $(PROGRAM)

debug_galaxy.o: galaxy.c
	$(CC) $(CFLAGS) $(DEBUG) -c galaxy.c

debug_galaxy:	$(OBJS) galaxy.c debug_galaxy.o
	$(CC) $(OBJS) $(LDLIBS) -o $(PROGRAM)
	./$(PROGRAM)

clean:
	$(RM)  *.o $(PROGRAM) 
.PHONY: clean 
