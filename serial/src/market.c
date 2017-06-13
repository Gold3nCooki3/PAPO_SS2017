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
	if(in_matrix(market, vec)->type == BLOCKVAL){
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
field*** import_market(char* path, meta *mmi){

	FILE *file = fopen(path, "r");
	if (file == NULL){
		exit(EXIT_FAILURE);
	}

	fscanf(file, "%d,%d,%d,%d,%d,%d,%d,%d\n", &mmi->rows, &mmi->columns, &mmi->stories,
			&mmi->shelf_count, &mmi->lift_count, &mmi->register_count, &mmi->stock_count, &mmi->exit_count);

	//Allocate pointers to specific field types
	vector3* shelves = malloc(sizeof(vector3)*mmi->shelf_count);
	vector3* lifts = malloc(sizeof(vector3)*mmi->lift_count);
	vector3* stocks = malloc(sizeof(vector3)*mmi->stock_count);
	vector3* registers = malloc(sizeof(vector3)*mmi->register_count);
	vector3* exits = malloc(sizeof(vector3)*mmi->exit_count);

	int q=0, w=0, e=0, r=0, t = 0;

	field*** market = create_market(mmi->rows, mmi->columns, mmi->stories);
	for(int a = 0; a < mmi->stories; a++){
		for(int b = 0; b < mmi->columns; b++){
			for(int c = 0; c < mmi->rows; c++){
				fscanf(file, "%d,%d,%d\n",
					&market[a][b][c].type,
					&market[a][b][c].content,
					&market[a][b][c].amount);
				vector3 v = {c,b,a};
				//printf("(%d, %d, %d)\n",c,b,a);
				switch(market[a][b][c].type) {
				case SHELF:
					shelves[q++] = v;
					break;
				case LIFT:
				case ESCALATOR:
					lifts[w++] = v;
					break;
				case REGISTER:
					registers[e++] = v;
					break;
				case STOCK:
					stocks[r++] = v;
					break;
				case EXIT:
					exits[t++] = v;
					break;
				default:
					break;
				}
			}
		}
	}
	fclose(file);

	mmi->shelf_fields = shelves;
	mmi->lift_fields = lifts;
	mmi->stock_fields = stocks;
	mmi->register_fields = registers;
	mmi->exit_fields = exits;

return market;
}

/*Deallocate 3d field array
 * TODO: needs to be extended when create_market is optimized
 * @param market	: address of 3d field array
 * @param y, floor_count	: width, hight of the market
 */
void free_market(field*** market, int y, int floor_count){
	for(int o = 0; o < floor_count; o++ ){
		for(int i = 0; i < y; i++){
			free(market[o][i]);
		}
		free(market[o]);
	}
	free(market);
}

void free_meta(meta* mmi){
	free(mmi->shelf_fields);
	free(mmi->lift_fields);
	free(mmi->stock_fields);
	free(mmi->register_fields);
	free(mmi->exit_fields);
}
