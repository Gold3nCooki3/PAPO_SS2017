#include "markedsim.h"

field*** create_marked(int x, int y, int floor_count){
	field *** marked = malloc(floor_count * sizeof(field**)); 
	for(int i = 0; i < floor_count; i++){
		field** floor = malloc(y * sizeof(field*));
		marked[i] = floor;
		for(int j = 0; j < y; j++){
			floor[j] = malloc(x * sizeof(field));
		}
	}
	return marked;
}

field*** import_marked(char* path, int* x, int* y, int* floor_count){
	FILE *file = fopen(path, "r");
	if (file == NULL) exit(EXIT_FAILURE);

	fscanf(file, "%d,%d,%d\n", x, y, floor_count);
	field*** marked = create_marked(*x, *y, *floor_count);
	for(int a = 0; a < (*floor_count); a++){
		for(int b = 0; b < (*y); b++){
			for(int c = 0; c < (*x); c++){
				fscanf(file, "%d,%d,%d\n",
					&marked[a][b][c].type,
					&marked[a][b][c].content,
					&marked[a][b][c].amount);
			}
		}
	}
	fclose(file);
return marked;
}

void free_marked(field*** marked, int y, int floor_count){
	for(int o = 0; o < floor_count; o++ ){
		for(int i = 0; i < y; i++){
			free(marked[o][i]);
		}
		free(marked[o]);
	}
	free(marked);
}

field* in_matrix(field*** marked, vector3 vec){
	return &marked[vec.z][vec.y][vec.x];
}

int is_blocked(field*** marked, vector3 vec){
	if(marked[vec.z][vec.y][vec.x].type == BLOCKVAL){
		return TRUE;
	}
	return FALSE;
}

int clamp(int d) {
  if(d >= 1) return 1;
  if(d <= -1) return 1;
  return 0;
}

int vec_equal(vector3 const * vec1, vector3 const* vec2){
	return (vec1->x == vec2->x) && (vec1->y == vec2->y) && (vec1->z == vec2->z);
}

int move_entity(field*** marked, entity* e){
	int pos = e->listpos;
	vector3 rel_pos = {e->list[pos].x  + 1, e->list[pos].y, e->list[pos].z};
	//get field right left bot or top of the destination, this is the field we want to go
	if(is_blocked(marked, rel_pos)){
		rel_pos.x -= 2;
		if (is_blocked(marked, rel_pos)){
			rel_pos.x++;
			rel_pos.y++;
			}if (is_blocked(marked, rel_pos)){
				rel_pos.y -= 2;
			}else{
				printf("Not valid!\n");
				exit(EXIT_FAILURE);
			}
	}

//	printf("i want to go from (%2d, %2d) to (%2d, %2d)  to get (%2d, %2d) \n",
	//		e->position.x, e->position.y, rel_pos.x, rel_pos.y, e->list[pos].x, e->list[pos].y);
	//move
	if(e->list[pos].z == e->position.z){
		int move_in_x = clamp(rel_pos.x - e->position.x);
		int move_in_y = clamp(rel_pos.y - e->position.y);
		printf("id: %d, x: %d y: %d \n", e->id,move_in_x ,move_in_y);

		vector3 vec_x = {e->position.x  + move_in_x, e->position.y, e->position.z};
		vector3 vec_y = {e->position.x, e->position.y + move_in_y, e->position.z};
		if(move_in_x != 0 && !is_blocked(marked, vec_x)){
			e->position = vec_x;
		}else if(move_in_y != 0 && !is_blocked(marked, vec_y)){
			e->position  = vec_y;
		}else{
			printf("Something went wrong!\n");
			exit(EXIT_FAILURE);
		}
		//printf("i got to (%2d, %2d)\n", e->position.x, e->position.y);
	}else if(in_matrix(marked, e->position)->type == ROLLTREPPE){
		printf("i am going up!\n");
		e->position.z++;
	}else{
		printf("Somehow i got here !?\n");
		//Weg zur Rolltreppe ?
		;
	}

	//end
	if(pos == LISTL-1 && vec_equal(&e->list[pos],&e->position)) return 0;
	if(vec_equal(&rel_pos,&e->position)){
		printf("i got it yes");
		switch (e->type){
			case CUSTOMER: in_matrix(marked, rel_pos)->amount -= 1; break;
			case EMPLOYEE: in_matrix(marked, rel_pos)->amount += 10;
			printf("new amount is now %d", in_matrix(marked, rel_pos)->amount); break;
			default: printf("NO not right\n"); break;
		}
		e->listpos++;
	}
	//printf("a small step for me .. \n");
	return 1;
}

void work_queue(field*** marked, queue_t* queue){
	if(queue_empty(queue)){
		printf("empty!");
		return;
	}else{
		entity* first = queue_dequeue(queue);
		entity* e = first;
		do{
			int not_finished = move_entity(marked, e);
			if(not_finished){
				queue_enqueue(queue, e);
			}else {
				if(queue_empty(queue)) break;
			}
			e = queue_dequeue(queue);
		}while(first != e);
		if(first == e) queue_enqueue(queue, e);
	}
}

void spawn_entity(field*** marked, queue_t* queue, vector3 position, int type){
	if(is_blocked(marked, position)) exit(EXIT_FAILURE);
	static int counter = 0;
	//TODO:: gen list (type);
	entity* e = malloc(sizeof(*e));
		e->id = counter;
		e->type = type;
		e->listpos = 0;
		e->position= position;
			vector3 v = {rand()%10, rand()%11, position.z};
		e->list[0] = v;
	queue_enqueue(queue, e);
	counter++;
}

int main(int argc, char *argv[]){
	if (argc < 2) exit(EXIT_FAILURE);
	srand(time(NULL));

	int x, y, floor_count;
	field*** marked;
	queue_t *queue = queue_new();

	marked = import_marked(argv[1], &x, &y, &floor_count);
	printf("x: %d\n", x);
	printf("y: %d\n", y);
	printf("floor_count: %d\n", floor_count);
	
	test_spawn(marked, queue, x, y, floor_count);
	while(!queue_empty(queue)){
		work_queue(marked, queue);
		print_queue(queue);
	}

	free_marked(marked, y, floor_count);
	queue_destroy(queue);
	return 0;
}
