#define RECORDS_SIZE 10*1024*1024	// 10 MB
#define MCW MPI_COMM_WORLD
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	int rank, size, *buf, * file_data, count;

	MPI_File fh1, fh2;
	MPI_Offset file_size, portion, segment;
	MPI_Datatype filetype;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	buf = (int*)malloc(RECORDS_SIZE * sizeof(int));

	MPI_File_open(MCW, "file1.dat", MPI_MODE_RDONLY, MPI_INFO_NULL, &fh1);
	MPI_File_get_size(fh1, &file_size);

	portion = file_size / size;
	count = portion / sizeof(int);
	file_data = (int*)malloc(portion);

	MPI_File_read_shared(fh1, file_data, count, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&fh1);



	MPI_Type_vector(2, count / 2, file_size / sizeof(int) / 2, MPI_INT, &filetype);
	MPI_Type_commit(&filetype);
	segment = portion / 2;
	MPI_File_open(MCW, "file2.dat", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh2);
	MPI_File_set_view(fh2, rank * segment, MPI_INT, filetype, "native", MPI_INFO_NULL);
	MPI_File_write_all(fh2, file_data, count, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&fh2);

	MPI_Finalize();

	return 0;
}