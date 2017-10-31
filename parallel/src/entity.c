#include "entity.h"

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

/*============================================HELP FUNCTIONS PATHFINDING======================================*/
typedef struct vector3 PathNode;

static void PathNodeNeighbors(ASNeighborList neighbors, void *node,
		void *context) {

	PathNode *pathNode = (PathNode *) node;

	if (!is_blocked(
			(PathNode ) { pathNode->x + 1, pathNode->y, pathNode->z })) {
		ASNeighborListAdd(neighbors, &(PathNode ) { pathNode->x + 1,
						pathNode->y, pathNode->z }, 1);
	}
	if (!is_blocked(
			(PathNode ) { pathNode->x - 1, pathNode->y, pathNode->z })) {
		ASNeighborListAdd(neighbors, &(PathNode ) { pathNode->x - 1,
						pathNode->y, pathNode->z }, 1);
	}
	if (!is_blocked(
			(PathNode ) { pathNode->x, pathNode->y + 1, pathNode->z })) {
		ASNeighborListAdd(neighbors,
				&(PathNode ) { pathNode->x, pathNode->y + 1, pathNode->z },
				1);
	}
	if (!is_blocked(
			(PathNode ) { pathNode->x, pathNode->y - 1, pathNode->z })) {
		ASNeighborListAdd(neighbors,
				&(PathNode ) { pathNode->x, pathNode->y - 1, pathNode->z },
				1);
	}
}

static float PathNodeHeuristic(void *fromNode, void *toNode, void *context) {
	PathNode *from = (PathNode *) fromNode;
	PathNode *to = (PathNode *) toNode;
	if (from->z != to->z){
//TODO!		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
//		exit(EXIT_FAILURE);
		return 0;
	}
	return (fabs(from->x - to->x) + fabs(from->y - to->y));
}

int has_path(entity* const e) {
	if (e->path_position > 0 && ASPathGetCount(e->path) > 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}

static const ASPathNodeSource PathNodeSource = { sizeof(PathNode),
		&PathNodeNeighbors, &PathNodeHeuristic, NULL, NULL };
/*===================================================================================================*/
void force_stop() {
	free_market();
	free_meta();
	MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
	exit(5);
}


PE * fast_realloc_PE(PE * ptr, int count, int * max, int size){
			if(count >= *max - 1){
					*max += size;
					PE* p = realloc(ptr, *max * sizeof(PE));
					return p;
			}
			return ptr;
}

void fast_realloc_PS(PathArrays* PA, int * count, int * max, int size){
			if(*count >= *max -1){
					*max += size;
					PA->known_Path = realloc(PA->known_Path, *max * sizeof(PS));
			}
}

void fast_realloc(entity *** ptr, int * count, int * max, int index, int size){
			if(count[index] >= max[index]-1){
					max[index] += size;
					ptr[index] = realloc(ptr[index], max[index] * sizeof(entity*));
			}
}



vector3 get_close_vector3(vector3* const list, int listlength, vector3 start,
		int forceSameLevel) {
	vector3 dest;
	int mindistance = -1;
	int distance;
	for (int i = 0; i < listlength; i++) {
		distance = abs(list[i].x - start.x) + abs(list[i].y - start.y) + 10
				+ abs(list[i].z - start.z);
		if ((!forceSameLevel || list[i].z == start.z)
				&& (distance < mindistance || mindistance == -1)) {
			mindistance = distance;
			dest = list[i];
		}
	}
	if (mindistance == -1) {
		printf("error");
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}
	return dest;
}

int compfunc(const void * a, const void * b) {
	vector3 vec_a = *(vector3*) a;
	vector3 vec_b = *(vector3*) b;
	int dista =
			(start_vec.z == vec_a.z) ?
					abs(start_vec.x - vec_a.x) - abs(start_vec.y - vec_a.y) :
					-1;
	int distb =
			(start_vec.z == vec_b.z) ?
					abs(start_vec.x - vec_b.x) - abs(start_vec.y - vec_b.y) :
					-1;

	if (dista > distb)
		return -1;
	if (dista == distb)
		return 0;
	if (dista < distb)
		return 1;

	return 0;
}


int find_edge_field(vector3* edge_fields, vector3 start_vec, int edge_count, int start){
	for (int c = start; c < edge_count; c++) {
					ASPath path = ASPathCreate(&PathNodeSource, NULL, &start_vec,
							&(edge_fields[c]));
					if (ASPathGetCount(path) > 0) { //is path vaild
						return c;
					}
				}
	return ERR;
}

void replace_dublicates(int* targets){
	for(int o = 0; o < 6; o++){
		if(targets[o] < 0) continue;
		int temp = targets[o];
		for(int i = o+1; i < 6; i++){
			if(temp == targets[i]) targets[i] = -1;
		}
	}
}

/*========================================================================================*/

ASPath generate_localpath(vector3 start, vector3 dest, meta* const mmi,
		int* lift_flag, vector3 * memory) {
	PathNode pathFrom = (PathNode) start;
	vector3 pathTo_v = dest;

	if (pathTo_v.z != start.z) {/*
		if (mmi->lift_count > 0) {
			//printf(" searching for lift \n");
			pathTo_v = get_close_vector3(mmi->lift_fields, mmi->lift_count,
					start, TRUE);
		} else {
			*lift_flag = TRUE;
			return NULL;
		}*/
                *lift_flag = FALSE;
                return NULL;
	} else if (in_process(dest)) {
		switch (in_matrix_g(dest)->type) {
		case REGISTER:
		case SHELF:
			pathTo_v.x++;
			if (is_blocked(pathTo_v)) {
				pathTo_v.x -= 2;
				if (is_blocked(pathTo_v)) {
					pathTo_v.x += 1;
					pathTo_v.y += 1;
					if (is_blocked(pathTo_v)) {
						pathTo_v.y -= 2;
						if (is_blocked(pathTo_v)) {

						}
					}
				}
			}
			break;
		default:
			break;
		}
	} else {
		//search
		return NULL;
	}

	PathNode pathTo = (PathNode) pathTo_v;
	ASPath path = ASPathCreate(&PathNodeSource, NULL, &pathFrom, &pathTo);
	if (ASPathGetCount(path) == 0) { //not vaild
		return NULL;
	}
	*memory = pathTo_v;
	//printf("mem dest : (%d %d %d), Tpye %d", pathTo.z, pathTo.x, pathTo.z, in_matrix_g(pathTo)->type);
	return path;
}


void split_one_side(meta* const mmi, int side, int *tracker_ts, int *tracker_os,
		int* ts_max, int* os_max, PathArrays* const PA, PE* const other) {
	PE* local_ts = (side == 1) ? PA->local_r : PA->local_l;
	PE* local_os = (side == 1) ? PA->local_l : PA->local_r;
	int count_new_ts = (side == 1) ? PA->new_c_r : PA->new_c_l;
	int count_new_os = (side == 1) ? PA->new_c_l : PA->new_c_r;
	int count_ts = (side == 1) ? PA->rightcount : PA->leftcount;
	int count_os = (side == 1) ? PA->leftcount : PA->rightcount;
	int count_core_ts = (side == 1) ? PA->core_c_r : PA->core_c_l;
	int find_next_part = TRUE;

	//printf("R: %d in split\n", mmi->rank);
	for (int o = 0; o < count_new_ts; o++) {
		for (int i = count_ts - 1; i >= 0; i--) {
			printf("looping oid %d lid %d\n", other[o].id, local_ts[i].id);
			if (local_ts[i].id == other[o].id) {
				find_next_part = FALSE;
				printf("R: %d known other status %d\n", mmi->rank, other[o].status);
				if (other[o].status == ERR) { //NO PATH FOUND
					if (local_ts[i].status < mmi->edge_count) { //SWAP TO END TO SEND AGAIN
						qsort(mmi->edge_fields, mmi->edge_count,
								sizeof(vector3), compfunc);
						local_ts[i].status = local_ts[i].status + 1;
						int next_field = find_edge_field(mmi->edge_fields,
								local_ts[i].start, mmi->edge_count,
								local_ts[i].status);
						local_ts[i].dest = mmi->edge_fields[next_field];
						PE temp = local_ts[i];
						local_ts[i] = local_ts[count_ts - *tracker_ts - 1];
						local_ts[count_ts - 1 - (*tracker_ts)++] = temp;
					} else { //SWAP R -> L & DELETE IN R
						local_ts[i].status = ERR;
						PE temp = local_ts[i];
						local_ts[i] = local_ts[count_ts - *tracker_ts - 1];
						if (*tracker_ts > 1) {
							local_ts[count_ts - *tracker_ts - 1] =
									local_ts[count_ts - 1];
							count_ts--;
						}
						local_os = fast_realloc_PE(local_os,
								count_os + *tracker_os, os_max, count_new_ts);
						local_os[count_os + (*tracker_os)++] = temp;
					}
					break;
				} else if (other[i].status == COMPL) { //PATH FOUND
					if (i >= count_core_ts) { //SWAP R -> L & SAVE PATH & DELETE IN R
						local_ts[i].status = COMPL;
						PE temp = local_ts[i];
						local_ts[i] = local_ts[count_ts - *tracker_ts - 1];
						if (*tracker_ts > 1) {
							local_ts[count_ts - *tracker_ts - 1] =
									local_ts[count_ts - 1];
							count_ts--;
						}
						local_os = fast_realloc_PE(local_os,
								count_os + *tracker_os, os_max, count_new_ts);
						local_os[count_os + (*tracker_os)++] = temp;
						fast_realloc_PS(PA, PA->knownPath_count,
								PA->knownPathmax, 10);
						PA->known_Path[*(PA->knownPath_count)].id = temp.id;
						PA->known_Path[(*(PA->knownPath_count))++].dest =
								temp.dest;
						printf("R %d OTHER COMPL \n", mmi->rank);
					} else {
						printf("R %d LOCAL COMPL \n", mmi->rank);
						PA->not_completed--;
					}
					break;
				} else {
					ASPath exist = ASPathCreate(&PathNodeSource, NULL,
							&other[o].dest, &local_ts[i].start);
					if (ASPathGetCount(exist) > 0) { //Check if Way is necessary
						//send err
						local_ts[count_ts - 1 - *tracker_ts] = other[o];
						local_ts[count_ts - 1 - (*tracker_ts)++].status = ERR;
					} else {
						find_next_part = TRUE;
					}
					break;
				}
			} else {
				if (i == 0)
					find_next_part = TRUE;
			}
		}
		// NOT FOUND IN LOCAL OR FOUND IN LOCAL BUT NEEDS SECOND PASSING
		if (find_next_part) {
			int liftflag;
			vector3 dontneed;

			printf("dest (%d, %d, %d) final dest(%d, %d, %d)\n",
					other[o].dest.x, other[o].dest.y, other[o].dest.z,
					other[o].final_dest.x, other[o].final_dest.y,
					other[o].final_dest.z);
			ASPath tempPath = generate_localpath(other[o].dest,
					other[o].final_dest, mmi, &liftflag, &dontneed);
			if (ASPathGetCount(tempPath) > 0) {
				local_ts = fast_realloc_PE(local_ts, (count_ts + *tracker_os),
						ts_max, count_core_ts);
				local_ts[count_ts + *tracker_ts] = other[o];
				local_ts[count_ts + (*tracker_ts)++].status = COMPL;
				printf("R: %d FOUND PATH *tracker %d, status %d\n", mmi->rank, *tracker_ts,  local_ts[count_ts + (*tracker_ts)-1].status);
			} else {
				printf("R: %d dindt found PATH SPLITTING \n", mmi->rank);
				other[o].start = other[o].dest;
				start_vec = other[o].start;
				qsort(mmi->edge_fields, mmi->edge_count, sizeof(vector3),
						compfunc);
				other[o].status = find_edge_field(mmi->edge_fields, start_vec,
						mmi->edge_count, 0);
				other[o].dest = mmi->edge_fields[other[o].status];
				int destid = other[o].dest.y + other[o].dest.z * mmi->columns;
				if ((destid > mmi->startline && (side == 1))
						|| (destid <= mmi->startline && (side != 1))) {
					local_ts = fast_realloc_PE(local_ts,
							(count_ts + *tracker_ts), ts_max, count_new_os);
					local_ts[count_ts + (*tracker_ts)++] = other[o];
					// loacl_ts[count_ts + *tracker_ts].status = ERR;
				} else {
					local_os = fast_realloc_PE(local_os,
							(count_os + *tracker_os), os_max, count_new_ts);
					local_os[count_os + (*tracker_os)++] = other[o];
				}
			}
		}

	}

}

void recursiv_split(meta* const mmi, PathArrays* const PA, PE * const other_r, PE * const other_l){

	int leftmax = PA->leftcount + PA->new_c_l + 1, rightmax = PA->rightcount + PA->new_c_r + 1, tracker_r = 0, tracker_l = 0 ;

	PA->local_r = realloc (PA->local_r, rightmax * sizeof(PE));
	PA->local_l = realloc (PA->local_l, leftmax * sizeof(PE));

	split_one_side(mmi, 1, &tracker_r, &tracker_l, &rightmax, &leftmax, PA, other_r);
	split_one_side(mmi, 0, &tracker_l, &tracker_r, &leftmax, &rightmax, PA, other_l);
	if(mmi->rank == 1){
		printf("links %d rechts %d\n", tracker_l, tracker_r);
		printf("1.0 id %d status %d\n", PA->local_l[0].id, PA->local_l[0].status);
	}
	PA->rightcount = PA->rightcount + tracker_r;
	PA->leftcount  = PA->leftcount + tracker_l;
	PA->new_c_r = tracker_r;
	PA->new_c_l = tracker_l;

}

void generate_paths(queue_t* const queue, meta* const mmi, PS* known_Path, int* knownPathmax, int* knownPath_count) {
	struct queue_node_s *node = queue->front;
	int rightcount = 0, leftcount = 0, rank = mmi->rank, size = mmi->size;
	MPI_Request req_c_r, req_c_l, req_r, req_l;

	PE* local_r = malloc(mmi->spawn_count * sizeof(PE));
	PE* local_l = malloc(mmi->spawn_count * sizeof(PE));

	MPI_Datatype MPI_PathE;
	MPI_Type_contiguous(11, MPI_INT, &MPI_PathE);
	MPI_Type_commit(&MPI_PathE);

	while (node != NULL) {
		entity* e = node->data;
		int lift_flag = FALSE;
		printf("e->list[e->listpos]: %i,%i,%i", e->list[e->listpos].x, e->list[e->listpos].y, e->list[e->listpos].z);
                if(e->list[e->listpos].z != e->list[e->listpos+1].z) {
                        e->listpos++;
                        printf("WECHSLE STOCKWERK!");
                        continue;
                }
		ASPath tempPath = generate_localpath(e->position, e->list[e->listpos],
				mmi, &lift_flag, &e->memory_dest);
		if (ASPathGetCount(tempPath) > 0) {
			e->path = tempPath;
		} else {
			start_vec = e->position;
			for(int counter = 0; counter < *knownPath_count; counter++){
				if (e->id == known_Path[counter].id) {
					e->path = ASPathCreate(&PathNodeSource, NULL, &start_vec, &known_Path[counter].dest);
				}
			}
			int result = -1;

			qsort(mmi->edge_fields, mmi->edge_count, sizeof(vector3), compfunc);

			/**/
			for (int c = 0; c < mmi->edge_count; c++)
				printf(" [%2d, %2d, %2d];", mmi->edge_fields[c].x,
						mmi->edge_fields[c].y, mmi->edge_fields[c].z);
			printf("\n");
			//force_stop();
			/**/

			for (int c = 0; c < mmi->edge_count; c++) {
				//printf("path %d \n", c);
				printf("start  [%d, %d, %d]\n", start_vec.x, start_vec.y, start_vec.z);
				printf("edge  [%d, %d, %d] ", mmi->edge_fields[c].x, mmi->edge_fields[c].y, mmi->edge_fields[c].z);
				//printf("in P %d\n", in_process(mmi->edge_fields[c]));
				ASPath path = ASPathCreate(&PathNodeSource, NULL, &start_vec,
						&(mmi->edge_fields[c]));
				//printf("path %d \n", c);
				if (ASPathGetCount(path) > 0) { //is path vaild
					result = c;
					e->path = path;
					break;
				}else if (c == mmi->edge_count - 1) {
					printf("Error");
					MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
					exit(EXIT_FAILURE);
				}
			}
			printf("\n Edge [%d %d %d] \n", mmi->edge_fields[result].x, mmi->edge_fields[result].y, mmi->edge_fields[result].z);

			PE p = { e->id, result, start_vec, mmi->edge_fields[result],
					e->list[e->listpos] };
			int destid = e->list[e->listpos].y + e->list[e->listpos].z * mmi->columns;
			if ( destid > mmi->startline ) {
				local_r[rightcount++] = p;
			} else {
				local_l[leftcount++] = p;
			}
		}
		node = node->next;
	}

	int core_c_r = rightcount;
	int core_c_l = leftcount;
	int new_c_r = rightcount;
	int new_c_l = leftcount;
	int not_completed = rightcount + leftcount;

	PE * other_r = NULL;
	PE * other_l = NULL;

	PathArrays PA = { local_r, local_l, known_Path, knownPathmax, knownPath_count,
			rightcount, leftcount, core_c_r ,core_c_l, new_c_r, new_c_l, not_completed};

	//int max_iter = 0;
	while (TRUE) {
		int pos_r = PA.rightcount - PA.new_c_r;
		int pos_l = PA.leftcount - PA.new_c_l;

		MPI_Status st_1, st_2, st_3, st_4;
                int temp;
                MPI_Allreduce(&PA.not_completed, &temp, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
                if(temp <= 0) break;
		if( rank == 0 ) printf("Z: %d\n", temp);

		if (rank < size - 1) {
			MPI_Isend(&PA.new_c_r, 1, MPI_INT, rank + 1, PATHTAG-1, MPI_COMM_WORLD, &req_c_r);
			MPI_Isend(&PA.local_r[pos_r], PA.new_c_r, MPI_PathE, rank + 1, PATHTAG-2,
					MPI_COMM_WORLD, &req_r);
			if(rank == 0) printf("0 : SEND dest (%d %d %d) , pos_r %d new_c %d\n", PA.local_r[0].dest.x, PA.local_r[0].dest.y, 
							PA.local_r[0].dest.z, pos_r, PA.new_c_r);

		}
		if (rank > 0) {
			MPI_Isend(&PA.new_c_l, 1, MPI_INT, rank - 1, PATHTAG +1, MPI_COMM_WORLD, &req_c_l);
			MPI_Isend(&PA.local_l[pos_l], PA.new_c_l, MPI_PathE, rank - 1, PATHTAG+2,
					MPI_COMM_WORLD, &req_l);
			 if(rank == 1) {
				printf("1: lcount %d ", PA.leftcount);
				printf("1: id %d statuts %d pos %d\n", PA.local_l[0].id, PA.local_l[0].status, pos_l);
			}
		}


		if (rank > 0) { /* other fields*/
			MPI_Recv(&PA.new_c_l, 1, MPI_INT, rank - 1, PATHTAG -1 , MPI_COMM_WORLD, &st_1);
			other_l = calloc(PA.new_c_l, sizeof(PE));
			MPI_Recv(other_l, PA.new_c_l, MPI_PathE, rank - 1, PATHTAG -2, MPI_COMM_WORLD, &st_2);
			if(PA.new_c_l > 0) printf("RECV start (%d %d %d)\n", other_l[0].dest.x, other_l[0].dest.y, other_l[0].dest.z);

		}
		if (rank < size - 1) {
			MPI_Recv(&PA.new_c_r, 1, MPI_INT, rank + 1, PATHTAG +1, MPI_COMM_WORLD, &st_3);
			other_r = calloc(PA.new_c_r, sizeof(PE));
			MPI_Recv(other_r, PA.new_c_r, MPI_PathE, rank + 1, PATHTAG +2, MPI_COMM_WORLD, &st_4);
			if(rank == 0) printf("RECV 0: id: %d status: %d\n", other_r[0].id, other_r[0].status);
		}

		/*find path for left and right*/
		/*switch start = dest, find new dest*/
		if (rank < size-1){
			MPI_Wait(&req_c_r, &st_1);
			MPI_Wait(&req_r, &st_2);
	      		MPI_Request_free(&req_c_r);
			MPI_Request_free(&req_r);
		}
		if (rank > 0){
			MPI_Wait(&req_c_l, &st_3);
			MPI_Wait(&req_l, &st_4);
			MPI_Request_free(&req_c_l);
			MPI_Request_free(&req_l);
		}

		recursiv_split(mmi, &PA, other_r, other_l);



		//MPI_Allreduce(&PA.not_completed, &temp, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
		if( rank == 0 ) printf("ZZ: %d\n", PA.not_completed);
	//if(temp <= 0) break;
	}

	int i = 0, r= 0, l=0;
	node = queue->front;
	while( i < core_c_l + core_c_r && node != NULL){
		entity* e = node->data;
		if(local_r[r].id == e->id){
			e->path = ASPathCreate(&PathNodeSource, NULL, &e->position, &local_r[r++].dest);
		}else if(local_l[l].id == e->id){
			e->path = ASPathCreate(&PathNodeSource, NULL, &e->position, &local_l[l++].dest);
		}else{
			printf("This should not have happend\n");
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
			exit(3);
		}
		node = node->next;
		i++;
	}

	mmi->entity_count += mmi->spawn_count;
	mmi->spawn_count = 0;
	free(PA.local_r);
	free(PA.local_l);
	free(other_r);
	free(other_l);
}

/*Moves an entity towards their destination,
 * to pick up or fill up content from a frame
 * @param market	: market where entity is moving within
 * @param e			: entity
 * @return FALSE if person has reached their final destination
 */
EnS move_entity(meta* const mmi, queue_t* const empty_shelfs, entity* const e) {

	//Get to new Position
	PathNode* new_pos = ASPathGetNode(e->path, e->path_position);
	e->position = *new_pos;
	e->path_position++;

	EnS return_val = ENQUEUE;
	// exit
	if (ASPathGetCount(e->path) == e->path_position) {
		field* f;
		vector3* temp_vec;
		//printf("PF_ POS: (%d, %d, %d) -> TPYE: %d\n", e->position.x, e->position.y, e->position.z, in_matrix_g(e->memory_dest)->type);
		switch (in_matrix_g(e->memory_dest)->type) {
		case STOCK:
		case EXIT:
			writePath(e->path, e);
			ASPathDestroy(e->path);
			return DESTROY;
			break;
		case ESCALATOR:
		case LIFT:
			return_val = NEWPATH;
			if (e->position.z > e->list[e->listpos].z) {
				e->position.z -= 1;
				if (!in_process(e->position))
					return_val = DOWN;
			} else {
				e->position.z += 1;
				if (!in_process(e->position))
					return_val = UP;
			}
                        new_pos = ASPathGetNode(e->path, e->path_position);
                        e->position = *new_pos;
                        e->path_position++;
                        return_val = NEWPATH;
			//printf("PF_ Taking the lift %d, iP: %d\n", return_val, !in_process(e->position));
			break;
		case CORRIDOR:
			if (!in_process(e->list[e->listpos])) {
				return_val = (e->position.y + e->position.z * mmi->columns == mmi->startcolumn) ? EDGEL : EDGER;
			} else {
				f = in_matrix_g(e->list[e->listpos]);
				if (e->type == CUSTOMER) {
					if (f->amount > 0) {
						f->amount -= -1;
					} else if (f->amount == 0) { //collect empty fields
						f->amount = -1;
						temp_vec = malloc(sizeof(vector3));
						memcpy(temp_vec, &e->list[e->listpos], sizeof(vector3));
						mmi->empty_count++;
						queue_enqueue(empty_shelfs, temp_vec);
					}
				} else if (e->type == EMPLOYEE) {
					f += FILLVAL;
				}
				e->listpos++;
				return_val = NEWPATH;
			}
		default:
			break;
		}
		writePath(e->path, e);
		ASPathDestroy(e->path);
		e->path_position = 0;
	} else if (e->listpos >= e->amountItems) { //
		printf("Error: no exit");
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}
	return return_val;
}


void find_second_limits(meta* const mmi, int * second_ulimit, int * second_llimit, int * upper_rank, int * lowerrank) { //BruteForce
	int resultup = 0, resultdown = 0, temp_lup = 0, temp_ldown = 0, limitdown, limitup, helpval;

	if(mmi->rank < mmi->size-1){
		helpval = (mmi->startline + (mmi->chunksize / mmi->rows) - 2 + mmi->columns);
		limitup = (mmi->linecount / mmi->size) * (mmi->rank+2) + MIN((mmi->rank+2),(mmi->linecount%mmi->size));
		for (int r = mmi->rank + 1; r < mmi->size; r++) {
		resultup = r;
			temp_lup = (mmi->linecount / mmi->size)
				* r + MIN(r,(mmi->linecount%mmi->size));
			if (helpval < temp_lup){
				break;
			}else{
				limitup = temp_lup;
			}
		}
	}else{
		resultup = 0;
		limitup = mmi->linecount;
	}

	*upper_rank = resultup - 1;
	*second_ulimit = limitup;

	if(mmi->rank > 0){
		helpval = (mmi->startline - mmi->columns);
		limitdown = (mmi->linecount / mmi->size) * (mmi->rank-1) + MIN((mmi->rank-1),mmi->linecount%mmi->size);
		for (int r = mmi->rank - 1; r >= 0; r--) {
			resultdown = r;
			temp_ldown = (mmi->linecount / mmi->size) * r+ MIN(r,mmi->linecount%mmi->size);
			if (helpval >= temp_ldown){
				break;
			}else{
				limitdown = temp_ldown;
			}
		}
	}else{
		resultdown = -2;
		limitdown = -1;
	}

	*lowerrank = resultdown;
	*second_llimit = limitdown;
}


/*Move every entity in the entity_queue, dequeue if entity reached final destination
 * @param market 	: fields where entities move within
 * @param entity_queue 	: queue of all entities
 */
void work_queue(meta * const mmi, queue_t* const entity_queue,
		queue_t* const empty_shelfs, queue_t* const pathf_queue,  PS* known_Path, int* knownPathmax, int* knownPath_count) {
	MPI_Request req;
	if (!queue_empty(pathf_queue)) {
		generate_paths(pathf_queue, mmi, known_Path, knownPathmax, knownPath_count);
		while (!queue_empty(pathf_queue)) {
			entity* e = queue_dequeue(pathf_queue);
			queue_enqueue(entity_queue, e);
		}
	} else {
		generate_paths(pathf_queue, mmi, known_Path, knownPathmax, knownPath_count);
	}
	/*=============================*/
	/*s_L rank-1, s_R rank+1 , s_ol & s_oR depends on market separation */
	entity*** send_entities = calloc(6, sizeof(entity*));
	for(int i = 0; i < 6; i++){
		send_entities[i] = calloc(10, sizeof(entity*));
	}
	int lowerlimit = (mmi->rank == 0) ? -1 : (mmi->linecount / mmi->size) * (mmi->rank-1) + MIN(mmi->rank-1,mmi->linecount%mmi->size);
	int upperlimit = (mmi->rank < mmi->size-2) ? mmi->linecount : (mmi->linecount / mmi->size) * (mmi->rank+2) + MIN((mmi->rank+2),mmi->linecount%mmi->size);
	int second_ulimit=0, second_llimit=0, upper_rank, lower_rank;
	find_second_limits(mmi, &second_ulimit, &second_llimit, &upper_rank, &lower_rank);
	//printf("Rank : %d, LIMITS UP %d, LW %d, SUP %d, SLW %d\n", mmi->rank, upperlimit, lowerlimit, second_ulimit, second_llimit);
	int targets[6] = { mmi->rank-1, mmi->rank+1, lower_rank+1, upper_rank-1, lower_rank, upper_rank };
	replace_dublicates(targets);


	//printf("R: %d", mmi->rank);
	//for(int i = 0; i < 6; i++) printf(" %d ", targets[i]);
	//printf("\n");

	int maxima[6] = { 10, 10, 10, 10, 10, 10 };
	int s_counts[6] = { 0, 0, 0, 0, 0, 0 };

		/*=============================*/
	 if (queue_empty(entity_queue)) {

	} else {
	entity* first = queue_dequeue(entity_queue);
	entity* e = first;
	EnS statusfirst;
	do {
			if (!first) first = e;
			EnS status = move_entity(mmi, empty_shelfs, e);
			if(e == first) statusfirst = status;
			//printf("status: %d\n", status);
			switch (status) {
				case ENQUEUE: 	mmi->entity_count++;
						queue_enqueue(entity_queue, e); break;
				case NEWPATH: 	mmi->spawn_count++;
						queue_enqueue(pathf_queue, e);break;
				case DESTROY:	free(e->list);
						if (e == first) first = NULL;
						free(e);
						break;
				case DOWN: 	if((e->position.y + e->position.z * mmi->columns) < lowerlimit){
							if (e == first) first = NULL;
							if((e->position.y + e->position.z * mmi->columns) < second_llimit){
								fast_realloc(send_entities,s_counts, maxima,4, 10); //ooL
								send_entities[4][s_counts[4]] = e;
								s_counts[4] += 1;
							}else{
								fast_realloc(send_entities,s_counts, maxima,2, 10); //oL
								send_entities[2][s_counts[2]] = e;
								s_counts[2] += 1;
							}
						break;
						}
				case EDGEL: 	if (e == first) first = NULL;
						fast_realloc(send_entities,s_counts, maxima,0, 20);
						send_entities[0][s_counts[0]++] = e;
						s_counts[0] +=	1; break;
				case UP:	if((e->position.y + e->position.z * mmi->columns) > upperlimit){
							if (e == first) first = NULL;
							if((e->position.y + e->position.z * mmi->columns) > second_ulimit){ //ooR
								fast_realloc(send_entities,s_counts, maxima,5, 10);
								send_entities[5][s_counts[5]] = e;
								s_counts[5] += 1;
 							}else{
 								fast_realloc(send_entities,s_counts, maxima,3, 10);
								send_entities[3][s_counts[3]] = e;
								s_counts[3] += 1;
							}
						break;
						}
				case EDGER: 	if (e == first) first = NULL;
						fast_realloc(send_entities,s_counts, maxima,1, 10);
						send_entities[1][s_counts[1]] = e;
						s_counts[1] += 1;	break;
				default: 	break;
			}
			mmi->entity_count--;
			if (queue_empty(entity_queue)) break;
			e = queue_dequeue(entity_queue);
	} while (first != e);

	if ((statusfirst == ENQUEUE) && (first == e)){
	//	printf("!!!! ENQUEUE AGAIN \n");
		queue_enqueue(entity_queue, e); // otherwise one element gets lost
	}

	}

	//printf("??? is Q empty %d\n", queue_empty(entity_queue));

	EssentialEntity** send_buffers = calloc(6, sizeof(EssentialEntity*));
	/*=============================*/
	MPI_Datatype MPI_Entity;
	MPI_Type_contiguous(10 + 3*LISTL, MPI_INT, &MPI_Entity);
	MPI_Type_commit(&MPI_Entity);
	//SEND ENTITIES
	for(int i = 0; i < 6; i++){
		//printf("R %d Sending to %d c %d\n",mmi->rank, targets[i], s_counts[i]);
		if(targets[i] >= mmi->size || targets[i] < 0 || targets[i] == mmi->rank) continue;
		send_entities[i] = realloc(send_entities[i], s_counts[i]*sizeof(entity));
		EssentialEntity* entities_tosend = calloc(s_counts[i], sizeof(EssentialEntity));
		for(int c = 0;c < s_counts[i];c++){
			entities_tosend[c].type   = send_entities[i][c]->type;
			entities_tosend[c].id     = send_entities[i][c]->id;
			entities_tosend[c].listpos= send_entities[i][c]->listpos;
			entities_tosend[c].amountItems = send_entities[i][c]->amountItems;
			entities_tosend[c].position = send_entities[i][c]->position;
			memcpy(entities_tosend[c].list, send_entities[i][c]->list ,sizeof(vector3) * send_entities[i][c]->amountItems);
			free(send_entities[i][c]->list);
			free(send_entities[i][c]); // BYE BYE
		}
		//if(mmi->rank == 1)printf("!!rank 1 %d\n", s_counts[i]);
		MPI_Isend(&s_counts[i], 1, MPI_INT, targets[i], ENTITYTAG, MPI_COMM_WORLD, &req);
		MPI_Isend(entities_tosend, s_counts[i],  MPI_Entity, targets[i], ENTITYTAG, MPI_COMM_WORLD, &req);
		send_buffers[i] = entities_tosend;
		//if(mmi->rank == 0 && targets[i] == 1) printf("R 0: COUNT SEND %d \n", s_counts[i]);
	}
	//printf("\n");

	//RECIVE ENTITIES
	for(int i = 0; i < 6; i++){
		int count = 0;
		if(targets[i] == 1)printf("R %d: COUNT RECV %d\n",mmi->rank, count);
		if(targets[i] >= mmi->size || targets[i] < 0 || targets[i] == mmi->rank) continue;
		MPI_Recv(&count, 1, MPI_INT, targets[i], ENTITYTAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		if(targets[i] == 1)printf("R 1: COUNT RECV %d\n", count);
		EssentialEntity * entityarr = malloc((count+1) * sizeof(EssentialEntity));
		MPI_Recv(entityarr, count, MPI_Entity, targets[i], ENTITYTAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for(int b = 0; b < count; b++){
			entity* e = calloc(1, sizeof(*e));
				e->id = entityarr[b].id;
				e->type = entityarr[b].type;
				e->listpos = entityarr[b].listpos;
				e->path_position = 0;
				e->path = NULL;
				e->amountItems = entityarr[b].amountItems;
				e->position = entityarr[b].position;
				e->memory_dest.x = -1;
				vector3 * list = malloc(sizeof(vector3) * e->amountItems);
				memcpy(list, entityarr[b].list ,sizeof(vector3) * e->amountItems);
				e->list = list;
			queue_enqueue(pathf_queue, e);
			mmi->spawn_count++;
		}
		free(entityarr);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	/*if(mmi->rank == 1) {
		printf("ENQUEUE c %d \n", mmi->spawn_count);
		printf("EC %d \n", mmi->entity_count);
	}*/

        for(int i = 0; i < 6; i++){
                free(send_buffers[i]);
                free(send_entities[i]);
        }
        free(send_buffers);
	free(send_entities);
}

/**
 * Generates a random shopping list for each customer-entity
 */
vector3* generate_list(meta* const mmi, queue_t* empty_shelfs, int* items,
		EntityType Type) {
	*items = (Type == CUSTOMER) ? rand() % LISTL + 5 : LISTL;
	vector3* list = malloc(sizeof(vector3) * (*items));
	int shelf_count = mmi->shelf_count;
	for (int i = 0; i < *items; i++) {
		if (Type == CUSTOMER) {
			vector3 v;
			if (i == *items - 2) {
				//Kasse
				v = get_close_vector3(mmi->register_fields, mmi->register_count,
						list[i - 1], FALSE);
			} else if (i == *items - 1) {
				//Exit
				v = get_close_vector3(mmi->exit_fields, mmi->exit_count,
						list[i - 1], FALSE);
			} else {
				int r = abs(rand() % shelf_count);
				v = mmi->shelf_fields[r];
			}
			list[i] = v;
		} else {
			vector3* v;
			if (i == *items - 1) {
				//Exit
				vector3 h = get_close_vector3(mmi->stock_fields,
						mmi->stock_count, list[i - 1], FALSE);
				v = &h;
			} else {
				v = queue_dequeue(empty_shelfs);
			}
			list[i] = *v;
			if (i < *items - 1) {
				free(v);
				mmi->empty_count--;
			}
		}
	}
	return list;
}

/*Spawn an entity and enqueue it
 * @param entity_queue  : queue of all entities
 * @param position      : position where the entity is spawned
 * @param type          : type of the entity
 */
void spawn_entity(meta* const mmi, queue_t* const entity_queue, queue_t* const empty_shelfs, entity* e, EntityType type){
        static int counter = 0;
//        vector3 * list= malloc(sizeof(vector3));
//      list[1] = {4,4,4};
        queue_enqueue(entity_queue, e);
        //Should be done with a Mutex..
        counter++;
	mmi->spawn_count++;
//        mmi->entity_count = counter;
}

entity* make_worker(int id, vector3* empty_shelf) {
        entity* e = calloc(1, sizeof(*e));
                e->id = id;
                e->type = EMPLOYEE;
                e->listpos = 0;
                e->path_position = 0;
                e->amountItems = 1;
                //will be changed by the spawning process
                e->position= (vector3) {0,0,0};
                e->memory_dest.x = -1;
                //exit has to be concatenated
                e->list = empty_shelf;
        return e;
}

/* Reads the first to integers of an input file to determine the number of entities and the max size of their shopping list
 * @param *fh           : file pointer
 */
int readInit(MPI_File *fh) {
        MPI_File_read_all(*fh, &items, 1, MPI_INT, MPI_STATUS_IGNORE);
        MPI_File_read_all(*fh, &ecount, 1, MPI_INT, MPI_STATUS_IGNORE);
        //Change endian if necessary
        #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        items = __builtin_bswap32(items);
        ecount = __builtin_bswap32(ecount);
        #endif
        readbuf = calloc(items*3+1, sizeof(int));
	return ecount;
}


/* Reads a single entity with id and shoppingn list from a file into a buffer
 * @param *fh           : file pointer
 * @param entid         : the id of the entity to read [0, ecount]
 */
entity* readEntity(meta* const mmi, MPI_File *fh, int entid) {
        int offset = (entid * (items*3+1) +2) * 4;
        MPI_File_read_at(*fh, offset, readbuf, items*3+1, MPI_INT, MPI_STATUS_IGNORE);

        //Generate list
        vector3* list = calloc(3*items, sizeof(vector3));
        int o = 0;

        for(int i = 0; i < items*3; i++) {
        #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        //readbuf[i+1] = __builtin_bswap32(readbuf[i+1]);
        //readbuf[i+2] = __builtin_bswap32(readbuf[i+2]);
        //readbuf[i+3] = __builtin_bswap32(readbuf[i+3]);
        list[o] = (vector3) {__builtin_bswap32(readbuf[i+1]), __builtin_bswap32(readbuf[i+2]), __builtin_bswap32(readbuf[i+3])};
        #elif
                list[o] = (vector3) {readbuf[i+1], readbuf[i+2], readbuf[i+3]};
        #endif
                o++;
        }
/*        printf("-------------------------\n");
for(int i = 0; i < items*3; i+=3) {
    printf("%d,%d,%d\n", list[i].x, list[i].y, list[i].z);
}*/

        entity* e = calloc(1, sizeof(*e));
                e->id = readbuf[0];
                e->type = CUSTOMER;
                e->listpos = 0;
                e->path_position = 0;
                e->amountItems = items;
                e->position = mmi->exit_fields[rand() % mmi->exit_count];
                e->memory_dest.x = -1;

	int addedfields = 1;
	vector3* resolvedlist = calloc(3*items*3+3, sizeof(vector3));
        resolvedlist[0] = e->position;
	//check if first item and entry are in same story
	if(e->position.z != list[0].z) {
		resolvedlist[addedfields] = mmi->lift_fields[rand() % mmi->lift_count];
		resolvedlist[addedfields].z = e->position.z;
		addedfields++;
                resolvedlist[addedfields] = resolvedlist[addedfields-1];
                resolvedlist[addedfields].z = list[0].z;
                addedfields++;
                resolvedlist[addedfields] = list[0];
                addedfields++;
	}
	//Resolve story changes
        int c = 1;
        for(c = 1;c < items; c++) {
            if(list[c*3].x == 0 && list[c*3].y == 0 && list[c*3].z == 0) break;
            if(list[c*3-1].z == list[c*3].z) {                  //fields are on same level
                resolvedlist[addedfields] = list[c*3];
                addedfields++;
            } else {                                        //levelchange needed, get to next elevator
                resolvedlist[addedfields] = mmi->lift_fields[rand() % mmi->lift_count];
                resolvedlist[addedfields].z = resolvedlist[addedfields-1].z;
                addedfields++;
                resolvedlist[addedfields] = resolvedlist[addedfields-1];
                resolvedlist[addedfields].z = list[c*3].z;
                addedfields++;
                resolvedlist[addedfields] = list[c*3];
                addedfields++;
            }
        }
        //Get back to exit (entrance == exit)
        if(list[c*3].z != e->position.z) {
            resolvedlist[addedfields] = mmi->lift_fields[rand() % mmi->lift_count];
            resolvedlist[addedfields].z = list[c*3-1].z;
            addedfields++;
            resolvedlist[addedfields] = resolvedlist[addedfields-1];
            resolvedlist[addedfields].z = list[c*3].z;
            addedfields++;
            resolvedlist[addedfields] = list[c*3];
        } else {
            resolvedlist[addedfields] = list[c*3];
        }
        printf("-------------------------%i\n",addedfields);
for(int i = 0; i < addedfields; i++) {
    printf("%d,%d,%d\n", resolvedlist[i].x, resolvedlist[i].y, resolvedlist[i].z);
}/*
for(int i = 0; i < items*3; i+=3) {
    printf("%d,%d,%d\n", list[i].x, list[i].y, list[i].z);
}*/
        vector3* tmp;
	tmp = realloc(resolvedlist, addedfields);
        if(tmp == NULL) {
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
        resolvedlist = tmp;
        e->list = resolvedlist;
//	e->list = list;
	free(list);
        return e;
}

void pathWriterInit(MPI_File outputfh) {
	output = outputfh;
}

void writePath(ASPath path, entity* e) {
	int size = ASPathGetCount(path)*3*sizeof(int)+sizeof(int)*3+1;
	int* writecontainer = malloc(size);
	writecontainer[0] = size;
	writecontainer[1] = e->id;
	writecontainer[2] = e->listpos;
	writecontainer[3] = e->path_position;
	for(int i = 0; i > ASPathGetCount(path); i++) {
		vector3* field = ASPathGetNode(path, i);
		writecontainer[i*3+4] = field->x;
		writecontainer[i*3+5] = field->y;
		writecontainer[i*3+6] = field->z;
	}
	printf("--------------------------------------%i",size);
	MPI_File_write_shared(output, writecontainer, size, MPI_INT, MPI_STATUS_IGNORE);
//	MPI_File_write_shared(output, &e->id, 1, MPI_INT, MPI_STATUS_IGNORE);
}

//Warning, legacy code!

/*Spawn an entity and enqueue it
 * @param entity_queue 	: queue of all entities
 * @param position 	: position where the entity is spawned
 * @param type		: type of the entity
 */
void spawn_entity_random(meta* const mmi, queue_t* const entity_queue,
		queue_t* const empty_shelfs, EntityType type) {
	static int counter = 0;
	vector3 * list = malloc(2* sizeof(vector3));
	vector3 testdest = { 4, 4, 4 };
	vector3 testdest2 = { 17, 17, 2};
	list[0] = testdest2;
	list[1] = testdest;
	entity* e = calloc(1, sizeof(*e));
	e->id = 0;
	e->type = type;
	e->listpos = 0;
	e->path_position = 0;
	e->amountItems = 1;
	vector3 testpos = {24, 11, 1};
	e->position = testpos;
	e->memory_dest.x = -1;
	e->list = list;
	queue_enqueue(entity_queue, e);
	counter++;
	mmi->spawn_count++;
}
