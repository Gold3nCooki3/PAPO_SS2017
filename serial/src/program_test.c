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
			printf(" id: %4d, type: %d, pos: (%2d,%2d,%2d), dest: (%2d,%2d,%2d) \n",
				e->id, e ->type, e->position.x, e->position.y, e->position.z, e->list[e->listpos].x, e->list[e->listpos].y, e->list[e->listpos].z, e);
			node = node->next;
		}
		printf("\n \n");
}


int test_vec_equal(){
	int c = 0;
	vector3 vec1 = {1,1,1};
	vector3 vec2 = {1,1,1};
	if(!vec_equal(&vec1, &vec2)) c++;

	vector3 vec3 = { 1, 100 ,1000};
	vector3 vec4 = { 1, 1 ,1 };
	if(vec_equal(&vec3, &vec4)) c++;

	vector3 vec5 = { 0, 0 ,0};
	vector3 vec6 = { 0, 0 ,0};
	if(!vec_equal(&vec5, &vec6)) c++;
	if(c > 0){
		printf("Error test vec\n");
	}return c;
}

field*** test_import_market(char * path,int * y, int * z){
	int c = 0;
	int x;
	meta mmi;
	field*** m = import_market(path, &mmi);
	if(m[0][0][0].type != 0 ||
			m[0][0][0].content != 0 ||
			m[0][0][0].amount != 0)  c++;

	if(m[4][9][9].type != 3 ||
			m[4][9][9].content != 0 ||
			m[4][9][9].amount != 0) c++;

	if(m[3][2][2].type == 1 ||
			m[3][2][1].content != 0 ||
			m[3][2][1].amount != 0) c++;
	if(x != 10 || *y != 10 || *z != 5) c++;
	if(c > 0){
		free_market(m, *y, *z);
		printf("Error test import\n");
		exit(EXIT_FAILURE);
	}
	return m;
}

int test_in_matrix(field*** m){
	vector3 vec1 = {0,0,0};
	field* f = in_matrix(m, vec1);
	int c = 0;
	if(f->type != 0 || f->content != 0 || f->amount != 0) c++;
	return c;
}

int test_isblocked(field*** m){
	int c = 0;
	vector3 vec = { 0, 0 ,0};
	if(is_blocked(m, vec)) c++;

	vector3 vec2 = { 3, 0 ,3};
	if(!is_blocked(m, vec2)) c++;
	return c;
}

void test_market(char* path){
	int a = 0;
	int y, z;
	a += test_vec_equal();
	field*** m = test_import_market(path, &y, &z);
	a += test_in_matrix(m);
	a += test_isblocked(m);
	free_market(m, y, z);
	printf("Es sind %d Fehler aufgetreten\n", a);
	if(a > 0) exit(EXIT_FAILURE);
}

void test_spawn(meta* const mmi, queue_t* const queue,queue_t* const empty_shelfs){
	for(int i= 0; i < 4; i++){
		spawn_entity(mmi, queue, empty_shelfs,  CUSTOMER);
	}
	for(int i= 0; i < 4; i++){
		spawn_entity(mmi, queue, empty_shelfs, EMPLOYEE);
	}
	printf("\n \n");
	print_queue(queue);
	printf("\n \n");
}


