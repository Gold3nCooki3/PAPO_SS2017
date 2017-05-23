CC = gcc
CFLAGS = -g
BIN=prog

OBJ = markedsim.o queue.o program_test.o

prog: $(OBJ)
	$(CC) $(CLFAGS) -o prog $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(BIN) $(OBJ) 
