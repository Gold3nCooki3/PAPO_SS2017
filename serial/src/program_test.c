#include "program_test.h"

vector3
rand_vector3(int x, int y, int z){
	vector3 v = {rand()%x, rand()%y, rand()%z};
	return v;
}

void 
test_spawn(field*** marked, queue_t* queue, int x, int y, int floor_count){
	for(int i= 0; i < 4; i++){
		printf("p: %p",queue);
		printf(" fp: %p",queue->front);
		printf(" bp: %p",queue->back); 
		spawn_entity(marked, queue, rand_vector3(x, y, floor_count), 0);
	}
	for(int i= 0; i < 4; i++){
		printf("p: %p",queue);
		printf(" fp: %p",queue->front);
		printf(" bp: %p",queue->back);
		spawn_entity(marked, queue, rand_vector3(x, y, floor_count), 5);
	}
	printf("\n \n");
	for(int i= 0; i < 8; i++){
		printf("p: %p",queue);
		printf(" fp: %p",queue->front);
		printf(" bp: %p",queue->back);
		entity* e = queue_dequeue(queue);
		printf(" id: %d, type: %d, pos: (%d,%d,%d) e: %p\n",
			e->id, e ->type, e->position.x, e->position.y, e->position.z, e);
	}
}
