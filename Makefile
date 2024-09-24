CC = gcc
OPT = -O3
#OPT = -g
WARN = -Wall
CFLAGS = $(OPT) $(WARN) $(INC) $(LIB)

# List all your .c files here (source files, excluding header files)
SIM_SRC = sim.c

# List corresponding compiled object files here (.o files)
SIM_OBJ = sim.o
 
#################################

# default rule

all: clean sim
	@echo "my work is done here..."


# rule for making sim

sim: $(SIM_OBJ)
	$(CC) -o sim $(CFLAGS) $(SIM_OBJ) src/Cache.c  src/Queue.c src/CacheOperation.c -lm
	@echo "-----------DONE WITH sim-----------"


# generic rule for converting any .c file to any .o file
 
.c.o:
	$(CC) $(CFLAGS) -c $*.c


# type "make clean" to remove all .o files plus the sim binary

clean:
	rm -f *.o sim


# type "make clobber" to remove all .o files (leaves sim binary)

clobber:
	rm -f *.o


