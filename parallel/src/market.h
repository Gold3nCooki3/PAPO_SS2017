#ifndef MARKET_H
#define MARKET_H

#define TRUE 1
#define FALSE 0
#define MASTER 0
#define DEBUGTAG 1

#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"

typedef enum FieldType {CORRIDOR, SHELF, ESCALATOR, LIFT, REGISTER, BLOCKVAL, STOCK, EXIT=7} FieldType;

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

struct
marketmetainfo {
	int rows;
	int columns;
	int stories;

	int startcolumn;
	int startstorey;
	int linecount;
	int chunksize;
	int startline;
	int rank;
	int size;

	int edge_count;
	int shelf_count;
	int lift_count;
	int stock_count;
	int register_count;
	int exit_count;

	int spawn_count;
	int empty_count;
	int entity_count;

	vector3* edge_fields;
	vector3* shelf_fields;
	vector3* lift_fields;
	vector3* stock_fields;
	vector3* register_fields;
	vector3* exit_fields;
	
	field* matrix;
};
typedef struct marketmetainfo meta;

static field**** global__market;
static meta* global__mmi;

/* Test if Vectors are equal
 * @param vec1, vec2 address of the vectors
 * @return Boolean
 */
int vec_equal(vector3 * vec1, vector3 * vec2);

/*Import market data form textfile/csv,
 * @param path		: path to text file
 * @param mmi
 * @return 			: pointer of the filled array
 */
field**** import_market(char* path, meta *mmi);

/*Gives back the field pointer in the field Matrix
 * described by the vector position
 * @param vec 		: position of the field in the Matrix
 * @return 			: Field Pointer
 */
field* in_matrix_g(vector3 vec);

/*
 *
 */
int in_process(vector3 vec);

/*
 *
 */
void parprocess(MPI_File *in, const int rank, const int size, const int overlap);

/*Deallocate 3d field array
 * @param global__market
 */
void free_market();

/*Deallocates the meta object
 * @param global__mmi
 */
void free_meta();

/*Test if a field is blocked by something
 * @param vec 		: position of the field in the Matrix
 * @return 			: Boolean
 */
int is_blocked(vector3 vec);

#endif
