#ifndef ENTITY_H
#define ENTITY_H

#define TRUE 1
#define FALSE 0

//TODO: make enum for field type
#define ETRANCE 7
#define EXIT 7
#define ESCALATOR 0 //2

//TODO: make enum for entity type
#define CUSTOMER 0
#define EMPLOYEE 5

#define LISTL 2
#define FILLVAL 10

#include <stdio.h>
#include <stdlib.h>

#include "market.h"
#include "queue.h"
#include "program_test.h"

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
	int id;
	int type;
	int listpos;
	vector3 position;
	vector3 list[LISTL];
};
typedef struct entity entity;

/*Spawn an entity and enqueue it
 * @param queue 	: queue of all entities
 * @param position 	: position where the entity is spawned
 * @param type		: type of the entity
 */
void spawn_entity(field*** const market, queue_t* const queue, vector3 position, int type);

/*Move every entity in the queue, dequeue if entity reached final destination
 * @param market 	: fields where entities move within
 * @param queue 	: queue of all entities
 */
void work_queue(field*** const market, queue_t* const queue);

/*Moves an entity towards their destination,
 * to pick up or fill up content from a frame
 * ONLY FOR TESTING
 * @param market	: market where entity is moving within
 * @param e			: entity
 * @return FALSE if person has reached their final destination
 */
int move_entity(field*** const market, entity* const e);


#endif
