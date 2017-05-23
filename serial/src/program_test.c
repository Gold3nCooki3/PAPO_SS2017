#include "program_test.h"

vector3
rand_vector3(int x, int y, int z){
	vector3 v = {rand()%x, rand()%y, rand()%z};
	return v;
}

void 
test_spawn(field*** marked, queue_t* queue, int x, int y, int floor_count){
	for(int i= 0; i < 4; i++){
		spawn_entity(marked, queue, rand_vector3(x, y, floor_count), 0);
	}
	for(int i= 0; i < 4; i++){
		spawn_entity(marked, queue, rand_vector3(x, y, floor_count), 5);
	}
	printf("\n \n");
	struct queue_node_s *node = queue->front;
	for(int i= 0; i < 8; i++){
		entity* e = node->data;
		printf("p: %12p",queue);
		printf(" fp: %12p",node->next);
		printf(" bp: %12p",node);
		printf(" id: %d, type: %d, pos: (%2d,%2d,%2d) e: %p\n",
			e->id, e ->type, e->position.x, e->position.y, e->position.z, e);
		node = node->next;
	}
}
