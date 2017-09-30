#ifndef PROGRAM_TEST_H
#define PROGRAM_TEST_H

#include <stdlib.h>
#include <stdio.h>

#include "mpi.h"
#include "queue.h"
#include "marketsim.h"
#include "entity.h"
#include "AStar.h"

#define PRINTTAG 5
#define MASTER 0


struct PrintEntity{
	int pathpos;
	int pathcount;
	int id;
	int type;
	int posx;
	int posy;
	int posz;
	int destx;
	int desty;
	int destz;
};
typedef struct PrintEntity PrintEntity;

/*
 *print the whole queue of entities
 */
void print_queue_parallel(queue_t* const queue, meta *mmi, int chunk_lenth);

#endif
