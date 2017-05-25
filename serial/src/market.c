#include "market.h"

/* Test if Vectors are equal
 * @param vec1, vec2 address of the vectors
 * @return Boolean
 */
int vec_equal(vector3 * vec1, vector3 * vec2){
	return (vec1->x == vec2->x) && (vec1->y == vec2->y) && (vec1->z == vec2->z);
}

/*Gives back the field pointer in the field Matrix
 * described by the vector position
 * @param market 	: field matrix
 * @param vec 		: position of the field in the Matrix
 * @return 			: Field Pointer
 */
field* in_matrix(field*** const market, vector3 vec){
	return &market[vec.z][vec.y][vec.x];
}

/*Test if a field is blocked by something
 * @param market 	: field matrix
 * @param vec 		: position of the field in the Matrix
 * @return 			: Boolean
 */
int is_blocked(field*** const market, vector3 vec){
	if(market[vec.z][vec.y][vec.x].type == BLOCKVAL){
		//!= WALKABLE
		return TRUE;
	}
	return FALSE;
}

/*Initializes an 3d field array
 * TODO: Arrange values to one Memory Block
 * 	 field array only for access
 * @param x, y, floor_count : length, width and hight of the market
 * @return 			: pointer of allocated field array
 */
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

/*Import market data form textfile/csv,
 * @param path		: path to text file
 * @param x, y, floor_count : addresses for output of length,
 * 		  width and hight of the market
 * @return 			: pointer of the filled array
 */
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

/*Deallocate 3d field array
 * TODO: needs to be extended when create_market is optimized
 * @param market	: address of 3d field array
 * @param y, floor_count	: width, hight of the market
 */
void free_market(field*** const market, int y, int floor_count){
	for(int o = 0; o < floor_count; o++ ){
		for(int i = 0; i < y; i++){
			free(market[o][i]);
		}
		free(market[o]);
	}
	free(market);
}
