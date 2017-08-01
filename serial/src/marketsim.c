#include "marketsim.h"

int main(int argc, char *argv[]){
	if (argc < 2) exit(EXIT_FAILURE);
	srand(time(NULL));

	int maxeployees = 5;
	int customerspawns = 10;
	int simulations = 10;
	int eployeespawns = 1;
	int employeebag = 10;

	switch (argc){
		case 5: maxeployees = atoi(argv[4]);
		case 4:	customerspawns = atoi(argv[3]);
		case 3: simulations = atoi(argv[2]);
		default: break;
	}

	int emp_count = 0;

	field*** market;
	meta mmi;
	mmi.empty_count = 0;

	printf("1");
	market = import_market(argv[1], &mmi);
	queue_t *empty_shelfs = queue_new();
	queue_t *queue = queue_new();
	for(int i = 0; i < simulations; i++){//Anlaufen
		eployeespawns = mmi.empty_count/employeebag;
		for(int c = 0; c < customerspawns; c++){
			spawn_entity(&mmi, queue, empty_shelfs,CUSTOMER);
			if(c < eployeespawns)spawn_entity(&mmi, queue, empty_shelfs, EMPLOYEE);
		}
		work_queue(market, &mmi, queue, empty_shelfs);
		print_queue(queue);
	}
	while(!queue_empty(queue)){ //Auslaufen
		eployeespawns = mmi.empty_count/employeebag;
		for(int c = 0; c < eployeespawns; c++){
			spawn_entity(&mmi, queue, empty_shelfs, EMPLOYEE);
		}
		work_queue(market, &mmi, queue, empty_shelfs);
		print_queue(queue);
	}
	while(!queue_empty(empty_shelfs)){
		free(queue_dequeue(empty_shelfs));
	}
	free_market();
	free_meta();
	queue_destroy(queue);
	queue_destroy(empty_shelfs);
	return 0;
}

