#include "markedsim.h"

field*** 
createmarked(int x, int y, int floor_count){
	field *** marked = malloc(floor_count * sizeof(field**)); 
	for(int i = 0; i < floor_count; i++){
		field** floor = malloc(y * sizeof(field*));
		marked[i] = floor;
		for(int j = 0; j < y; j++){
			floor[j] = malloc(x * sizeof(field));
		}
	}
	return marked;
}

field*** 
import_marked(char* path, int* x, int* y, int* floor_count){
	FILE *file = fopen(path, "r");
	if (file == NULL) exit(EXIT_FAILURE);
	
	fscanf(file, "%d,%d,%d\n", x, y, floor_count);
	field*** marked = createmarked(*x, *y, *floor_count);
	for(int a = 0; a < (*floor_count); a++){
		for(int b = 0; b < (*y); b++){
			for(int c = 0; c < (*x); c++){			
				fscanf(file, "%d,%d,%d\n",
					&marked[a][b][c].type,
					&marked[a][b][c].content,
					&marked[a][b][c].amount);
			}
		}
	}
	fclose(file);
return marked;
}

void 
freemarked(field*** marked, int y, int floor_count){
	for(int o = 0; o < floor_count; o++ ){
		for(int i = 0; i < y; i++){
			free(marked[o][i]);
		}
		free(marked[o]);
	}
	free(marked);
}

void
work_queue(queue_t* queue){
	
	
}

void
spawn_entity(queue_t* ){
	
}


int
main(int argc, char *argv[]){
if (argc < 2) exit(EXIT_FAILURE);

int x, y, floor_count;	
field*** marked;
queue_t *queue = queue_new();

marked = import_marked(argv[1], &x, &y, &floor_count);
printf("x: %d\n", x);
printf("y: %d\n", y);
printf("floor_count: %d\n", floor_count);

freemarked(marked, y, floor_count);
}
