#include "marketsim.h"

#ifndef DEBUG
int main(int argc, char *argv[]){
	if (argc < 2) exit(EXIT_FAILURE);
	srand(time(NULL));

	int eployeespawns = 10;
	int customerspawns = 50;
	int simulations = 100;

	switch (argc){
		case 5: eployeespawns = atoi(argv[4]);
		case 4:	customerspawns = atoi(argv[3]);
		case 3: simulations = atio(argv[2]);
		default: break;
	}


	int x, y, floor_count;
	field*** market;
	queue_t *queue = queue_new();
	queue_t *empty_shelfs = queue_new();
	meta mmi;

	market = import_market(argv[1], &x, &y, &floor_count, &mmi);
	printf("x: %d\n", x);
	printf("y: %d\n", y);
	printf("floor_count: %d\n", floor_count);
	
	for(int i = 0; i < simulations; i++){
		//test_spawn(&mmi, queue, empty_shelfs);
		for(int c = 0; c < customerspawns; c++){
			spawn_entity(mmi, queue, empty_shelfs, CUSTOMER);
		}
		for(int e = 0; e < eployeespawns; e++){
			spawn_entity(mmi, queue, empty_shelfs, EMPLOYEE);
		}
		work_queue(market, &mmi, queue, empty_shelfs);
		print_queue(queue);
	}
	while(!queue_empty(queue)){
		work_queue(market, &mmi, queue, empty_shelfs);
		print_queue(queue);
	}
	free_market(market, y, floor_count);
	queue_destroy(queue);
	queue_destroy(empty_shelfs);
	return 0;
}
#else
int main (int argc, char *argv[]){
	printf("DEBUG\n");
	printf("DEBUG\n");
	test_market(argv[1]);
}
#endif
