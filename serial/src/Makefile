CC = gcc
CFLAGS = -g
BIN=prog

OBJ = markedsim.o

prog: $(OBJ)
	$(CC) $(CLFAGS) -o prog $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(BIN) $(OBJ) 
