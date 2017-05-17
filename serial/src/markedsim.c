#include "markedsim.h"
#include stdio.h


void createmarked(int x, int y, int floor_count){
	int *** marked = malloc(floor_count * sizeof(int**)); 
	for(int i = 0; i < floor_count; i++){
		int** floor = malloc(y * sizeof(int*));
		marked[i] = floor;
		for(int j = 0; j < y; j++){
			floor[j] = calloc(x * sizeof(int))
		}
		
	}
	
	
}

void freemarked(int** marked){
	for(int i = 0; i < (sizeof(marked)/sizeof(int**)); i+++ ){
	
	
	}

}


int
main(){
	
createmarked();	
	
freemarked();
	
}