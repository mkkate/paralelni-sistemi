/*
zad2a. Napisati program kojim se elementi gornje trougaone matrice Anxn
procesa 0, šalju u gornju trougaonu matricu Tnxn procesa 1.
		P0						P1
	A = {0, 1, 2,			T = {0, 1, 2,
		 3, 4, 5,		=>		 0, 4, 5,
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

	MPI_Datatype newtype;

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
	
	MPI_Type_indexed(N, array_of_blocklengths, array_of_displacements, MPI_INT, &newtype);
	MPI_Type_commit(&newtype);

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


		MPI_Send(a, 1, newtype, 1, 0, MCW);
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

		MPI_Recv(t, 1, newtype, 0, 0, MCW, &st);

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