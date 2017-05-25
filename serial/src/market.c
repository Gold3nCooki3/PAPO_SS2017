#include "market.h"

field* in_matrix(field*** market, vector3 vec){
	return &market[vec.z][vec.y][vec.x];
}

int is_blocked(field*** market, vector3 vec){
	if(market[vec.z][vec.y][vec.x].type == BLOCKVAL){
		return TRUE;
	}
	return FALSE;
}

field*** create_market(int x, int y, int floor_count){
	field *** market = malloc(floor_count * sizeof(field**));
	for(int i = 0; i < floor_count; i++){
		field** floor = malloc(y * sizeof(field*));
		market[i] = floor;
		for(int j = 0; j < y; j++){
			floor[j] = malloc(x * sizeof(field));
		}
	}
	return market;
}

field*** import_market(char* path, int* x, int* y, int* floor_count){
	FILE *file = fopen(path, "r");
	if (file == NULL) exit(EXIT_FAILURE);

	fscanf(file, "%d,%d,%d\n", x, y, floor_count);
	field*** market = create_market(*x, *y, *floor_count);
	for(int a = 0; a < (*floor_count); a++){
		for(int b = 0; b < (*y); b++){
			for(int c = 0; c < (*x); c++){
				fscanf(file, "%d,%d,%d\n",
					&market[a][b][c].type,
					&market[a][b][c].content,
					&market[a][b][c].amount);
			}
		}
	}
	fclose(file);
return market;
}

void free_market(field*** market, int y, int floor_count){
	for(int o = 0; o < floor_count; o++ ){
		for(int i = 0; i < y; i++){
			free(market[o][i]);
		}
		free(market[o]);
	}
	free(market);
}
