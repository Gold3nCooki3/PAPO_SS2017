#ifndef MARKET_H
#define MARKET_H

#define TRUE 1
#define FALSE 0

#include <stdlib.h>
#include <stdio.h>

typedef enum FieldType {ESCALATOR, BLOCKVAL, EXIT=7} FieldType;

/*Struct for one filed in the market
 * type		: frame, walk, stock, escalator etc
 * content	: content of the frame
 * amount	: count of content
 */
struct
field{
	FieldType type;
	int content;
	int amount;
};
typedef struct field field;

struct
vector3{
	int x;
	int y;
	int z;
};
typedef struct vector3 vector3;

/* Test if Vectors are equal
 * @param vec1, vec2 address of the vectors
 * @return Boolean
 */
int vec_equal(vector3 * vec1, vector3 * vec2);

/*Initializes an 3d field array
 * TODO: later generator function
 * field array only for access
 * @param x, y, floor_count : length, width and hight of the market
 * @return 			: pointer of allocated field array
 */
//field*** create_market(int x, int y, int floor_count);

/*Import market data form textfile/csv,
 * @param path		: path to text file
 * @param x, y, floor_count : addresses for output of length,
 * 		  width and hight of the market
 * @return 			: pointer of the filled array
 */
field*** import_market(char* path, int* x, int* y, int* floor_count);

/*Gives back the field pointer in the field Matrix
 * described by the vector position
 * @param market 	: field matrix
 * @param vec 		: position of the field in the Matrix
 * @return 			: Field Pointer
 */
field* in_matrix(field*** const market, vector3 vec);

/*Deallocate 3d field array
 * @param market	: address of 3d field array
 * @param y, floor_count	: width, hight of the market
 */
void free_market(field*** market, int y, int floor_count);

/*Test if a field is blocked by something
 * @param market 	: field matrix
 * @param vec 		: position of the field in the Matrix
 * @return 			: Boolean
 */
int is_blocked(field*** const market, vector3 vec);

#endif
