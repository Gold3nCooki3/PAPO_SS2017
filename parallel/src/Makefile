CC = mpicc
CFLAGS = -std=c99 -g -Wall
BIN=prog.exe
OBJ = marketsim.o market.o entity.o queue.o  program_test.o AStar.o

all: pre-build prog

pre-build:
	@echo PRE

prog: $(OBJ)
	$(CC) $(CLFAGS) -lm -o prog $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(BIN) $(OBJ) 
