#include "entity.h"

int clamp(int d) {
  if(d >= 1) return 1;
  if(d <= -1) return -1;
  return 0;
}

int vec_equal(vector3 const * vec1, vector3 const* vec2){
	return (vec1->x == vec2->x) && (vec1->y == vec2->y) && (vec1->z == vec2->z);
}

int move_entity(field*** market, entity* e){
	int pos = e->listpos;
	vector3 rel_pos = {e->list[pos].x , e->list[pos].y, e->list[pos].z};
	//get field right left bot or top of the destination, this is the field we want to go
	if(in_matrix(market, e->list[pos]) ->type == ROLLTREPPE){
	}else{
		rel_pos.x++;
		if(is_blocked(market, rel_pos)){
				rel_pos.x -= 2;
			if (is_blocked(market, rel_pos)){
				rel_pos.x++;
				rel_pos.y++;
				if (is_blocked(market, rel_pos)){
					rel_pos.y -= 2;
				}else{
					printf("Not valid!\n");
					exit(EXIT_FAILURE);
				}
			}
		}
	}

	//move
	if(rel_pos.z == e->position.z){
		int move_in_x = clamp(rel_pos.x - e->position.x);
		int move_in_y = clamp(rel_pos.y - e->position.y);
		printf("id: %d, x: %d y: %d <=", e->id,move_in_x ,move_in_y);
		printf(" (%2d, %2d) -> (%2d, %2d)  : (%2d, %2d) \n",
					e->position.x, e->position.y, rel_pos.x, rel_pos.y,rel_pos.x - e->position.x , rel_pos.x - e->position.y);
		vector3 vec_x = {e->position.x  + move_in_x, e->position.y, e->position.z};
		vector3 vec_y = {e->position.x, e->position.y + move_in_y, e->position.z};
		if(move_in_x != 0 && !is_blocked(market, vec_x)){
			e->position = vec_x;
		}else if(move_in_y != 0 && !is_blocked(market, vec_y)){
			e->position  = vec_y;
		}else{
			printf("Something went wrong!\n");
			exit(EXIT_FAILURE);
		}
		//printf("i got to (%2d, %2d)\n", e->position.x, e->position.y);
	}else if(in_matrix(market, e->position)->type == ROLLTREPPE){
		printf("i am going up!\n");
		if(rel_pos.z > e->position.z){
			e->position.z++;
		}else{
			e->position.z--;
		}

	}else{
		printf("Somehow i got here !?\n");
		exit(EXIT_FAILURE);
	}

	//end
	if(vec_equal(&rel_pos,&e->position)){
		switch (e->type){
			case CUSTOMER: in_matrix(market, rel_pos)->amount -= 1; break;
			case EMPLOYEE: in_matrix(market, rel_pos)->amount += 10; break;
			default: printf("NO not right\n"); break;
		}
		if(in_matrix(market, e->position)->type == EXIT) return 0;
		if(pos == LISTL-1){
			printf("%d No Exit\n", e->id);
			return 0;}
		e->listpos++;
	}
	//printf("a small step for me .. \n");
	return 1;
}

void work_queue(field*** market, queue_t* queue){
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
		if(first == e) queue_enqueue(queue, e);
	}
}

void spawn_entity(field*** market, queue_t* queue, vector3 position, int type){
	if(is_blocked(market, position)) exit(EXIT_FAILURE);
	static int counter = 0;
	//TODO:: gen list (type);
	entity* e = malloc(sizeof(*e));
		e->id = counter;
		e->type = type;
		e->listpos = 0;
		e->position= position;
			//TODO:: get rid of this
			vector3 v = {rand()%10, rand()%11, position.z};
			vector3 r = {rand()%10, rand()%11, position.z};
			vector3 v2 = {rand()%10, rand()%11,position.z+1};
			vector3 v3 = {rand()%10, rand()%11,position.z-1};
		e->list[0] = v;
		e->list[1] = (rand()%2 && v3.z < 50) ? v3: v2;
	queue_enqueue(queue, e);
	counter++;
}
