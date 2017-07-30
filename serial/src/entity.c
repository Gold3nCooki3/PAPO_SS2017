#include "entity.h"

typedef struct vector3 PathNode;

/*Clamps a value to -1, 0, 1
 * @param d 	: input value
 * @return 		: -1, 0, 1
 */
int clamp(int d) {
  if(d >= 1) return 1;
  if(d <= -1) return -1;
  return 0;
}

vector3 get_close_vector3(vector3* const list, int listlength, vector3 start, int forceSameLevel){
	vector3 dest;
	int mindistance = -1;
	int distance;
	for(int i = 0; i < listlength; i++){
		/*TODO: 10 * delta(z) nicht das beste*/
		distance = abs(list[i].x - start.x)  + abs(list[i].y - start.y) + 10*abs(list[i].z - start.z);
		if(!forceSameLevel && (distance > mindistance || mindistance == -1)) {
			mindistance = distance;
			dest = list[i];
		}else if(forceSameLevel && list[i].z == start.z && (distance > mindistance || mindistance == -1)){
			mindistance = distance;
			dest = list[i];
		}
	}
	if(mindistance == -1){printf("error"); exit(EXIT_FAILURE);}
	return dest;
}


static void PathNodeNeighbors(ASNeighborList neighbors, void *node, void *context)
{

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

static float PathNodeHeuristic(void *fromNode, void *toNode, void *context)
{
    PathNode *from = (PathNode *)fromNode;
    PathNode *to = (PathNode *)toNode;
    if(from->z != to->z) exit(EXIT_FAILURE);
    // using the manhatten distance since this is a simple grid and you can only move in 4 directions
    return (fabs(from->x - to->x) + fabs(from->y - to->y));
}

static const ASPathNodeSource PathNodeSource = {
    sizeof(PathNode),
    &PathNodeNeighbors,
    &PathNodeHeuristic,
    NULL,
    NULL
};

int has_path(entity* const e){
	if(ASPathGetCount(e->path) > 0){
		return TRUE;
	}else{
		return FALSE;
	}
}
/*Moves an entity towards their destination,
 * to pick up or fill up content from a frame
 * @param market	: market where entity is moving within
 * @param e			: entity
 * @return FALSE if person has reached their final destination
 */6
int move_entity(field*** const market, meta* const mmi,queue_t* const empty_shelfs, entity* const e){
	if (!has_path(e)){
		PathNode pathFrom = (PathNode)e->position;
		vector3 pathTo_v = e->list[e->listpos];

		if(pathTo_v.z != e->position.z){
			pathTo_v = get_close_vector3(mmi->lift_fields, mmi->lift_count, e->position,1);
		}else{

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
								exit(EXIT_FAILURE);
							}
						}
					}
				}break;
			default: break;
		}

		}

		PathNode pathTo = (PathNode)pathTo_v;
		e->path = ASPathCreate(&PathNodeSource, NULL, &pathFrom, &pathTo);
		e->memory_dest = pathTo_v;
		if(ASPathGetCount(e->path) == 0){ //not vaild
			printf("No path");
			exit(EXIT_FAILURE);
		}
	}

	//Get to new Position
	PathNode* new_pos = ASPathGetNode(e->path, e->path_position);
	e->position = *new_pos;
	e->path_position++;

	// exit
	if(ASPathGetCount(e->path) == e->path_position+1){
		field* f;
		switch (in_matrix_g(e->memory_dest)->type){
			case STOCK:
			case EXIT: return FALSE; break;
			case ESCALATOR:
			case LIFT: e->position.z++; break;
			case CORRIDOR:
				f = in_matrix_g(e->list[e->listpos]);
				if(f->amount > 0 && e->type == CUSTOMER) f -= 1;
				if(e->type == EMPLOYEE) f += FILLVAL;
			default: break;
		}
		ASPathDestroy(e->path);
		e->listpos++;
	}else if(e->listpos == e->amountItems -1){ //
		printf("Error: no exit");
		exit(EXIT_FAILURE);
	}
	return TRUE;
}

/*Move every entity in the queue, dequeue if entity reached final destination
 * @param market 	: fields where entities move within
 * @param queue 	: queue of all entities
 */
void work_queue(field*** const market, meta * const mmi, queue_t* const queue, queue_t* const empty_shelfs){
	if(queue_empty(queue)){
		printf("empty!");
		return;
	}else{
		entity* first = queue_dequeue(queue);
		entity* e = first;
		do{
			if(!first)first = e;
			int not_finished = move_entity(market, mmi, empty_shelfs, e);
			if(not_finished){
				queue_enqueue(queue, e);
			}else {
				free(e->list);
				if(e == first) first = NULL;
				if(queue_empty(queue)) break;
			}
			e = queue_dequeue(queue);
		}while(first != e);
		if(first == e) queue_enqueue(queue, e); // otherwise one element gets lost
	}
}

/**
 * Generates a random shopping list for each customer-entity
 */
vector3* generate_list(meta* const mmi, queue_t* empty_shelfs, int* items, EntityType Type) {
	*items = (Type == CUSTOMER) ? rand()%LISTL+5 : LISTL+2;
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
			mmi->emtpy_count--;
			list[i] = *v;
		}
	}
printf("made list\n");
	return list;
}

/*Spawn an entity and enqueue it
 * @param queue 	: queue of all entities
 * @param position 	: position where the entity is spawned
 * @param type		: type of the entity
 */
void spawn_entity(meta* const mmi, queue_t* const queue, queue_t* const empty_shelfs, EntityType type){
	static int counter = 0;
	int items = 0;
	vector3* list;
	list= generate_list(mmi, empty_shelfs, &items, type);
	vector3 position = (type == CUSTOMER) ? mmi->exit_fields[abs(rand()%mmi->exit_count)] : mmi->stock_fields[abs(rand()%mmi->stock_count)];
	entity* e = calloc(1, sizeof(*e));
		e->id = counter;
		e->type = type;
		e->listpos = 0;
		e->path_position = 0;
		e->amountItems = items;
		e->position= position;
		e->memory_dest.x = -1;
		e->list = list;
	queue_enqueue(queue, e);
	counter++;
}
