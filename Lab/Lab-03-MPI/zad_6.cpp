// br. procesa = N = 3
#define N 3
#define MCW MPI_COMM_WORLD
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	int rank, size;
	int A[N][N]{}, B[N][N]{}, C[N][N]{};
	int local_a[N], local_c[N]{}, local_max[N]{};

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	if (rank == 0)
	{
		printf("A:\n");
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				A[i][j] = i * N + j + 1;
				printf("%d\t", A[i][j]);
			}
			printf("\n");
		}

		printf("B:\n");
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				B[i][j] = i * N + j + 2;
				printf("%d\t", B[i][j]);
			}
			printf("\n");
		}
	}

	MPI_Scatter(A, N, MPI_INT, local_a, N, MPI_INT, 0, MCW);
	MPI_Bcast(B, N * N, MPI_INT, 0, MCW);

	// mnozenje
	printf("P[%d]: local_c = ", rank);
	for (int i = 0; i < N; i++)
	{
		local_c[i] = 0;
		for (int j = 0; j < N; j++)
			local_c[i] += local_a[j] * B[j][i];
		printf("%d\t", local_c[i]);
	}

	MPI_Gather(local_c, N, MPI_INT, C, N, MPI_INT, 0, MCW);

	// nalazenje maksimuma svake kolone iz C
	/* //umesto preko for petlje, moze MPI_Reduce
	for (int i = 0; i < N; i++)
		local_max[i] = INT_MIN;
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			if (C[i][j] > local_max[j])
				local_max[j] = C[i][j];
	*/
	MPI_Reduce(local_c, local_max, N, MPI_INT, MPI_MAX, 0, MCW);

	if (rank == 0)
	{
		printf("\nRezultujuce C:\n");
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
				printf("%d\t", C[i][j]);
			printf("\n");
		}

		printf("Max svake kolone iz C:\n");
		for (int i = 0; i < N; i++)
			printf("%d\t", local_max[i]);
	}

	MPI_Finalize();

	return 0;
}