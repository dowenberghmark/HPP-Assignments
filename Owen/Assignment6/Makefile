CC = gcc
EXTRA=
CFLAGS=-std=c11 -Wall -O3 -march=native  $(EXTRA)  
LDLIBS=-L/opt/X11/lib -lX11 -lm $(EXTRA) -flto 
INCLUDES=-I/opt/X11/include
DEBUG=-g -DDEBUG
DEBUGQUAD=-DQUAD -g
PROGRAM=galsim
PROGRAM2=quad
OBJS=galaxy.o graphics.o quad_tree.o

galsim: galaxy.c $(OBJS)
	$(CC) $(OBJS) $(LDLIBS)  -o $(PROGRAM)

galaxy.o: graphics.o galaxy.c galaxy.h quad_tree.c quad_tree.h quad_tree.o
	$(CC) $(CFLAGS) galaxy.h -D _DEFAULT_SOURCE -c galaxy.c



quad_tree.o: quad_tree.c
	$(CC) $(CFLAGS) $(LDLIBS)  quad_tree.h -c quad_tree.c

quad_tree:  quad_tree.c debug_quad_tree.o
	$(CC) quad_tree.o $(LDLIBS) -o $(PROGRAM2)


debug_galaxy.o: galaxy.c galaxy.h
	$(CC) $(CFLAGS) $(DEBUG) galaxy.h -c galaxy.c


debug_galaxy:	$(OBJS) galaxy.c debug_galaxy.o
	$(CC) $(OBJS) $(LDLIBS) -o $(PROGRAM)
	./$(PROGRAM)
debug_quad_tree.o: quad_tree.c
	$(CC) $(CFLAGS) $(DEBUGQUAD) quad_tree.h -c quad_tree.c

debug_quad_tree:  quad_tree.c debug_quad_tree.o
	$(CC) quad_tree.o $(LDLIBS) -o $(PROGRAM2)
	./$(PROGRAM2)


graphics.o: ./graphics/graphics.c ./graphics/graphics.h
	gcc $(CFLAGS) $(INCLUDES) -c ./graphics/graphics.c


clean:
	$(RM)  *.o $(PROGRAM) comp *~ $(PROGRAM2)
.PHONY: clean 
