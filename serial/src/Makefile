CC = gcc
CFLAGS = -g -Wall
BIN=prog

OBJ = marketsim.o market.o entity.o queue.o AStar.o program_test.o

prog: $(OBJ)
	$(CC) $(CLFAGS) -o prog $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(BIN) $(OBJ) 
