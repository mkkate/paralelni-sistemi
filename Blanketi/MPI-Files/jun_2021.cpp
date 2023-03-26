#define MCW MPI_COMM_WORLD
#define NUM 105
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	int rank, size, buf1[NUM], buf2[NUM], count, intsize;
	bool equal = true;
	int count, * blocklengths, * displacements;

	MPI_File fh1, fh2, fh3;
	MPI_Offset offset;
	MPI_Datatype indexed;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	for (int i = 0; i < NUM; i++)
		buf1[i] = rank * NUM + i;

	// 1. 
	MPI_File_open(MCW, "file1.dat", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh1);
	offset = (size - rank - 1) * NUM * sizeof(int);
	MPI_File_seek(fh1, offset, MPI_SEEK_END);
	MPI_File_write_all(fh1, buf1, NUM, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&fh1);

	// 2.
	MPI_File_open(MCW, "file1.dat", MPI_MODE_WRONLY, MPI_INFO_NULL, &fh2);
	MPI_File_read_at_all(fh2, offset, buf2, NUM, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&fh2);

	for (int i = 0; i < NUM; i++)
	{
		if (buf1[i] != buf2[i])
		{
			equal = false;
			break;
		}
	}
	printf("Data from both files is %s\n", equal ? "true" : "false");

	// 3.
	count = size - 1;
	for (int i = 0; i < size; i++)
	{
		blocklengths[i] = (i + 1) * NUM;
		displacements[i] = (i + 1) * (size - 1) * NUM * sizeof(int);
	}

	MPI_Type_indexed(count, blocklengths, displacements, MPI_INT, &indexed);
	MPI_Type_commit(&indexed);

	MPI_File_open(MCW, "file3.dat", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh3);
	MPI_File_set_view(fh3, rank * sizeof(int), MPI_INT, indexed, "native", MPI_INFO_NULL);
	MPI_File_write_all(fh3, buf2, NUM, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&fh3);

	MPI_Finalize();

	return 0;
}