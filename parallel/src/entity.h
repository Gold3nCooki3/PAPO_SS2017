
#ifndef ENTITY_H
#define ENTITY_H

#define TRUE 1
#define FALSE 0

#define LISTL 10
#define FILLVAL 10

#define PATHTAG 555

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


typedef enum EntityType {CUSTOMER, EMPLOYEE=5} EntityType;


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
PathEssentials{
	int id;
	int status;
	vector3 start;
	vector3 dest;
};
typedef struct PathEssentials PE;

static vector3 start_vec;

/*Spawn an entity and enqueue it
 * @param queue 	: queue of all entities
 * @param position 	: position where the entity is spawned
 * @param type		: type of the entity
 */
void spawn_entity(meta* const mmi, queue_t* const entity_queue, queue_t* const empty_shelfs, EntityType type);

/*Move every entity in the queue, dequeue if entity reached final destination
 * @param market 	: fields where entities move within
 * @param queue 	: queue of all entities
 *
 */
void work_queue(meta * const mmi, queue_t* const entity_queue, queue_t* const empty_shelfs, queue_t* const pathf_queue);

#endif
