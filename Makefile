OBJS = comm.o sim.o test.o
CC = gcc
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)
EXEC = csv
all: $(OBJS)
			$(CC) $(LFLAGS) $(OBJS) -o $(EXEC)
		
comm.o : common.h comm.c
			$(CC) $(CFLAGS) comm.c

sim.o : common.h sim.c
	    $(CC) $(CFLAGS) sim.c

test.o : common.h test.c
	    $(CC) $(CFLAGS) test.c
clean:
	rm -f $(OBJS)
	rm -f $(EXEC)
run:
	./$(EXEC)
                 
