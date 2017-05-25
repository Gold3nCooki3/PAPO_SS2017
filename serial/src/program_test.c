#include "program_test.h"

vector3
rand_vector3(int x, int y, int z){
	vector3 v = {rand()%x, rand()%y, rand()%z};
	return v;
}

void print_queue(queue_t* queue){
	struct queue_node_s *node = queue->front;
		while(node != NULL){
			entity* e = node->data;
			printf("p: %12p",queue);
			printf(" pn: %12p",queue->front);
			printf(" fp: %12p",node->next);
			printf(" bp: %12p",node);
			printf(" id: %d, type: %d, pos: (%2d,%2d,%2d), dest: (%2d,%2d,%2d) \n",
				e->id, e ->type, e->position.x, e->position.y, e->position.z, e->list[e->listpos].x, e->list[e->listpos].y, e->list[e->listpos].z, e);
			node = node->next;
		}
		printf("\n \n");
}

void 
test_spawn(field*** market, queue_t* queue, int x, int y, int floor_count){
	for(int i= 0; i < 4; i++){
		spawn_entity(market, queue, rand_vector3(x, y, floor_count), 0);
	}
	for(int i= 0; i < 4; i++){
		spawn_entity(market, queue, rand_vector3(x, y, floor_count), 5);
	}
	printf("\n \n");
	print_queue(queue);
	printf("\n \n");
}


