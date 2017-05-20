#include "markedsim.h"

struct field*** createmarked(int x, int y, int floor_count){
	struct  field *** marked = malloc(floor_count * sizeof(struct field**)); 
	for(int i = 0; i < floor_count; i++){
		struct  field** floor = malloc(y * sizeof(struct field*));
		marked[i] = floor;
		for(int j = 0; j < y; j++){
			floor[j] = malloc(x * sizeof(struct field));
		}
	}
	return marked;
}

void freemarked(struct field*** marked, int y, int floor_count){
	for(int o = 0; o < floor_count; o++ ){
		for(int i = 0; i < y; i++){
			free(marked[o][i]);
		}
		free(marked[o]);
	}
	free(marked);
}


void import_marked(char* path){

}

int
main(){


struct field*** marked = createmarked(x, y, floor_count);







freemarked(marked, y, floor_count);
}
