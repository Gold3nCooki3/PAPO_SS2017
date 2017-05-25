#ifndef MARKET_H
#define MARKET_H

#define TRUE 1
#define FALSE 0

#define BLOCKVAL 1

#include <stdlib.h>
#include <stdio.h>

struct
field{
	int type;
	int content;
	int amount;
};
typedef struct field field;

struct
vector3{
	int x;
	int y;
	int z;
};
typedef struct vector3 vector3;

//for lator generator function
//field*** create_market(int x, int y, int floor_count);

field*** import_market(char* path, int* x, int* y, int* floor_count);

field* in_matrix(field*** market, vector3 vec);

void free_market(field*** market, int y, int floor_count);

int is_blocked(field*** market, vector3 vec);

#endif
