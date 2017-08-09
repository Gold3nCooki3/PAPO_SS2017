#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mpi.h"

void fn1(MPI_File *fh, int rank, int size){
	char *buf;
	int  *buf2;
	int c = 8;

	buf 	= calloc(c, sizeof(int));
	buf2	= calloc(c, sizeof(int));
	MPI_File_set_view(*fh, 0, MPI_INT, MPI_INT, NULL, MPI_INFO_NULL);
	MPI_File_read_all(*fh, buf2, c, MPI_INT, MPI_STATUS_IGNORE);

	printf("%d\n", buf[0]);
	printf("%d\n", buf[3]);
	printf("i: %d\n", buf2[0]);

	free(buf);
	free(buf2);
}

void fn2(MPI_File *fh, int rank, int size){
	int buffer[5] = {20456, 2, 3, 256, 0};
	MPI_Offset myoffset;
	myoffset = rank * 5 * sizeof(int);

	MPI_File_set_view(*fh, 0, MPI_INT, MPI_INT, NULL, MPI_INFO_NULL);
	MPI_File_write_at(*fh, myoffset, buffer, 5, MPI_INT, MPI_STATUS_IGNORE);

}


int main(int argc, char *argv[]){
	MPI_Init(&argc, &argv);
	MPI_File fh, fh2;
	int rank, size;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int err = MPI_File_open(MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
	if(err != 0 ) exit(42);
	err = MPI_File_open(MPI_COMM_WORLD, argv[2], MPI_MODE_WRONLY, MPI_INFO_NULL, &fh2);
	if(err != 0 ) exit(42);
	fn2(&fh2, rank, size);
	fn1(&fh, rank, size);

MPI_File_close(&fh);
MPI_File_close(&fh2);
MPI_Finalize();
exit(0);
}


