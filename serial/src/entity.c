#include "entity.h"

/*Clamps a value to -1, 0, 1
 * @param d 	: input value
 * @return 		: -1, 0, 1
 */
int clamp(int d) {
  if(d >= 1) return 1;
  if(d <= -1) return -1;
  return 0;
}

/*Moves an entity towards their destination,
 * to pick up or fill up content from a frame
 * @param market	: market where entity is moving within
 * @param e			: entity
 * @return FALSE if person has reached their final destination
 */
int move_entity(field*** const market, entity* const e){
	int pos = e->listpos;
	vector3 actual_dest = {e->list[pos].x , e->list[pos].y, e->list[pos].z};
	/*get field right, left, bottom or top of the destination,
	 * this is the field the entity has to go */
	if(in_matrix(market, e->list[pos]) ->type == ESCALATOR){
	}else{
		actual_dest.x++;
		if(is_blocked(market, actual_dest)){
				actual_dest.x -= 2;
			if (is_blocked(market, actual_dest)){
				actual_dest.x++;
				actual_dest.y++;
				if (is_blocked(market, actual_dest)){
					actual_dest.y -= 2;
				}else{
					printf("Not valid!\n");
					exit(EXIT_FAILURE);
				}
			}
		}
	}

	//move if entity is on the same floor as the destination
	if(actual_dest.z == e->position.z){
		int move_in_x = clamp(actual_dest.x - e->position.x);
		int move_in_y = clamp(actual_dest.y - e->position.y);
		vector3 vec_x = {e->position.x  + move_in_x, e->position.y, e->position.z};
		vector3 vec_y = {e->position.x, e->position.y + move_in_y, e->position.z};
		if(move_in_x != 0 && !is_blocked(market, vec_x)){
			e->position = vec_x;
		}else if(move_in_y != 0 && !is_blocked(market, vec_y)){
			e->position  = vec_y;
		}else{
			printf("No Movement");
			exit(EXIT_FAILURE);
		}
	}else if(in_matrix(market, e->position)->type == ESCALATOR){
		//entity is standing on an escalator
		if(actual_dest.z > e->position.z){
			e->position.z++;
		}else{
			e->position.z--;
		}

	}else{
		printf("Not on an escalator field nor moving\n");
		exit(EXIT_FAILURE);
	}

	//Check if entity reached destination
	if(vec_equal(&actual_dest,&e->position)){
		field* f = in_matrix(market, actual_dest);
		switch (e->type){
			case CUSTOMER:
				if(f->type != ESCALATOR){
					if(f->amount  > 0) {
						f->amount -= 1;
					}else{
						printf("Didn't get content\n");
					}
				}
				break;
			case EMPLOYEE:
				if(f->type != ESCALATOR)	f->amount += FILLVAL; break;
			default: printf("Nether customer nor employee\n"); break;
		}
		if(f->type == EXIT || pos >= LISTL-1) return FALSE;
		e->listpos++;
	}
	return TRUE;
}

/*Move every entity in the queue, dequeue if entity reached final destination
 * @param market 	: fields where entities move within
 * @param queue 	: queue of all entities
 */
void work_queue(field*** const market, queue_t* queue){
	if(queue_empty(queue)){
		printf("empty!");
		return;
	}else{
		entity* first = queue_dequeue(queue);
		entity* e = first;
		do{
			if(!first)first = e;
			int not_finished = move_entity(market, e);
			if(not_finished){
				queue_enqueue(queue, e);
			}else {
				if(e == first) first = NULL;
				if(queue_empty(queue)) break;
			}
			e = queue_dequeue(queue);
		}while(first != e);
		if(first == e) queue_enqueue(queue, e); // otherwise one element gets lost
	}
}

/*Spawn an entity and enqueue it
 * @param queue 	: queue of all entities
 * @param position 	: position where the entity is spawned
 * @param type		: type of the entity
 */
void spawn_entity(field*** const market, queue_t* queue, vector3 position, int type){
	static int counter = 0;
	//TODO:: gen list (type);
	int amountItems = rand()%18+2;		//2-20 Items on the shoppinglist
	//vector3 list[amountItems];
	vector3* list= generate_shoppinglist(market, 1, 10, 11, 50);
	entity* e = malloc(sizeof(*e));
		e->id = counter;
		e->type = type;
		e->listpos = 0;
		e->position= position;
		e->list = list;
		/*	//TODO:: get rid of this
			vector3 v = {rand()%10, rand()%11, position.z};
			vector3 r = {rand()%10, rand()%11, position.z};
			vector3 v2 = {rand()%10, rand()%11,position.z+1};
			vector3 v3 = {rand()%10, rand()%11,position.z-1};
		e->list[0] = v;
		e->list[1] = (rand()%2 && v3.z < 50) ? v3: v2;*/
	queue_enqueue(queue, e);
	counter++;
}

/**
 * Generates a random shopping list for each customer-entity
 */
vector3* generate_shoppinglist(field*** const market, int amountItems, int rows, int columns, int stories) {
	vector3* list = malloc(sizeof(vector3) * amountItems);
	vector3 v;			//Single item
	for(int i = 0; i < amountItems; i++) {
		printf("%d", i);
		//random field in the mall
		v.x = rand()%rows;
		v.y = rand()%columns;
		v.z = rand()%stories;

		if(in_matrix(market, v) ->type == SHELF) {			//IF is SHELF	 -> add field
			list[i] = v;
		} else {											//generate new Field
			i--;
		}
	}
	return list;
}

/*entity* create_entity(int id, EntityType type, vector3 position, int listsize, vector3* list) {
	entity e;
	e->id = id;
	e->type = type;
	e->listpos = 0;
	e->position = position;
	e->list = list;
	return *e;
}*/
