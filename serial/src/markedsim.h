#ifndef MARKEDSIM_H
#define MARKEDSIM_H
#define TURE 1
#define FALSE 0
#define BLOCKVAL 1
#define LISTL 1

#include <stdlib.h>  // rand(), srand()
#include <time.h>    // time()
#include <stdio.h>
#include "queue.h"

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

struct
entity{
	int id;
	int type;
	vector3 position;
	vector3 list[5];
};
typedef struct entity entity;

field*** import_marked(char* path, int* x, int* y, int* floor_count);
void freemarked(field*** marked, int y, int floor_count);
field*** createmarked(int x, int y, int floor_count);

#endif
