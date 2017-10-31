
#ifndef ENTITY_H
#define ENTITY_H

#define TRUE 1
#define FALSE 0

#define LISTL 10
#define FILLVAL 10

#define PATHTAG 55
#define ENTITYTAG 66

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>

#include "mpi.h"
#include "market.h"
#include "queue.h"
#include "program_test.h"
#include "AStar.h"

int *readbuf;		//buffer to read a single entity
int items, ecount;	//contains max_items per entity and number of entities in file

typedef enum PATHF_STATUS {ERR = -1, COMPL = -2} PATHF_STATUS;
typedef enum EntityType {CUSTOMER, EMPLOYEE=5} EntityType;
typedef enum EnqueueStatus {ENQUEUE, NEWPATH, DESTROY, UP, DOWN, EDGEL, EDGER} EnS;

/*Entity
 * id 		: count upwards
 * type 	: employee, customer
 * listpos	: position in list,
 * 			  to find out where to go next
 * position : position in the market
 * list		: List of Positions
 * 			  where to get or fill up content
 */
struct
entity{
	EntityType type;
	int id;
	int listpos;
	int amountItems;
	int path_position;
	ASPath path;
	vector3 position;
	vector3 memory_dest;
	vector3* list;
};
typedef struct entity entity;

struct
EssentialEntity{
	EntityType type;
	int id;
	int listpos;
	int amountItems;
	vector3 position;
	vector3 memory_dest;
	vector3 list[LISTL];
};
typedef struct EssentialEntity EssentialEntity;

struct
PathEssentials{
	int id;
	int status;
	vector3 start;
	vector3 dest;
	vector3 final_dest;
};
typedef struct PathEssentials PE;

struct
PathSave{
	int id;
	vector3 dest;
};
typedef struct PathSave PS;

struct
PathArrays{
	PE * local_r;
	PE * local_l;
	PS * known_Path;
	int* knownPathmax;
	int* knownPath_count;
	int rightcount;
	int leftcount;
	int core_c_r;
	int core_c_l;
	int new_c_r;
	int new_c_l;
	int not_completed;
};
typedef struct PathArrays PathArrays;

static vector3 start_vec;

MPI_File output;

/*Spawn an entity and enqueue it
 * @param queue 	: queue of all entities
 * @param position 	: position where the entity is spawned
 * @param e		: entitiy to spawn
 * @param type		: type of the entity
 */
void spawn_entity(meta* const mmi, queue_t* const entity_queue, queue_t* const empty_shelfs, entity* e, EntityType type);
void spawn_entity_random(meta* const mmi, queue_t* const entity_queue, queue_t* const empty_shelfs, EntityType type);

/*Move every entity in the queue, dequeue if entity reached final destination
 * @param market 	: fields where entities move within
 * @param queue 	: queue of all entities
 *
 */
void work_queue(meta * const mmi, queue_t* const entity_queue, queue_t* const empty_shelfs, queue_t* const pathf_queue, PS* known_Path, int* knownPathmax, int* knownPath_count);

/*
 * Generates an employee entity to refill an empty shelf
 * @param id            : id of the employee
 * @param empty_shelf   : shelf to refill
 */
entity* make_worker(int id, vector3* empty_shelf);

/* Prepares the parallel reading in a file for the entry-commgroup
 *  initially reads the number of entities and the number of max_items per entity
 * @param fh            : file to read entities from
 + @return		: number of entities to spawn
 */
int readInit(MPI_File *fh);

/* Reads a single entity
 * @param fh            : file to read
 * @param entid         : id of entity to read
 */
void readEntity(meta* const mmi, MPI_File *fh, int entid, queue_t* const pathf_queue);

void pathWriterInit(MPI_File outputfh);

void writePath(ASPath path, entity* e);

#endif
