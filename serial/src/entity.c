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

vector3 get_close_vector3(vector3* const list, int listlength, vector3 start){
	vector3 dest;
	int mindistance = -1;
	int distance;
	for(int i = 0; i < listlength; i++){
		/*TODO: 10 * delta(z) nicht das beste*/
		distance = abs(list[i].x - start.x)  + abs(list[i].y - start.y) + 10*abs(list[i].z - start.z);
		if(distance > mindistance || mindistance == -1) {
			mindistance = distance;
			dest = list[i];
		}
	}
	if(mindistance == -1){printf("error"); exit(EXIT_FAILURE);}
	return dest;
}

/*Moves an entity towards their destination,
 * to pick up or fill up content from a frame
 * @param market	: market where entity is moving within
 * @param e			: entity
 * @return FALSE if person has reached their final destination
 */
int move_entity(field*** const market, meta* const mmi,queue_t* const empty_shelfs, entity* const e){
	int pos = e->listpos;
	vector3 actual_dest = {e->list[pos].x , e->list[pos].y, e->list[pos].z};

	/*search for lift or escalator if not on one of these already and the destination is above
	 * memorize destination of the next lift or escalator*/
	if(e->position.z != actual_dest.z  && in_matrix(market, e->position)->type != ESCALATOR && in_matrix(market, e->position)->type != LIFT){
		if(e->memory_lift.x == -1){
			e->memory_lift = get_close_vector3(mmi->lift_fields, mmi->lift_count, e->position);
		}
		actual_dest = e->memory_lift;
	}else{
	/*get field right, left, bottom or top of the destination,
	 *this is the field the entity has to go */
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
	}else if(in_matrix(market, e->position)->type == ESCALATOR || in_matrix(market, e->position)->type == LIFT){
		//entity is standing on an escalator
		if(actual_dest.z > e->position.z){
			e->position.z++;
		}else{
			e->position.z--;
		}
		e->memory_lift.x = -1; //reset memory
	}else{
		printf("Not on an escalator field nor moving\n");
		exit(EXIT_FAILURE);
	}

	//Check if entity reached destination
	if(vec_equal(&actual_dest,&e->position)){
		field* f = in_matrix(market, actual_dest);
		field* shelf = in_matrix(market, e->list[pos]);
		switch (e->type){
			case CUSTOMER:
				if(f->type != ESCALATOR && f->type != LIFT){
					if(shelf->amount  > 0) {
						shelf->amount -= 1;
					}else{
						vector3 empty_shelf = e->list[pos];
						queue_enqueue(empty_shelfs, &empty_shelf);
						mmi->emtpy_count++;
						printf("Didn't get content\n");
					}
				}
				break;
			case EMPLOYEE:
				if(f->type != ESCALATOR || f->type != LIFT){
					shelf->amount += FILLVAL;
				}
				break;
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
void work_queue(field*** const market, meta * const mmi, queue_t* const queue, queue_t* const empty_shelfs){
	if(queue_empty(queue)){
		printf("empty!");
		return;
	}else{
		entity* first = queue_dequeue(queue);
		entity* e = first;
		do{
			if(!first)first = e;
			int not_finished = move_entity(market, mmi, empty_shelfs, e);
			if(not_finished){
				queue_enqueue(queue, e);
			}else {
				free(e->list);
				if(e == first) first = NULL;
				if(queue_empty(queue)) break;
			}
			e = queue_dequeue(queue);
		}while(first != e);
		if(first == e) queue_enqueue(queue, e); // otherwise one element gets lost
	}
}

/**
 * Generates a random shopping list for each customer-entity
 */
vector3* generate_list(meta* const mmi, queue_t* empty_shelfs, int* items, EntityType Type) {
	*items = (Type == CUSTOMER) ? rand()%LISTL+2 : LISTL+1;
	vector3* list = malloc(sizeof(vector3) * (*items));
	int shelf_count = mmi->shelf_count;
	for(int i = 0; i < *items; i++){
		if(Type == CUSTOMER){
			vector3 v;
			if(i == *items-2){
				//Kasse
				v = get_close_vector3(mmi->register_fields, mmi->register_count, list[i-1]);
			}else if(i == *items-1){
				//Exit
				v = get_close_vector3(mmi->exit_fields, mmi->exit_count, list[i-1]);
			}else{
				int r = abs((i*rand())%shelf_count);
				v = mmi->shelf_fields[r];
			}
			list[i] = v;
		}else{
			vector3* v;
			if(i == *items-1){
				//Exit
				vector3 h = get_close_vector3(mmi->stock_fields, mmi->stock_count, list[i-1]);
				v = &h;
			}else{
				v = queue_dequeue(empty_shelfs);
			}
			mmi->emtpy_count--;
			list[i] = *v;
		}
	}
printf("made list\n");
	return list;
}

/*Spawn an entity and enqueue it
 * @param queue 	: queue of all entities
 * @param position 	: position where the entity is spawned
 * @param type		: type of the entity
 */
void spawn_entity(meta* const mmi, queue_t* const queue, queue_t* const empty_shelfs, EntityType type){
	static int counter = 0;
	int items = 0;
	vector3* list;
	list= generate_list(mmi, empty_shelfs, &items, type);
	vector3 position = (type == CUSTOMER) ? mmi->exit_fields[abs(rand()%mmi->exit_count)] : mmi->stock_fields[abs(rand()%mmi->stock_count)];
	entity* e = malloc(sizeof(*e));
		e->id = counter;
		e->type = type;
		e->listpos = 0;
		e->amountItems = items;
		e->position= position;
		e->memory_lift.x = -1;
		e->list = list;
	queue_enqueue(queue, e);
	counter++;
}
