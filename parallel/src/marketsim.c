#include "marketsim.h"

int main(int argc, char *argv[]){
	MPI_Init(&argc, &argv);
	printf("Hello Word\n");
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

	//field**** market;
	//meta mmi;
	int rank, size;
	MPI_File in;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	printf("r: %d\n", rank);
	MPI_File_open(MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &in);
	parprocess(&in, rank, size, 0);
	//market = import_market(argv[1], &mmi);
	/*queue_t *empty_shelfs = queue_new();
	queue_t *entity_queue = queue_new();
	for(int i = 0; i < simulations; i++){//Anlaufen
		eployeespawns = mmi.empty_count/employeebag;
		for(int c = 0; c < customerspawns; c++){
			spawn_entity(&mmi, entity_queue, empty_shelfs,CUSTOMER);
			if(c < eployeespawns)spawn_entity(&mmi, entity_queue, empty_shelfs, EMPLOYEE);
		}
		work_queue(&mmi, entity_queue, empty_shelfs);
		print_queue(entity_queue);
	}
	while(!queue_empty(entity_queue)){ //Auslaufen
		eployeespawns = mmi.empty_count/employeebag;
		for(int c = 0; c < eployeespawns; c++){
			spawn_entity(&mmi, entity_queue, empty_shelfs, EMPLOYEE);
		}
		work_queue(&mmi, entity_queue, empty_shelfs);
		print_queue(entity_queue);
	}
	while(!queue_empty(empty_shelfs)){
		free(queue_dequeue(empty_shelfs));
	}*/
	//free_market();
	//free_meta();
	//queue_destroy(entity_queue);
	//queue_destroy(empty_shelfs);
	MPI_Finalize();
	return 0;
}
