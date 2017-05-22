#include "markedsim.h"

field*** 
create_marked(int x, int y, int floor_count){
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

field*** 
import_marked(char* path, int* x, int* y, int* floor_count){
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

void 
free_marked(field*** marked, int y, int floor_count){
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
	if(&marked[vec.z][vec.y][vec.x] == BLOCKVAL){
		return TRUE;
	}
	return FALSE;
}


void
work_queue(field*** marked, queue_t* queue){
	if(queue_empty(queue)){
		return;
	}else{
		entity* e = queue_dequeue(queue);
		printf("id: %d, type: %d, pos: (%d,%d,%d)\n",
		e->id, e->type, e->position.x, e->position.y, e->position.z);
	}	
}

void
spawn_entity(field*** marked, queue_t* queue, vector3 position, int type, int size){
	if(is_blocked(marked, position)) exit(EXIT_FAILURE);
	int static counter = 0;
	vector3* list = calloc(size, sizeof(vector3));
	//TODO:: gen list (type);
	entity e = {counter, type, position, list};
	queue_enqueue(queue, &e); 
}

void free_entitys(queue_t* queue){
	while(!queue_empty(queue)){
		entity* e = queue_dequeue(queue);
		free(e->list);
	}
}

int
main(int argc, char *argv[]){
	if (argc < 2) exit(EXIT_FAILURE);

	int x, y, floor_count;	
	field*** marked;
	queue_t *queue = queue_new();

	marked = import_marked(argv[1], &x, &y, &floor_count);
	printf("x: %d\n", x);
	printf("y: %d\n", y);
	printf("floor_count: %d\n", floor_count);

	free_marked(marked, y, floor_count);
	free_entitys(queue);
	
	return 0;
}
