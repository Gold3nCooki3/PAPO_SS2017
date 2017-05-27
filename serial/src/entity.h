#ifndef ENTITY_H
#define ENTITY_H

#define TRUE 1
#define FALSE 0

#define LISTL 10
#define FILLVAL 10

#include <stdio.h>
#include <stdlib.h>

#include "market.h"
#include "queue.h"
#include "program_test.h"

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
	vector3 position;
	vector3 memory_lift;
	vector3* list;
};
typedef struct entity entity;

/*Spawn an entity and enqueue it
 * @param queue 	: queue of all entities
 * @param position 	: position where the entity is spawned
 * @param type		: type of the entity
 */
void spawn_entity(meta* const mmi, queue_t* const queue, queue_t* const empty_shelfs, EntityType type);

/*Move every entity in the queue, dequeue if entity reached final destination
 * @param market 	: fields where entities move within
 * @param queue 	: queue of all entities
 */
void work_queue(field*** const market, meta * const mmi, queue_t* const queue, queue_t* const empty_shelfs);

/*Moves an entity towards their destination,
 * to pick up or fill up content from a frame
 * ONLY FOR TESTING
 * @param market	: market where entity is moving within
 * @param e			: entity
 * @return FALSE if person has reached their final destination
 */
int move_entity(field*** const market, queue_t* empty_shelfs,meta* const mmi, entity* const e);
#endif
