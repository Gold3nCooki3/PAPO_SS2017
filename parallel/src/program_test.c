#include "program_test.h"

vector3
rand_vector3(int x, int y, int z){
	vector3 v = {rand()%x, rand()%y, rand()%z};
	return v;
}

void collect_entities(queue_t* const queue, int rank, int chunk_length, PrintEntity* print_chunk){
	struct queue_node_s *node = queue->front;
		int i = 0;
		while(node != NULL && i < chunk_length){
			entity* e = node->data;
			int pc = (e->path_position > 0) ? ASPathGetCount(e->path) : 0;
			if(rank == MASTER){
				printf("id: %4d, type: %d, pathpos: %3d, pathcount: %3d,  pos: (%2d,%2d,%2d), dest: (%2d,%2d,%2d) \n",
				e->id, e->type,e->path_position, pc, e->position.x, e->position.y, e->position.z, e->list[e->listpos].x, e->list[e->listpos].y, e->list[e->listpos].z);
			}else{
				print_chunk[i].pathpos = e->path_position;
				print_chunk[i].pathcount = pc;
				print_chunk[i].id = e->id;
				print_chunk[i].type = e->type;
				print_chunk[i].posx = e->position.x;
				print_chunk[i].posy = e->position.y;
				print_chunk[i].posz = e->position.z;
				print_chunk[i].destx = e->list[e->listpos].x;
				print_chunk[i].desty = e->list[e->listpos].y;
				print_chunk[i].destz = e->list[e->listpos].z;
			}
			i++;
			node = node->next;
		}
}


void print_queue_parallel(queue_t* const queue, meta *mmi){
	int chunk_length = mmi->entity_count;
	MPI_Request req;
	PrintEntity* print_chunk = malloc(mmi->entity_count * sizeof(print_chunk));
	MPI_Datatype MPI_PrintEntity;
	MPI_Type_contiguous(10, MPI_INT, &MPI_PrintEntity);
	MPI_Type_commit(&MPI_PrintEntity);


	if(mmi->rank == MASTER){
		//print own data
		collect_entities(queue, MASTER, chunk_length, print_chunk);

		// get data
		for(int source = 1; source < mmi->size; source++){
			MPI_Recv(&chunk_length, 1, MPI_INT, source, PRINTTAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			realloc(print_chunk, chunk_length);
			MPI_Recv(print_chunk, chunk_length, MPI_PrintEntity, source, PRINTTAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			// print data
			for(int i = 0; i < chunk_length; i++ ){
				printf("id: %4d, type: %d, pathpos: %3d, pathcount: %3d,  pos: (%2d,%2d,%2d), dest: (%2d,%2d,%2d) \n",
						print_chunk[i].id, print_chunk[i].type, print_chunk[i].pathpos, print_chunk[i].pathcount,
						print_chunk[i].posx, print_chunk[i].posy, print_chunk[i].posz,
						print_chunk[i].destx, print_chunk[i].desty, print_chunk[i].destz);
			}
		}
	}else{
		//collect data
		collect_entities(queue, mmi->rank, chunk_length, print_chunk);

		//send data
		MPI_Isend(&chunk_length, 1, MPI_INT, MASTER, PRINTTAG, MPI_COMM_WORLD, &req);
		MPI_Isend(print_chunk, chunk_length, MPI_PrintEntity, MASTER, PRINTTAG, MPI_COMM_WORLD,  &req);
	}
	free(print_chunk);
}
