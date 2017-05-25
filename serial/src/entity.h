#ifndef ENTITY_H
#define ENTITY_H

#define TRUE 1
#define FALSE 0

#define LISTL 2
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
	int id;
	EntityType type;
	int listpos;
	vector3 position;
	//vector3 list[LISTL];
	vector3* list;
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

/*Generates a random shopping list by checking a random field if it is a SHELF.
 * If not the next field is tested.
 * @param market		: market in which the shoppinglist is generated
 * @param list			: returner für the generated list
 * @param amountItems	: amount of Items on the shopping list
 * @param marketinfo	: vector3 with information for rows, columns and stories
 */
vector3* generate_shoppinglist(field*** const market, int amountItems, int rows, int columns, int stories);

/**
 * Constructor for an entity
 * @param id
 * @param type
 * @param position
 * @param listsize		: size of the shopping list
 * @param list
 */
//void create_entity(entity e, int id, EntityType type, vector3 position, int listsize, vector3* list);

#endif
