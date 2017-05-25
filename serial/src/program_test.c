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


int test_vec_equal(){
	int c = 0;
	vector3 vec1 = { 1, 1 ,1 };
	vector3 vec2 = { 1, 1 ,1 };
	if(!vec_equal(vec1, vec2)) c++;

	vec1 = { 1, 100 ,1000};
	vec2 = { 1, 1 ,1 };
	if(vec_equal(vec1, vec2)) c++;

	vec1 = { 0, 0 ,0};
	vec2 = { 0, 0 ,0 };
	if(vec_equal(vec1, vec2)) c++;
	if(c > 0) printf("Error test vec");
	return c;
}

int test_import_market(field*** m, int * y, int * z){
	int c = 0;
	int x;
	char* path = "..\testdata";
	m = import_market(path, &x, &y, &z);

	if(m[0][0][0].type != 0 ||
			m[0][0][0].content != 100000 ||
			m[0][0][0].amount != 2)  c++;

	if(m[49][10][9].type != 42 ||
			m[49][10][9].content != 42 ||
			m[49][10][9].amount != 42) c++;

	if(m[49][0][9].type == 42 ||
			m[49][10][9].content != 42 ||
			m[49][10][9].amount != 42) c++;

	if(x != 10 || y != 11 || z != 50) c++;
	if(c > 0){
		free_market(m, *y, *z);
		printf("Error test import");
		exit(EXIT_FAILURE);
	}
	return c;
}

int test_in_matrix(field*** m){
	int c = 0;
	Vector3 vec = { 0, 0 ,0};
	field* f = in_matrix(m, vec);
	if(f->type.type != 0 || f.content != 100000 || f.amount != 2) c++;
	return c;
}

int test_isblocked(field*** m){
	int c = 0;
	Vector3 vec = { 0, 0 ,0};
	if(is_blocked(m, vec)) c++;

	vec.x = 1;
	if(!is_blocked(m, vec)) c++;

	return c;
}

void test_marked(){
	int a = 0;
	int y, z;
	field*** m;
	a += test_vec_equal();
	a += test_import_market(m, &y, &z);
	a += test_in_matirx(m);
	a += test_isblocked(m);
	free_market(m, y, z);
	printf("Es sind %d Fehler aufgetreten", a);
	if(a > 0) exit(EXIT_FAILURE);
}

void test_spawn(field*** market, queue_t* queue, int x, int y, int floor_count){
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


