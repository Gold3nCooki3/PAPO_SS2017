#ifndef MARKEDSIM_H
#define MARKEDSIM_H
#define TURE 1
#define FALSE 0

#define ETRANCE 7
#define EXIT 7
#define ROLLTREPPE 3
#define BLOCKVAL 1

#define CUSTOMER 0
#define EMPLOYEE 5

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
	int listpos;
	vector3 position;
	vector3 list[LISTL];
};
typedef struct entity entity;

void spawn_entity(field*** marked, queue_t* queue, vector3 position, int type);

#include "program_test.h"

#endif
