// br. procesa = 4
#define MCW MPI_COMM_WORLD
#define N 6
#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	int rank, size;
	int A[N][N], T[N][N];

	MPI_Datatype send;
	MPI_Request req;
	MPI_Status st;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	if (rank == 0)
	{
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
				A[i][j] = i * N + j;

		printf("A:\n");
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
				printf("%d\t", A[i][j]);
			printf("\n");
		}
	}

	MPI_Type_create_resized(MPI_INT, 0, (N - 1) * sizeof(int), &send);
	MPI_Type_commit(&send);

	if (rank == 0)
	{
		for (int i = 0; i < size; i++)
		{
			MPI_Isend(&A[0][N - (i + 1)], N - i, send, i, 0, MCW, &req);
			MPI_Isend(&A[i][N - 1], N - i, send, i, 0, MCW, &req);
		}
	}

	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			T[i][j] = 0;

	MPI_Recv(&T[0][0], N - rank, MPI_INT, 0, 0, MCW, &st);
	MPI_Recv(&T[1][0], N - rank, MPI_INT, 0, 0, MCW, &st);


	printf("P[%d]:\n", rank);
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
			printf("%d\t", T[i][j]);
		printf("\n");
	}

	MPI_Finalize();

	return 0;
}