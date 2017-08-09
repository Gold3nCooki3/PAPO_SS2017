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
 * @param vec 		: position of the field in the Matrix
 * @return 			: Field Pointer
 */
field* in_matrix_g(vector3 vec){
	if(global__market){
		if(vec.z > global__mmi->stories -1 || vec.z < 0 || vec.y > global__mmi->columns -1|| vec.y < 0 || vec.x > global__mmi->rows -1 || vec.x < 0) return 0;
		return global__market[vec.z][vec.y][vec.x];
	}else{
		printf("Error: no global matrix");
		exit(EXIT_FAILURE);
	}
}


/*Test if a field is blocked by something
 * @param vec 		: position of the field in the Matrix
 * @return 			: Boolean
 */
int is_blocked(vector3 vec){
	field* f= in_matrix_g(vec);
	if(f != NULL){
		switch (f->type){
			case CORRIDOR:
			case ESCALATOR:
			case LIFT:
			case REGISTER:
			case STOCK:
			case EXIT: return FALSE; break;
			case SHELF:
			case BLOCKVAL:
			default: return TRUE; break;
		}
	}
	return TRUE;
}

/*
 *
 */
void parprocess(MPI_File *fh, const int rank, const int size, const int overlap) {
	MPI_Offset globalstart;
    int mysize;
    char *chunk, *chunk2;
    MPI_Request   req;
	  /* read in relevant chunk of file into "chunk",
     * which starts at location in the file globalstart
     * and has size mysize
     */
        MPI_Offset globalend;
        MPI_Offset filesize;

        //field strukt into MPI datarype
        MPI_Datatype mpi_field_type;
        MPI_Type_contiguous(3, MPI_INT, &mpi_field_type);
	MPI_Type_commit(&mpi_field_type);

         /* figure out who reads what */
        MPI_File_get_size(*fh, &filesize);
        filesize--;  /* get rid of text file eof */
        mysize = filesize/size;
        printf("%d, s(mpi_field): %d\n", mysize, sizeof(mpi_field_type));
	/*globalstart = rank * mysize;
        globalend   = globalstart + mysize - 1;
        if (rank == size-1) globalend = filesize-1;
	*/
        /* add overlap to the end of everyone's chunk except last proc... */
        //if (rank != size-1)
        //globalend += overlap;

        //mysize =  globalend - globalstart + 1;

        /* allocate memory */
        //chunk = malloc( (mysize + 1)*sizeof(mpi_field_type));
        chunk2 = malloc(8*sizeof(char));
	
        /* everyone reads in their part */
        MPI_File_read_all(*fh, chunk2 , 8, MPI_BYTE, MPI_STATUS_IGNORE);
        //MPI_File_read_at_all(*fh, globalstart, chunk, mysize, mpi_field_type, MPI_STATUS_IGNORE);
        //chunk[mysize] = '\0';

	//MPI_Barrier(MPI_COMM_WORLD);

	if( rank == 0 ){
		printf("\n %d: \n", rank);
                for(int i = 0; i < 8; i++){
                        printf("%d\n", chunk2[i]);
                }}
        /*        for(int i = 0; i < 200; i++){
                        printf("%c", chunk[i]);
                }
                printf("\n");printf("...\n");
		int b = 1;
		for(; b < size; b++){
			int ieee = MPI_Recv(chunk, mysize+1, MPI_CHAR, b, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
       			for(int i = 0; i < 20; i++){
       				printf("%c", chunk[i]);
       			}
			printf("\n");printf("...\n");
		}
	}else{
	 	MPI_Isend(chunk, mysize, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &req);
	}
	*/
	free(chunk2);
        //free(chunk);
}

/*Initializes an 3d field array
 *	field array only for access
 * @param x, y, floor_count : length, width and hight of the market
 * @return 			: pointer of allocated field array
 */
field**** create_market(int rows, int columns, int stories){
	field **** market = malloc(stories * sizeof(field***));
	for(int i = 0; i < stories; i++){
		field*** floor = malloc(columns * sizeof(field**));
		market[i] = floor;
		for(int j = 0; j < columns; j++){
			floor[j] = malloc(rows * sizeof(field*));
		}
	}
	global__market = market;
	return market;
}

/*Import market data form textfile/csv,
 * @param path		: path to text file
 * @param mmi		; pointer to meta struct
 * @return 			: pointer of the filled array
 */
field**** import_market(char* path, meta *mmi){

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

	int q=0, w=0, e=0, r=0, t = 0, temp = 0;
	
	field* matrix = calloc(mmi->rows * mmi->columns * mmi->stories, sizeof(field));
	field**** market = create_market(mmi->rows, mmi->columns, mmi->stories);
	for(int a = 0; a < mmi->stories; a++){
		for(int b = 0; b < mmi->columns; b++){
			for(int c = 0; c < mmi->rows; c++){
				fscanf(file, "%d,%d,%d\n",
					&matrix[temp].type,
					&matrix[temp].content,
					&matrix[temp].amount);
				market[a][b][c] = &matrix[temp];
				vector3 v = {c,b,a};
				temp++;
				//printf("(%d, %d, %d)\n",c,b,a);
				switch(market[a][b][c]->type) {
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
	global__mmi = mmi;
	global__mmi->matrix = matrix;
return market;
}

/*Deallocate 3d field array
 * TODO: needs to be extended when create_market is optimized
 * @param global_market
 */
void free_market(){
	for(int o = 0; o < global__mmi->stories; o++ ){
		for(int i = 0; i < global__mmi->columns; i++){
			free(global__market[o][i]);
		}
		free(global__market[o]);
	}
	free(global__market);
}

void free_meta(){
	free(global__mmi->matrix);
	free(global__mmi->shelf_fields);
	free(global__mmi->lift_fields);
	free(global__mmi->stock_fields);
	free(global__mmi->register_fields);
	free(global__mmi->exit_fields);
}
