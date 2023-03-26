// koraci 1 i 2 dobri
#define N 4
#define MCW MPI_COMM_WORLD
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	int rank, size, buf1[N], buf2[N], buf3[N][2 * N]{}, count;
	bool consistent = true;

	MPI_File fh1, fh2, fh3;
	MPI_Offset offset;
	MPI_Datatype vector, resized, filetype;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	for (int i = 0; i < N; i++)
		buf1[i] = rank * N + i;

	// 1.
	MPI_File_open(MCW, "prva.dat", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh1);
	MPI_File_seek(fh1, rank * N * sizeof(int), MPI_SEEK_SET);
	MPI_File_write_all(fh1, buf1, N, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&fh1);

	printf("P[%d]: buf1 = ", rank);
	for (int i = 0; i < N; i++)
		printf("%d ", buf1[i]);
	printf("\n");

	// 2.
	MPI_File_open(MCW, "prva.dat", MPI_MODE_RDONLY, MPI_INFO_NULL, &fh2);
	MPI_File_read_shared(fh2, buf2, N, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&fh2);

	printf("P[%d]: buf2 = ", rank);
	for (int i = 0; i < N; i++)
		printf("%d ", buf2[i]);
	printf("\n");

	for (int i = 0; i < N; i++)
		if (buf1[i] != buf2[i])
		{
			consistent = false;
			break;
		}
	printf("Cosistency is %s\n", consistent ? "true" : "false");

	// 3. buf3 nije dobar ispis
	MPI_Type_vector(N, 1, 2 * N, MPI_INT, &vector);
	//MPI_Type_commit(&vector);
	MPI_Type_create_resized(vector, 0, sizeof(int), &resized);
	//MPI_Type_commit(&resized);
	MPI_Type_contiguous(2, resized, &filetype);
	MPI_Type_commit(&filetype);

	MPI_File_open(MCW, "druga.dat", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh3);
	MPI_File_set_view(fh3, rank * 2 * sizeof(int), MPI_INT, filetype, "native", MPI_INFO_NULL);
	MPI_File_write_all(fh3, buf3, 2 * N, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&fh3);

	printf("P[%d]: buf3 =\n", rank);
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < 2 * N; j++)
			printf("%d ", buf3[i][j]);
		printf("\n");
	}

	printf("\n\n");

	MPI_Finalize();

	return 0;
}