#include "marketsim.h"

int main(int argc, char *argv[]){
	MPI_Init(&argc, &argv);
	//argv: program-name mall-file entity-file max_entities_in_mall outputfile
	if (argc < 5) {
		printf("Usage: program-name mall-hex-file entity-hex-file max_count-entities-in-mall outputfile\n");
		exit(EXIT_FAILURE);
	}
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

	int maxcustomers = atoi(argv[3]);
	field**** market;
	meta *mmi = calloc(1, sizeof(meta));
	MPI_Comm_rank(MPI_COMM_WORLD, &mmi->rank);
	MPI_Comm_size(MPI_COMM_WORLD, &mmi->size);

	market = import_market(argv[1], mmi);
	queue_t *empty_shelfs = queue_new();
	queue_t *entity_queue = queue_new();
	queue_t *pathf_queue = queue_new();
//	if(mmi->exit_count > 0) spawn_entity(mmi, pathf_queue, empty_shelfs ,CUSTOMER);
	
	int current_customers = 0;
	int customerindex = 0;
	MPI_Request req;
	MPI_Status stat;
	MPI_Comm entrygroup;
	int entryrank, entrysize, fileopenerror;
	MPI_File entlist;
	int entlist_size = 0;
	int abort = 0;

	MPI_File output;
	char* outputfilename = argv[4];
	//fileopenerror = MPI_File_open(MPI_COMM_WORLD, outputfilename, MPI_MODE_CREATE|MPI_MODE_RDWR|MPI_MODE_EXCL, MPI_INFO_NULL, &output);
        fileopenerror = MPI_File_open(MPI_COMM_WORLD, outputfilename, MPI_MODE_CREATE|MPI_MODE_RDWR, MPI_INFO_NULL, &output);
	if(fileopenerror != MPI_SUCCESS) {
		printf("Process %i: File (%s) could not be created or already exists!", mmi->rank, outputfilename);
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
	}
	pathWriterInit(output);

	//Init commgroup for entry-nodes
	short color = 0;
	if(mmi->exit_count > 0) color = 1;
	MPI_Comm_split(MPI_COMM_WORLD, color, 0, &entrygroup);
	MPI_Comm_rank(entrygroup, &entryrank);
	MPI_Comm_size(entrygroup, &entrysize);

	//Synchronize splitting
	MPI_Barrier(MPI_COMM_WORLD);
	//Init MPI-IO for entrygroup
	if(mmi->exit_count > 0) {
		fileopenerror = MPI_File_open(entrygroup, argv[2], MPI_MODE_RDONLY, MPI_INFO_NULL, &entlist);
		if(fileopenerror == MPI_SUCCESS) {
			entlist_size = readInit(&entlist);
			printf("entlist_size = %i -- entryrank = %i\n", entlist_size, entryrank);
		} else {
			abort = 1;
		}
	}

	if(abort == 1) {
		printf("Could not open entity-file!\n");
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}

	MPI_Barrier(MPI_COMM_WORLD);
/*
	print_queue_parallel(pathf_queue, mmi, mmi->spawn_count);
	int i = 0, knownPathmax = 10, knownPath_count = 0;
	PS* known_Path = malloc(knownPathmax * sizeof(PS));
	while(i++ < 100){
		work_queue(mmi, entity_queue, empty_shelfs, pathf_queue, known_Path, &knownPathmax, &knownPath_count);
		print_queue_parallel(entity_queue, mmi, mmi->entity_count);
	}
	free(known_Path);
*/
entity* e;
	//Spawn first entity
	if(mmi->exit_count > 0 && entryrank == 0 && entlist_size > 0) {
		e = readEntity(mmi, &entlist, current_customers);
		
		spawn_entity(mmi, pathf_queue, empty_shelfs, e, CUSTOMER);
		current_customers++;
		mmi->spawn_count++;
		customerindex++;
/*                e = readEntity(&entlist, current_customers);
		e->position = mmi->exit_fields[rand() % mmi->exit_count];
		spawn_entity(mmi, pathf_queue, empty_shelfs, e, CUSTOMER);
		current_customers++;
		mmi->spawn_count++;
*/	} else if(entryrank == 0 && mmi->exit_count > 0 && entlist_size == 0) {
		printf("Entityfile seems to be empty. entlist_size = %i -- entryrank = %i\n", entlist_size, entryrank);
	}
	
	print_queue_parallel(pathf_queue, mmi, mmi->spawn_count);
	int i = 0, knownPathmax = 100, knownPath_count = 0;
	PS* known_Path = malloc(knownPathmax * sizeof(PS));

//	while(i++ < 100) {
//		work_queue(mmi, entity_queue, empty_shelfs, pathf_queue, known_Path, &knownPathmax, &knownPath_count);
//		print_queue_parallel(entity_queue, mmi, mmi->entity_count);
//	}
//	free(known_Path);


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
	}*/
        MPI_Barrier(MPI_COMM_WORLD);
	int message[2] = {0,0};
	entity* work_entity;
	while(1==1){
//		eployeespawns = mmi->empty_count/employeebag;
			if(entryrank == 0 && mmi->exit_count > 0) {		//Check if entrygroup-root
				if(customerindex == entlist_size) {		//End spawning and start emptying the queue
					message[0] = -1;
					message[1] = -1;
					for(int r = entrysize-1; r > 0; r++) {
						MPI_Send(&message, 2, MPI_INT, r, 123, entrygroup);
					}
					break;
				}

				if((pathf_queue->length + entity_queue->length) < maxcustomers) {	//If mall not saturated, spawn entities - also use for synchronization as it is blocking
					//Send message to all other spawners
					for(int r = entrysize-1; r >= 0; r++) {
						if(customerindex + (maxcustomers - (pathf_queue->length + entity_queue->length)) < entlist_size) {
							message[1] = (int) ((maxcustomers - (pathf_queue->length + entity_queue->length)) / entrysize)-1;
						} else {
							message[1] = (int) (entlist_size - customerindex) / entrysize-1;
						}
						if((maxcustomers-(pathf_queue->length+entity_queue->length)) % entrysize < r) message[1]++;
						message[0] = customerindex;
						customerindex = customerindex + message[1];
						if(r == 0) break;
						MPI_Send(&message, 2, MPI_INT, r, 123, entrygroup);
					}
					
					//Spawn locally
					for(int s = 0; s < message[1]; s++) {
						work_entity = readEntity(mmi, &entlist, message[0] + s);
						work_entity->position = mmi->exit_fields[rand() % mmi->exit_count];
						spawn_entity(mmi, pathf_queue, empty_shelfs, work_entity, CUSTOMER);
						mmi->spawn_count++;
					}
				} else {
					message[0] = 0;
					message[1] = 0;
					for(int r = entrysize-1; r > 0; r++) {
						MPI_Send(&message, 2, MPI_INT, r, 123, entrygroup);
					}
				}
			} else if(entryrank > 0 && mmi->exit_count > 0) {
				MPI_Recv(&message, 1, MPI_INT, 0, 123, entrygroup, MPI_STATUS_IGNORE);
				
				if(message[0] == -1) {			//Received end of spawning message
					break;
				}

				if(message[0] > 0) {
					for(int s = 0; s < message[1]; s++) {
						work_entity = readEntity(mmi, &entlist, message[0] + s);
						work_entity->position = mmi->exit_fields[rand() % mmi->exit_count];
						spawn_entity(mmi, pathf_queue, empty_shelfs, work_entity, CUSTOMER);

						mmi->spawn_count++;
					}
				}
			}
		
//MPI_Barrier(MPI_COMM_WORLD);
//		if(mmi->exits > 0) {			//Check if entities need to be spawned
//
//		}

//		for(int c = 0; c < current_customers; c++){
//			spawn_entity(mmi, entity_queue, empty_shelfs, EMPLOYEE);
//		}
                MPI_Barrier(MPI_COMM_WORLD);
		work_queue(mmi, entity_queue, empty_shelfs, pathf_queue, known_Path, &knownPathmax, &knownPath_count);
		print_queue_parallel(entity_queue, mmi, mmi->entity_count);
                MPI_Barrier(MPI_COMM_WORLD);
	}
	int still_in_mall_loc = 1, still_in_mall_global = 1;
	while(still_in_mall_global > 0) { //finish all entities
		still_in_mall_loc = pathf_queue->length + entity_queue->length;			//pathf_queue should be 0
		MPI_Allreduce(&still_in_mall_loc, &still_in_mall_global, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
		work_queue(mmi, entity_queue, empty_shelfs, pathf_queue, known_Path, &knownPathmax, &knownPath_count);
		print_queue_parallel(entity_queue, mmi, mmi->entity_count);

	}

	while(!queue_empty(empty_shelfs)){
		free(queue_dequeue(empty_shelfs));
	}
	
	MPI_File_close(&output);
	if(mmi->exit_count > 0) {
		MPI_File_close(&entlist);
	}
	free_market();
	free_meta();
	queue_destroy(entity_queue);
	queue_destroy(pathf_queue);
	queue_destroy(empty_shelfs);
	MPI_Finalize();
	return 0;
}
