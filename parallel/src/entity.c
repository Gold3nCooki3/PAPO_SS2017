#include "entity.h"

/*============================================HELP FUNCTIONS PATHFINDING======================================*/
typedef struct vector3 PathNode;

static void PathNodeNeighbors(ASNeighborList neighbors, void *node, void *context){

	PathNode *pathNode = (PathNode *)node;

	if (!is_blocked((PathNode){pathNode->x+1, pathNode->y, pathNode->z})){
		ASNeighborListAdd(neighbors, &(PathNode){pathNode->x+1, pathNode->y,pathNode->z}, 1);
	}
	if (!is_blocked((PathNode){pathNode->x-1, pathNode->y, pathNode->z})){
		ASNeighborListAdd(neighbors, &(PathNode){pathNode->x-1, pathNode->y,pathNode->z}, 1);
	 }
	if (!is_blocked((PathNode){pathNode->x, pathNode->y+1, pathNode->z})){
		ASNeighborListAdd(neighbors, &(PathNode){pathNode->x, pathNode->y+1,pathNode->z}, 1);
	}
	if (!is_blocked((PathNode){pathNode->x, pathNode->y-1, pathNode->z})){
		ASNeighborListAdd(neighbors, &(PathNode){pathNode->x, pathNode->y-1,pathNode->z}, 1);
	}
}

static float PathNodeHeuristic(void *fromNode, void *toNode, void *context){
	PathNode *from = (PathNode *)fromNode;
    PathNode *to = (PathNode *)toNode;
    if(from->z != to->z) exit(EXIT_FAILURE);
    return (fabs(from->x - to->x) + fabs(from->y - to->y));
}

int has_path(entity* const e){
	if(e->path_position > 0 && ASPathGetCount(e->path) > 0){
		return TRUE;
	}else{
		return FALSE;
	}
}

static const ASPathNodeSource PathNodeSource = {
    sizeof(PathNode),
    &PathNodeNeighbors,
    &PathNodeHeuristic,
    NULL,
    NULL
};
/*===================================================================================================*/


vector3 get_close_vector3(vector3* const list, int listlength, vector3 start, int forceSameLevel){
	vector3 dest;
	int mindistance = -1;
	int distance;
	for(int i = 0; i < listlength; i++){
		distance = abs(list[i].x - start.x)  + abs(list[i].y - start.y) + 10 + abs(list[i].z - start.z);
		if((!forceSameLevel || list[i].z == start.z) && (distance < mindistance || mindistance == -1)) {
			mindistance = distance;
			dest = list[i];
		}
	}
	if(mindistance == -1){printf("error"); exit(EXIT_FAILURE);}
	return dest;
}

void generate_paths(queue_t* const queue, meta* const mmi){
	struct queue_node_s *node = queue->front;
	int rightcount = 0, leftcount = 0, rank = mmi->rank;
	PE* local_r = malloc(mmi->spawn_count * sizeof(PE));
	PE* local_l = malloc(mmi->spawn_count * sizeof(PE));

	MPI_Datatype MPI_PathE;
	MPI_Type_contiguous(8, MPI_INT, &MPI_PathE);
	MPI_Type_commit(&MPI_PathE);


	while(node != NULL){
		entity* e = node->data;
		ASPath tempPath = generate_localpath(e->position, e->list[e->listpos], mmi);
		if(!tempPath){
			e->path = tempPath;
		}else{
			vector3 start = e->position;
			vector3 dest, tempdest;

			int directioncolumn = (TRUE) ? mmi->startcolumn + mmi->linecount : mmi->startcolumn;

			for(int r = 0 ; r < mmi->rows; r++){ /* TODO EDGEFIELDS */
				int tempmin, min = -1;
				int d = abs(r - e->list[e->listpos].x) + abs(directioncolumn - e->list[e->listpos].y);
				if(d < min || min == -1){
					tempmin = d;
					tempdest.x = r;
					tempdest.y = directioncolumn;
					tempdest.z = e->position.z;
					if(!is_blocked(tempdest)){
						ASPath path = ASPathCreate(&PathNodeSource, NULL, &start, &tempdest);
						if(ASPathGetCount == 0) continue;
						dest = tempdest;
						min = tempmin;
					}
				}
			}

			PE p = {e->id, 0, dest, e->list[e->listpos]};
			if(TRUE){
				local_r[rightcount++] = p;
			}else{
				local_l[leftcount++] = p;
			}
		}
		node = node->next;
	}
	rightcount++;leftcount++;

	int t= 0;
	while(t++ < 1){

		if(mmi->rank != size-1){
			MPI_Isend(&leftcount, 1, MPI_INT, rank+1, PATHTAG, MPI_COMM_WORLD, &req);
			MPI_Isend(local_l, leftcount, MPI_PathE, rank+1, PATHTAG, MPI_COMM_WORLD, &req);
		}
		if(mmi->rank != 0){
			MPI_Isend(&rightcount, 1, MPI_INT, rank-1, PATHTAG, MPI_COMM_WORLD, &req);
			MPI_Isend(local_r, rightcount, MPI_PathE, rank-1,PATHTAG, MPI_COMM_WORLD, &req);
		}

		int temp_rc, temp_lc;
		if(mmi->rank != 0){ /* other fields*/
			MPI_Recv(&temp_rc, 1, MPI_INT, rank-1, PATHTAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE) ;
			PE * other_r = calloc(temp_rc, siezof(PE));
			MPI_Recv(other_r, temp_rc, MPI_INT, rank-1, PATHTAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE) ;
		}
		if(mmi->rank != size-1){
			MPI_Recv(&temp_lc, 1, MPI_INT, rank+1, PATHTAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE) ;
			PE * other_l = calloc(temp_lc, siezof(PE));
			MPI_Recv(other_l, temp_lc, MPI_INT, rank+1, PATHTAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE) ;
		}

		/*find path for left and right*/
		/*switch start = dest, find new dest*/

		for(int i = 0; i < temp_rc; i++){

		}
		for(int i = 0; i < temp_lc; i++){

		}



	}
}


void generate_localpath(vector3 start, vector3 dest, meta* const mmi){
	PathNode pathFrom = (PathNode)start;
	vector3 pathTo_v = dest;


	if(in_process(dest)){
		if(pathTo_v.z != start.z){
			if(mmi->lift_count > 0){
				pathTo_v = get_close_vector3(mmi->lift_fields, mmi->lift_count, e->position, TRUE);
			}else{
				return LIFT;
			}
		}else if(){
			switch (in_matrix_g(e->list[e->listpos])->type){
				case REGISTER:
				case SHELF: pathTo_v.x++;
					if(is_blocked(pathTo_v)){
						pathTo_v.x-=2;
						if(is_blocked(pathTo_v)){
							pathTo_v.x+=1;
							pathTo_v.y+=1;
							if(is_blocked(pathTo_v)){
								pathTo_v.y-=2;
								if(is_blocked(pathTo_v)){

								}
							}
						}
					}
					break;
				default: break;
			}
		}
	}else{
		return 0;
	}


	PathNode pathTo = (PathNode)pathTo_v;
	ASPath path = ASPathCreate(&PathNodeSource, NULL, &pathFrom, &pathTo);
	if(ASPathGetCount(e->path) == 0){ //not vaild
		return 0;
	}
	return path;
}

/*Moves an entity towards their destination,
 * to pick up or fill up content from a frame
 * @param market	: market where entity is moving within
 * @param e			: entity
 * @return FALSE if person has reached their final destination
 */
int move_entity(meta* const mmi,queue_t* const empty_shelfs, entity* const e){
	if (!has_path(e)){
		generate_localpath(e, mmi);
	}

	//Get to new Position
	PathNode* new_pos = ASPathGetNode(e->path, e->path_position);
	e->position = *new_pos;
	e->path_position++;

	// exit
	if(ASPathGetCount(e->path) == e->path_position){
		field* f;
		vector3* temp_vec;
		switch (in_matrix_g(e->memory_dest)->type){
			case STOCK:
			case EXIT:
				ASPathDestroy(e->path);
				return FALSE; break;
			case ESCALATOR:
			case LIFT: e->position.z = (e->position.z > e->list[e->listpos].z) ?  e->position.z-1 : e->position.z+1; break;
			case CORRIDOR:
				f = in_matrix_g(e->list[e->listpos]);
				if(e->type == CUSTOMER){
					if(f->amount > 0){
						f->amount -= 1;
					}else if(f->amount == 0){ //collect empty fields
						f->amount = -1;
						temp_vec = malloc(sizeof(vector3));
						memcpy(temp_vec, &e->list[e->listpos], sizeof(vector3));
						mmi->empty_count++;
						queue_enqueue(empty_shelfs, temp_vec);
					}
				}else if(e->type == EMPLOYEE){
					 f += FILLVAL;
				}
				e->listpos++;
			default: break;
		}
		ASPathDestroy(e->path);
		e->path_position = 0;
	}else if(e->listpos >= e->amountItems){ //
		printf("Error: no exit");
		exit(EXIT_FAILURE);
	}
	return TRUE;
}

/*Move every entity in the entity_queue, dequeue if entity reached final destination
 * @param market 	: fields where entities move within
 * @param entity_queue 	: queue of all entities
 */
void work_queue(meta * const mmi, queue_t* const entity_queue, queue_t* const empty_shelfs, queue_t* const pathf_queue){
	if(!queue_empty(pathf_queue)){
		generate_paths(pathf_queue, mmi);
	}else{
		printf("Nothing to spawn");
	}

	if(queue_empty(entity_queue)){
		printf("empty!");
		return;
	}else{
		entity* first = queue_dequeue(entity_queue);
		entity* e = first;
		do{
			if(!first)first = e;
			int not_finished = move_entity(mmi, empty_shelfs, e);
			if(not_finished){
				queue_enqueue(entity_queue, e);
			}else {
				free(e->list);
				if(e == first) first = NULL;
				free(e);
				if(queue_empty(entity_queue)) break;
			}
			e = queue_dequeue(entity_queue);
		}while(first != e);
		if(first == e) queue_enqueue(entity_queue, e); // otherwise one element gets lost
	}
}

/**
 * Generates a random shopping list for each customer-entity
 */
vector3* generate_list(meta* const mmi, queue_t* empty_shelfs, int* items, EntityType Type) {
	*items = (Type == CUSTOMER) ? rand()%LISTL+5 : LISTL;
	vector3* list = malloc(sizeof(vector3) * (*items));
	int shelf_count = mmi->shelf_count;
	for(int i = 0; i < *items; i++){
		if(Type == CUSTOMER){
			vector3 v;
			if(i == *items-2){
				//Kasse
				v = get_close_vector3(mmi->register_fields, mmi->register_count, list[i-1], FALSE);
			}else if(i == *items-1){
				//Exit
				v = get_close_vector3(mmi->exit_fields, mmi->exit_count, list[i-1], FALSE);
			}else{
				int r = abs(rand()%shelf_count);
				v = mmi->shelf_fields[r];
			}
			list[i] = v;
		}else{
			vector3* v;
			if(i == *items-1){
				//Exit
				vector3 h = get_close_vector3(mmi->stock_fields, mmi->stock_count, list[i-1], FALSE);
				v = &h;
			}else{
				v = queue_dequeue(empty_shelfs);
			}
			list[i] = *v;
			if(i < *items-1){
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
void spawn_entity(meta* const mmi, queue_t* const entity_queue, queue_t* const empty_shelfs, EntityType type){
	static int counter = 0;
	vector3 * list= malloc(sizeof(vector3));
	list[1] = {4,4,4};
	entity* e = calloc(1, sizeof(*e));
		e->id = 0;
		e->type = type;
		e->listpos = 0;
		e->path_position = 0;
		e->amountItems = items;
		e->position= {0,0,0};
		e->memory_dest.x = -1;
		e->list = list;
	queue_enqueue(entity_queue, e);
	counter++;
	mmi->spawn_count = counter;
}
