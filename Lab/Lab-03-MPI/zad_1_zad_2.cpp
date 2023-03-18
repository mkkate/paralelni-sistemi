/*
	N = 6, Br. procesa = 3
*/
#define N 6
#define MCW MPI_COMM_WORLD
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	int rank, size, A[N][N]{}, * niz;

	struct {
		int value;
		int rank;
	}local_min, global_min, local_max, global_max;


	MPI_Datatype vector, column;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	int niz_size = (N / 2) * (N / size);
	niz = new int[niz_size] {};

	MPI_Type_vector(N / 2, 1, 2, MPI_INT, &vector);
	MPI_Type_commit(&vector);
	MPI_Type_create_resized(vector, 0, niz_size * sizeof(int), &column);
	MPI_Type_commit(&column);

	if (rank == 0)
	{
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
				A[i][j] = i * N + j;

		printf("A:\n");
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
				printf("%d ", A[i][j]);
			printf("\n");
		}
	}

	MPI_Scatter(A, N / size, column, niz, (N / 2) * (N / size), MPI_INT, 0, MCW);

	printf("P[%d] niz: ", rank);
	for (int j = 0; j < niz_size; j++)
		printf("%d ", niz[j]);
	printf("\n");

	local_min = { INT_MAX,rank };
	local_max = { INT_MIN,rank };
	for (int i = 0; i < niz_size; i++)
	{
		if (local_min.value > niz[i])
			local_min.value = niz[i];

		if (local_max.value < niz[i])
			local_max.value = niz[i];
	}
	printf("P[%d]: local_min = %d\n", rank, local_min.value);
	printf("P[%d]: local_max = %d\n", rank, local_max.value);

	MPI_Reduce(&local_max, &global_max, 1, MPI_2INT, MPI_MAXLOC, 0, MCW);
	MPI_Reduce(&local_min, &global_min, 1, MPI_2INT, MPI_MINLOC, 0, MCW);

	if (rank == 0)
	{
		printf("Resenje zadatka:\n");
		printf("P[%d]: Globalni minimum je %d, iz procesa %d\n", rank, global_min.value, global_min.rank);
		printf("P[%d]: Globalni maximum je %d, iz procesa %d\n", rank, global_max.value, global_max.rank);
	}

	MPI_Finalize();

	return 0;
}