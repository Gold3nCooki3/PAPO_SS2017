#ifndef ENTITY_H
#define ENTITY_H

#define TURE 1
#define FALSE 0

#define ETRANCE 7
#define EXIT 7
#define ROLLTREPPE 0 //2

#define CUSTOMER 0
#define EMPLOYEE 5
#define LISTL 2

#include <stdio.h>
#include <stdlib.h>  // rand(), srand()
#include <time.h>    // time()

#include "market.h"
#include "queue.h"
#include "program_test.h"

struct
entity{
	int id;
	int type;
	int listpos;
	vector3 position;
	vector3 list[LISTL];
};
typedef struct entity entity;

void spawn_entity(field*** market, queue_t* queue, vector3 position, int type);
void work_queue(field*** market, queue_t* queue);

//only for testing
int move_entity(field*** market, entity* e);


#endif
