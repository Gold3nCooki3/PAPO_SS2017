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
		vector3 vect = {vec.x,vec.y - global__mmi->startcolumn,vec.z - global__mmi->startstorey};
		if(vect.z > global__mmi->stories -1 || vect.z < 0 || vect.y > global__mmi->columns -1|| vect.y < 0 || vect.x > global__mmi->rows -1 || vect.x < 0) return 0;
		return global__market[vect.z][vect.y][vect.x];
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
field* readfile(MPI_File *fh, meta * const mmi) {

	MPI_Offset start, end, filesize;
	MPI_Request req;
	MPI_Datatype MPI_field;

	int *firstline, fieldcount, mysize, overlap, flinecount = 8;
	int rank = mmi->rank, size = mmi->size;
	field *field_chunk;

    /*field strukt into MPI datatype*/
	MPI_Type_contiguous(3, MPI_INT, &MPI_field);
	MPI_Type_commit(&MPI_field);

    /* figure out who reads what */
    MPI_File_get_size(*fh, &filesize);
	fieldcount 	= (filesize/sizeof(int) - flinecount) / 3;
	mysize 		= fieldcount/size;
	start 		= rank * mysize;
    end   		= start + mysize;
    if (rank == size-1) end = fieldcount;

	/* add overlap to the end of everyone's chunk except last process ... */
    if (rank != size-1)
    end 		+= overlap;
	mysize 		= end - start;

	/* make start to an adress in the file */
	start  		= start * 3 * sizeof(int) + flinecount * sizeof(int);

	/* allocate memory */
    field_chunk	= malloc( mysize * sizeof(field));
	firstline 	= malloc( flinecount * sizeof(int));

	/* everyone reads in their part */
	MPI_File_read_all(*fh, firstline , flinecount, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_read_at_all(*fh, start, field_chunk, mysize, MPI_field, MPI_STATUS_IGNORE);

	/* switch endianess, when the processor is a little endian processor */
	#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	for(int t = 0; t < flinecount; t++){
		firstline[t] 		= __builtin_bswap32(firstline[t]);
	}
	#endif
	mmi->rows = firstline[0];
	mmi->columns = firstline[1];
	mmi->stories = mysize / (firstline[0] *  firstline[1]) + 1;
	mmi->startcolumn = ((mysize * rank) % (firstline[1] * firstline[0])) / firstline[1];
	mmi->startstorey = ((mysize * rank) / (firstline[1] * firstline[0]));

	for(int t = 0; t < mysize; t++){
	#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		field_chunk[t].type	= __builtin_bswap32(field_chunk[t].type);
		field_chunk[t].content	= __builtin_bswap32(field_chunk[t].content);
		field_chunk[t].amount	= __builtin_bswap32(field_chunk[t].amount);
	#endif
		switch (field_chunk[t].type){
			case SHELF:		mmi->shelf_count++; break;
			case LIFT:
			case ESCALATOR:	mmi->lift_count++; break;
			case REGISTER:	mmi->register_count++; break;
			case STOCK:		mmi->stock_count++; break;
			case EXIT:		mmi->exit_count++; break;
			default: break;
		}

	}
	#endif

	/*Print for testing
	if( rank == 0 ){
		printf("Process %d:\n", rank);
                for(int t = 0; t < flinecount; t++){
                        printf("%d, ", firstline[t]);
                }
		printf("\n");
		for(int i = 0; i < mysize; i++){
			int z = i / (firstline[1] * firstline[0]) ;
			int y = (i % (firstline[1] * firstline[0])) / firstline[1];
			int x = (i % (firstline[1] * firstline[0])) % firstline[1];
			printf("X: %2d, Y: %2d, Z: %2d ", x, y, z);
                        printf("T: %d, C: %d, A: %3d\n", field_chunk[i].type, field_chunk[i].content, field_chunk[i].amount);
                }
                printf("\n");

		for(int source = 1; source < size; source++){
			printf("Process: %d\n", source);
			int err = MPI_Recv(field_chunk, mysize, MPI_field, source, DEBUGTAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
       			int gi = source * mysize;
			printf("gi = %d, err = %d\n", gi, err);
			for(int i = 0; i < mysize; i++){
				gi += 1;
                	        int z = (gi / (firstline[1] * firstline[0]));
                       		int y = (gi % (firstline[1] * firstline[0])) / firstline[1];
                        	int x = (gi % (firstline[1] * firstline[0])) % firstline[1];
                        	printf("X: %2d, Y: %2d, Z: %2d ", x, y, z);
                        	printf("T: %d, C: %d, A: %3d\n", field_chunk[i].type, field_chunk[i].content, field_chunk[i].amount);
                	}
		}
	}else{
	 	MPI_Isend(field_chunk, mysize, MPI_field, MASTER, DEBUGTAG, MPI_COMM_WORLD, &req);
		printf("!!! T: %d, C: %d, A: %3d\n", field_chunk[0].type, field_chunk[0].content, field_chunk[0].amount);
	}
    */
	free(firstline);
	return field_chunk;
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

	MPI_File mallfile;
	int err = MPI_File_open(MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &mallfile);
	if(err != 0) exit(EXIT_FAILURE);
	field* matrix = readfile(&in, mmi);
	MPI_Fiele_close(mallfile);

	//Allocate pointers to specific field types
	vector3* shelves = malloc(sizeof(vector3)*mmi->shelf_count);
	vector3* lifts = malloc(sizeof(vector3)*mmi->lift_count);
	vector3* stocks = malloc(sizeof(vector3)*mmi->stock_count);
	vector3* registers = malloc(sizeof(vector3)*mmi->register_count);
	vector3* exits = malloc(sizeof(vector3)*mmi->exit_count);

	int q=0, w=0, e=0, r=0, t = 0, index = 0;
	
	field**** market = create_market(mmi->rows, mmi->columns, mmi->stories);

	for(int a = 0; a < mmi->stories; a++){
		for(int b = 0; b < mmi->columns; b++){
			for(int c = 0; c < mmi->rows; c++){
				market[a][b][c] = &matrix[index++];
				vector3 v = {c,b,a};
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
