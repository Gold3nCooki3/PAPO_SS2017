#include "marketsim.h"

int main(int argc, char *argv[]){
	MPI_Init(&argc, &argv);
	if (argc < 2) exit(EXIT_FAILURE);
	srand(time(NULL));
	/*int maxeployees = 5;
	int customerspawns = 10;
	int simulations = 10;
	int eployeespawns = 1;
	int employeebag = 10;

	switch (argc){
		case 5: maxeployees = atoi(argv[4]);
		case 4:	customerspawns = atoi(argv[3]);
		case 3: simulations = atoi(argv[2]);
		default: break;
	}*/

	field**** market;
	meta *mmi = calloc(1, sizeof(meta));
	MPI_Comm_rank(MPI_COMM_WORLD, &mmi->rank);
	MPI_Comm_size(MPI_COMM_WORLD, &mmi->size);

	market = import_market(argv[1], mmi);
	queue_t *empty_shelfs = queue_new();
	queue_t *entity_queue = queue_new();
	queue_t *pathf_queue = queue_new();
	if(mmi->exit_count > 0) spawn_entity(mmi, pathf_queue, empty_shelfs ,CUSTOMER);
	print_queue_parallel(pathf_queue, mmi, mmi->spawn_count);
	int i = 0, knownPathmax = 10, knownPath_count;
	PS* known_Path = malloc(knownPathmax * sizeof(PS));
	while(i++ < 20){
		work_queue(mmi, entity_queue, empty_shelfs, pathf_queue, known_Path, knownPathmax, knownPath_count);
		print_queue_parallel(entity_queue, mmi, mmi->entity_count);
	}
	free(known_Path);
	/*for(int i = 0; i < simulations; i++){//Anlaufen
		//eployeespawns = mmi->empty_count/employeebag;
		for(int c = 0; c < customerspawns; c++){
			if(mmi->exit_count > 0) spawn_entity(mmi, entity_queue, empty_shelfs,CUSTOMER);
			if(c < eployeespawns)spawn_entity(mmi, entity_queue, empty_shelfs, EMPLOYEE);
		}
		print_queue(entity_queue);
	}
		work_queue(mmi, entity_queue, empty_shelfs);
		print_queue(entity_queue);
	}
	while(!queue_empty(entity_queue)){ //Auslaufen
		eployeespawns = mmi->empty_count/employeebag;
		for(int c = 0; c < eployeespawns; c++){
			spawn_entity(mmi, entity_queue, empty_shelfs, EMPLOYEE);
		}
		work_queue(mmi, entity_queue, empty_shelfs);
		print_queue(entity_queue);
	}
	while(!queue_empty(empty_shelfs)){
		free(queue_dequeue(empty_shelfs));
	}*/
	free_market();
	free_meta();
	queue_destroy(entity_queue);
	queue_destroy(pathf_queue);
	queue_destroy(empty_shelfs);
	MPI_Finalize();
	return 0;
}

