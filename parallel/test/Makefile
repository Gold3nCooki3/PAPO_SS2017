CC = mpicc
CFLAGS = -std=c99 -g -Wall -O0
BIN=prog.exe
OBJ = main.o

all: pre-build prog

pre-build:
	@echo PRE

prog: $(OBJ)
	$(CC) $(CLFAGS) -lm -o prog $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(BIN) $(OBJ) 
