/*
zad2b. Vrajicija zad2a: salje se donja A u gornju trougaonu T.
		P0						P1
	A = {0, 1, 2,			T = {0, 3, 4,
		 3, 4, 5,		=>		 0, 6, 7,
		 6, 7, 8}				 0, 0, 8}
	n = 3, br. procesa = 2
*/

#define N 3
#define MCW MPI_COMM_WORLD
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	int rank, * a, * t;
	int* array_of_blocklengths, * array_of_displacements;

	MPI_Datatype gornja, donja;

	MPI_Status st;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);

	array_of_blocklengths = (int*)malloc(N * sizeof(int));
	array_of_displacements = (int*)malloc(N * sizeof(int));
	if (array_of_blocklengths == nullptr || array_of_displacements == nullptr)
		return -1;

	for (int i = 0; i < N; i++)
	{
		array_of_blocklengths[i] = N - i;
		array_of_displacements[i] = i * N + i;
	}
	MPI_Type_indexed(N, array_of_blocklengths, array_of_displacements, MPI_INT, &gornja);
	MPI_Type_commit(&gornja);

	for (int i = 0; i < N; i++)
	{
		array_of_blocklengths[i] = i + 1;
		array_of_displacements[i] = i * N;
	}
	MPI_Type_indexed(N, array_of_blocklengths, array_of_displacements, MPI_INT, &donja);
	MPI_Type_commit(&donja);

	a = (int*)malloc(N * N * sizeof(int));
	t = (int*)malloc(N * N * sizeof(int));
	if (a == nullptr || t == nullptr)
		return -1;

	if (rank == 0)
	{
		printf("Unesi elemente matrice A:\n");
		fflush(stdout);

		for (int i = 0; i < N * N; i++)
			scanf_s("%d", &a[i]);

		printf("Inicijalna matrica A:\n");
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
				printf("%d ", a[i * N + j]);
			printf("\n");
		}


		MPI_Send(a, 1, donja, 1, 0, MCW);
	}
	else
	{
		// ako se inicijalizuje u P0, onda ce nakon MPI_Send elementi koji nisu iz newtype
		// ce imati neku bezveze vrednost
		for (int i = 0; i < N * N; i++)
			t[i] = 0;

		printf("Inicijalna matrica T:\n");
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
				printf("%d ", t[i * N + j]);
			printf("\n");
		}

		MPI_Recv(t, 1, gornja, 0, 0, MCW, &st);

		printf("Rezultujuca matrica T:\n");
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
				printf("%d ", t[i * N + j]);
			printf("\n");
		}
	}

	free(a);
	free(t);

	MPI_Finalize();

	return 0;
}