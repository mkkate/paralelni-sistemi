/*
zad1. Napisati program kojim se korišćenjem izvedenih tipova podataka vrši
slanje kolone 0 procesa 0 u vrstu 0 procesa 1.
		P0						P1
	A = {0, 1, 2,			A = {0, 5, 6,
		 5, 6, 7,		=>		 5, 6, 7,
		 10, 11, 12}			 10, 11, 12}
*/

#define N 3
#define MCW MPI_COMM_WORLD
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	int rank, * a0, * a1;

	MPI_Status st;
	MPI_Datatype newtype;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);

	MPI_Type_vector(N, 1, N, MPI_INT, &newtype);
	MPI_Type_commit(&newtype);

	a0 = (int*)malloc(N * N * sizeof(int));
	a1 = (int*)malloc(N * N * sizeof(int));
	if (a0 == nullptr || a1 == nullptr)
		return -1;

	if (rank == 0)
	{
		printf("Unesi elemente matrice A:\n");
		fflush(stdout);
		for (int i = 0; i < N * N; i++)
			scanf_s("%d", &a0[i]);

		printf("A0:\n");
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
				printf("%d ", a0[i * N + j]);
			printf("\n");
		}


		MPI_Send(a0, 1, newtype, 1, 0, MCW);
	}
	else
		MPI_Recv(a1, N, MPI_INT, 0, 0, MCW, &st);

	if (rank == 1)
	{
		printf("A1:\n");
		for (int i = 0; i < N; i++)
			printf("%d ", a1[i]);
	}

	free(a0);
	free(a1);

	MPI_Finalize();

	return 0;
}