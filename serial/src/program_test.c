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

field*** test_import_market(int * y, int * z, marketmetainfo * mmi){
	int c = 0;
	int x;
	char* path = malloc(sizeof(char)*16);
		path[0] = '.';
		path[1] = '.';
		path[2] = 92;
		path[3] = 't';
		path[4] = 'e';
		path[5] = 's';
		path[6] = 't';
		path[7] = 'd';
		path[8] = 'a';
		path[9] = 't';
		path[10] = 'a';
		path[11] = '.';
		path[12] = 'm';
		path[13] = 'a';
		path[14] = 'l';
		path[15] = 'l';

	field*** m = import_market(path, &x, y, z, mmi);
	free(path);
	if(m[0][0][0].type != 0 ||
			m[0][0][0].content != 100000 ||
			m[0][0][0].amount != 2)  c++;

	if(m[49][10][9].type != 42 ||
			m[49][10][9].content != 42 ||
			m[49][10][9].amount != 42) c++;

	if(m[49][0][9].type == 42 ||
			m[49][10][9].content != 42 ||
			m[49][10][9].amount != 42) c++;

	if(x != 10 || *y != 11 || *z != 50) c++;
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
	if(f->type != 0 || f->content != 100000 || f->amount != 2) c++;
	return c;
}

int test_isblocked(field*** m){
	int c = 0;
	vector3 vec = { 0, 0 ,0};
	if(is_blocked(m, vec)) c++;

	vec.x = 1;
	if(!is_blocked(m, vec)) c++;

	return c;
}

void test_market(){
	int a = 0;
	int y, z;
	marketmetainfo mmi;
	a += test_vec_equal();
	field*** m = test_import_market(&y, &z, &mmi);
	printf("STUFF");
	a += test_in_matrix(m);
	a += test_isblocked(m);
	free_market(m, y, z);
	printf("Es sind %d Fehler aufgetreten\n", a);
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


