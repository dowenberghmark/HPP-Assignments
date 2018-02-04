CC = gcc
CFLAGS=-std=c11 -Wall -g
LDLIBS=-L/opt/X11/lib -lX11 -lm
INCLUDES=-I/opt/X11/include
DEBUG=-g -DDEBUG
PROGRAM=galsim
OBJS=galaxy.o graphics.o


all: galsim galaxy.o

galaxy.o: graphics.o galaxy.c
	$(CC) $(CFLAGS) -D _DEFAULT_SOURCE -c galaxy.c

galsim: galaxy.c $(OBJS)
	$(CC) $(OBJS) $(LDLIBS)  -o $(PROGRAM)

debug_galaxy.o: galaxy.c
	$(CC) $(CFLAGS) $(DEBUG) -c galaxy.c

debug_galaxy:	$(OBJS) galaxy.c debug_galaxy.o
	$(CC) $(OBJS) $(LDLIBS) -o $(PROGRAM)
	./$(PROGRAM)
graphics.o: ./graphics/graphics.c ./graphics/graphics.h
	gcc $(CFLAGS) $(INCLUDES) -c ./graphics/graphics.c


clean:
	$(RM)  *.o $(PROGRAM) comp *~
.PHONY: clean 
