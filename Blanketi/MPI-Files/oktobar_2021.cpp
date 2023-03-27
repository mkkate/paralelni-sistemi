#define MCW MPI_COMM_WORLD
#define MINULL MPI_INFO_NULL
#define MSI MPI_STATUS_IGNORE
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	int rank, size, portion, count, vector_portion, * buf1;

	MPI_File fh1, fh2, fh3;
	MPI_Offset file_size;
	MPI_Datatype file_type;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);


	MPI_File_open(MCW, "file1.dat", MPI_MODE_RDONLY, MINULL, &fh1);
	MPI_File_get_size(fh1, &file_size);
	portion = file_size / size;
	count = portion / sizeof(int);
	buf1 = (int*)malloc(count);
	MPI_File_read_at_all(fh1, rank * portion, buf1, count, MPI_INT, MSI);
	MPI_File_close(&fh1);


	MPI_File_open(MCW, "file2.dat", MPI_MODE_CREATE | MPI_MODE_WRONLY, MINULL, &fh2);
	MPI_File_write_shared(fh2, buf1, count, MPI_INT, MSI);
	MPI_File_close(&fh2);


	vector_portion = portion / 5;
	MPI_Type_vector(5, vector_portion / sizeof(int), size * vector_portion / sizeof(int), MPI_INT, &file_type);
	MPI_Type_commit(&file_type);
	MPI_File_open(MCW, "file3.dat", MPI_MODE_CREATE | MPI_MODE_WRONLY, MINULL, &fh3);
	MPI_File_set_view(fh3, (size - 1 - rank) * sizeof(int), MPI_INT, file_type, "native", MINULL);
	MPI_File_write_all_begin(fh3, buf1, count, MPI_INT);
	for (int i = 0; i < count; i++)
		printf("%d ", buf1[i]);	// neka analiza, nije bitno
	MPI_File_write_all_end(fh3, buf1, MSI);
	MPI_File_close(&fh3);

	free(buf1);

	MPI_Finalize();

	return 0;
}