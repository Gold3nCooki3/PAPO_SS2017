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
	if (from->z != to->z)
		exit(EXIT_FAILURE);
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
	exit(5);
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
		exit(EXIT_FAILURE);
	}
	return dest;
}

ASPath generate_localpath(vector3 start, vector3 dest, meta* const mmi,
		int* lift_flag) {
	PathNode pathFrom = (PathNode) start;
	vector3 pathTo_v = dest;
	printf("PF_ Hey\n");
	printf("PF_ Start: %d, %d \n", pathTo_v.z, start.z);

	if (pathTo_v.z != start.z) {
		if (mmi->lift_count > 0) {
			printf("PF_ searching for lift \n");
			pathTo_v = get_close_vector3(mmi->lift_fields, mmi->lift_count,
					start, TRUE);
		} else {
			*lift_flag = TRUE;
			return NULL;
		}
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
		return NULL;
	}

	PathNode pathTo = (PathNode) pathTo_v;
	ASPath path = ASPathCreate(&PathNodeSource, NULL, &pathFrom, &pathTo);
	if (ASPathGetCount(path) == 0) { //not vaild
		return NULL;
	}
	return path;
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

void generate_paths(queue_t* const queue, meta* const mmi) {
	struct queue_node_s *node = queue->front;
	int rightcount = 0, leftcount = 0, rank = mmi->rank, size = mmi->size;
	MPI_Request req;

	PE* local_r = malloc(mmi->spawn_count * sizeof(PE));
	PE* local_l = malloc(mmi->spawn_count * sizeof(PE));

	MPI_Datatype MPI_PathE;
	MPI_Type_contiguous(8, MPI_INT, &MPI_PathE);
	MPI_Type_commit(&MPI_PathE);

	while (node != NULL) {
		entity* e = node->data;
		int lift_flag = FALSE;
		ASPath tempPath = generate_localpath(e->position, e->list[e->listpos],
				mmi, &lift_flag);
		if (ASPathGetCount(tempPath) > 0) {
			e->path = tempPath;
		} else {
			start_vec = e->position;
			int result = -1;

			qsort(mmi->edge_fields, mmi->edge_count, sizeof(vector3), compfunc);
			for (int c = 0; c < mmi->edge_count; c++)
				printf(" [%2d, %2d, %2d];", mmi->edge_fields[c].x,
						mmi->edge_fields[c].y, mmi->edge_fields[c].z);
			printf("\n");
			force_stop();
			for (int c = 0; c < mmi->edge_count; c++) {
				ASPath path = ASPathCreate(&PathNodeSource, NULL, &start_vec,
						&(mmi->edge_fields[c]));
				if (ASPathGetCount(path) > 0) { //is path vaild
					result = c;
					e->path = path;
					break;
				}
				if (c == mmi->edge_count - 1) {
					printf("Error");
				}
			}

			PE p = { e->id, result, mmi->edge_fields[result],
					e->list[e->listpos] };
			if (TRUE) {
				local_r[rightcount++] = p;
			} else {
				local_l[leftcount++] = p;
			}
		}
		node = node->next;
	}
	rightcount++;
	leftcount++;

	/*int t= 0;*/
	while (FALSE) {

		if (rank != size - 1) {
			MPI_Isend(&leftcount, 1, MPI_INT, rank + 1, PATHTAG, MPI_COMM_WORLD, &req);
			MPI_Isend(local_l, leftcount, MPI_PathE, rank + 1, PATHTAG,
					MPI_COMM_WORLD, &req);
		}
		if (rank != 0) {
			MPI_Isend(&rightcount, 1, MPI_INT, rank - 1, PATHTAG, MPI_COMM_WORLD, &req);
			MPI_Isend(local_r, rightcount, MPI_PathE, rank - 1, PATHTAG,
					MPI_COMM_WORLD, &req);
		}

		int temp_rc, temp_lc;
		if (rank != 0) { /* other fields*/
			MPI_Recv(&temp_rc, 1, MPI_INT, rank - 1, PATHTAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			PE * other_r = calloc(temp_rc, sizeof(PE));
			MPI_Recv(other_r, temp_rc, MPI_INT, rank - 1, PATHTAG,
					MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		if (rank != size - 1) {
			MPI_Recv(&temp_lc, 1, MPI_INT, rank + 1, PATHTAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			PE * other_l = calloc(temp_lc, sizeof(PE));
			MPI_Recv(other_l, temp_lc, MPI_INT, rank + 1, PATHTAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		/*find path for left and right*/
		/*switch start = dest, find new dest*/

		for (int i = 0; i < temp_rc; i++) {

		}
		for (int i = 0; i < temp_lc; i++) {

		}

	}
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
		printf("in here");
		field* f;
		vector3* temp_vec;
		switch (in_matrix_g(e->memory_dest)->type) {
		case STOCK:
		case EXIT:
			ASPathDestroy(e->path);
			return DESTROY;
			break;
		case ESCALATOR:
		case LIFT:
			printf("PF_ Takeing the lift");
			if (e->position.z > e->list[e->listpos].z) {
				e->position.z -= 1;
				if (!in_process(e->position))
					return_val = DOWN;
			} else {
				e->position.z += 1;
				if (!in_process(e->position))
					return_val = UP;
			}
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
			}
		default:
			break;
		}
		ASPathDestroy(e->path);
		e->path_position = 0;
		return_val = NEWPATH;
	} else if (e->listpos >= e->amountItems) { //
		printf("Error: no exit");
		exit(EXIT_FAILURE);
	}
	return return_val;
}

void fast_realloc(entity * ptr, int * count, int * max, int size){
			if(count == max-1){
					*max += size;
					ptr = realloc(ptr, *max * sizeof(entity));
			}
}

void find_second_limits(meta* const mmi, int * second_ulimit, int * second_llimit, int * upper_rank, int * lowerrank) { //BruteForce
	int result, temp_limit, limit, helpval;

	if(mmi->rank > mmi->size-1){
	helpval = (mmi->startcolumn + ((mmi->chunksize - 1) / mmi->rows) - 1
			+ (mmi->startstorey + 1) * mmi->columns);
	for (int r = mmi->rank + 1; r < mmi->size; r++) {
		result = r;
		temp_limit = mmi->linecount / mmi->size
				* r + MIN(r,mmi->linecount%mmi->size);
		if (helpval < temp_limit){
			break;
		}else{
			limit = temp_limit;
		}
	}
	}
	*upper_rank = result - 1;
	*second_ulimit = limit;

	if(mmi->rank > 0){
	helpval = (mmi->startcolumn + (mmi->startstorey - 1) * mmi->columns);
	for (int r = mmi->rank - 1; r >= 0; r--) {
		result = r;
		temp_limit = mmi->linecount / mmi->size
				* r+ MIN(r,mmi->linecount%mmi->size);
		if (helpval > temp_limit){
			break;
		}else{
			limit = temp_limit;
		}
	}
	}
	*lowerrank = result + 1;
	*second_llimit = temp_limit;
}

/*Move every entity in the entity_queue, dequeue if entity reached final destination
 * @param market 	: fields where entities move within
 * @param entity_queue 	: queue of all entities
 */
void work_queue(meta * const mmi, queue_t* const entity_queue,
		queue_t* const empty_shelfs, queue_t* const pathf_queue) {
	MPI_Request req;
	if (!queue_empty(pathf_queue)) {
		generate_paths(pathf_queue, mmi);
		while (!queue_empty(pathf_queue)) {
			entity* e = queue_dequeue(pathf_queue);
			queue_enqueue(entity_queue, e);
			mmi->entity_count++;
		}
	} else {
		//printf("Nothing to spawn");
	}

	if (queue_empty(entity_queue)) {
		printf("empty!");
	} else {

		/*=============================*/
		/*s_L rank-1, s_R rank+1 , s_ol & s_oR depends on market separation */
		entity*** send_entities = calloc(6, sizeof(entity*));
		for(int i = 0; i < 6; i++){
			send_entities[i] = calloc(10, sizeof(entity*));
		}
		int lowerlimit = mmi->linecount / mmi->size * mmi->rank-1 + MIN(mmi->rank-1,mmi->linecount%mmi->size);
		int upperlimit = mmi->linecount / mmi->size * mmi->rank+1 + MIN(mmi->rank+1,mmi->linecount%mmi->size);
		int second_ulimit, second_llimit, upper_rank, lower_rank;
		find_second_limits(mmi, &second_ulimit, &second_llimit, &upper_rank, &lower_rank);

		int targets[6] = { mmi->rank-1, mmi->rank+1, lower_rank+1, upper_rank-1, lower_rank, upper_rank };
		int maxima[6] = { 10, 10, 10, 10, 10, 10 };
		int s_counts[6] = { 0, 0, 0, 0, 0, 0 };

		/*=============================*/
		entity* first = queue_dequeue(entity_queue);
		entity* e = first;
		do {
			if (!first)
				first = e;
			EnS status = move_entity(mmi, empty_shelfs, e);
			printf("Status: %d \n", status);
			switch (status) {
				case ENQUEUE: queue_enqueue(entity_queue, e); break;
				case NEWPATH: queue_enqueue(pathf_queue, e);break;
				case DESTROY:
					free(e->list);
					if (e == first)
						first = NULL;
					free(e);
					break;
				case DOWN: if((e->position.y + e->position.z * mmi->columns) < lowerlimit){
								if((e->position.y + e->position.z * mmi->columns) < second_llimit){
									fast_realloc(*(send_entities[4]), &s_counts[4], &maxima[4], 10); //ooL
									send_entities[4][s_counts[4]++] = e;
								}else{
									fast_realloc(*(send_entities[2]), &s_counts[2], &maxima[2], 10); //oL
									send_entities[2][s_counts[2]++] = e;
								}
							}
				case EDGEL: fast_realloc(*(send_entities[0]), &s_counts[0], &maxima[0], 20);
							send_entities[0][s_counts[0]++] = e;	break;

				case UP: if((e->position.y + e->position.z * mmi->columns) > upperlimit){
								if((e->position.y + e->position.z * mmi->columns) > second_ulimit){ //ooR
									fast_realloc(*(send_entities[5]), &s_counts[5], &maxima[5], 10);
									send_entities[5][s_counts[5]++] = e;
								}else{
									fast_realloc(*(send_entities[3]), &s_counts[3], &maxima[3], 10);
									send_entities[3][s_counts[5]++] = e;
								}
							}
				case EDGER: fast_realloc(*(send_entities[1]), &s_counts[1], &maxima[1], 20);
							send_entities[1][s_counts[1]++] = e;	break;
				default: break;
			}
			if (status == DESTROY && queue_empty(entity_queue))
				break;
			printf(" 2 - ");
			e = queue_dequeue(entity_queue);
		} while (first != e);
		if (first == e)
			queue_enqueue(entity_queue, e); // otherwise one element gets lost


		/*=============================*/

		MPI_Datatype MPI_Entity;
		MPI_Type_contiguous(10 + 3*LISTL, MPI_INT, &MPI_Entity);
		MPI_Type_commit(&MPI_Entity);


		//SEND ENTITIES
		for(int i = 0; i < 6; i++){
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
			MPI_Isend(&s_counts[i], 1, MPI_INT, targets[i], ENTITYTAG, MPI_COMM_WORLD, &req);
			MPI_Isend(entities_tosend, s_counts[i],  MPI_Entity, targets[i], ENTITYTAG, MPI_COMM_WORLD, &req);
			free(entities_tosend);
		}

		//RECIVE ENTITIES
		for(int i = 0; i < 6; i++){
			int count;
			MPI_Recv(&count, 1, MPI_INT, targets[i], ENTITYTAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			EssentialEntity * entityarr = malloc(count * sizeof(EssentialEntity));
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
				queue_enqueue(pathf_queue, &entityarr[b]);
			}
			free(entityarr);
		}

	}
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
 * @param entity_queue 	: queue of all entities
 * @param position 	: position where the entity is spawned
 * @param type		: type of the entity
 */
void spawn_entity(meta* const mmi, queue_t* const entity_queue,
		queue_t* const empty_shelfs, EntityType type) {
	static int counter = 0;
	vector3 * list = malloc(sizeof(vector3));
	vector3 testdest = { 4, 4, 4 };
	list[0] = testdest;
	entity* e = calloc(1, sizeof(*e));
	e->id = 0;
	e->type = type;
	e->listpos = 0;
	e->path_position = 0;
	e->amountItems = 1;
	vector3 testpos = mmi->exit_fields[rand() % mmi->exit_count];
	e->position = testpos;
	e->memory_dest.x = -1;
	e->list = list;
	queue_enqueue(entity_queue, e);
	counter++;
	mmi->spawn_count = counter;
}
