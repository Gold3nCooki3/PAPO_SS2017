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

void freemarked(struct field*** marked){
	for(int o = 0; o < sizeof(marked); o++ ){
		for(int i = 0; i < sizeof(marked[o]); i++){
			free(marked[o][i]);
		}
		free(marked[o]);
	}
	free(marked);
}


int
main(){
	
struct field*** marked = createmarked(10, 10, 1);	
	
freemarked(marked);
	
}
