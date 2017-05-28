#include "marketsim.h"

#ifndef DEBUG
int main(int argc, char *argv[]){
	if (argc < 2) exit(EXIT_FAILURE);
	srand(time(NULL));

	int maxeployees = 10;
	int customerspawns = 50;
	int simulations = 100;
	int eployeespawns;

	switch (argc){
		case 5: maxeployees = atoi(argv[4]);
		case 4:	customerspawns = atoi(argv[3]);
		case 3: simulations = atoi(argv[2]);
		default: break;
	}


	field*** market;
	meta mmi;
	printf("1");
	market = import_market(argv[1], &mmi);
	queue_t *empty_shelfs = queue_new();
	queue_t *queue = queue_new();
	printf("2");
	for(int i = 0; i < simulations; i++){
		//test_spawn(&mmi, queue, empty_shelfs);
		printf("3");
		for(int c = 0; c < customerspawns; c++){
			spawn_entity(&mmi, queue, empty_shelfs,CUSTOMER);
		}

		eployeespawns = mmi.emtpy_count/LISTL;
		for(int e = 0; e < eployeespawns; e++){
			spawn_entity(&mmi, queue, empty_shelfs, EMPLOYEE);
		}
		work_queue(market, &mmi, queue, empty_shelfs);
		print_queue(queue);
	}
	while(!queue_empty(queue)){
		work_queue(market, &mmi, queue, empty_shelfs);
		print_queue(queue);
	}
	printf("4");
	free_market(market, mmi.columns, mmi.stories);
	free_meta(&mmi);
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
