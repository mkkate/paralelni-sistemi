/*
zad4. Napisati MPI program koji realizuje množenje matrice Amxn i matrice Bnxk,
čime se dobija i prikazuje rezultujuća matrica Cmxk.
Izračunavanje se obavlja tako što master proces šalje svakom procesu po jednu kolonu
matrice A i po jednu vrstu matrice B.
Svi elementi kolone matrice A se šalju odjednom.
Svi procesi učestvuju u izračunavanjima potrebnim za generisanje rezultata programa.
Zadatak rešiti isključivo primenom grupnih operacija.

NAPOMENA: br. procesa == N !!!
*/

#define M 2
#define N 3
#define K 4
#define MCW MPI_COMM_WORLD
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	int rank, size, root = 0;
	int a[M][N]{}, b[N][K]{}, c[M][K], local_c[M][K];
	int niz_a[M], niz_b[K];

	MPI_Datatype vector, column;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	MPI_Type_vector(M, 1, N, MPI_INT, &vector);
	MPI_Type_commit(&vector);
	MPI_Type_create_resized(vector, 0, 1 * sizeof(int), &column);
	MPI_Type_commit(&column);

	// inicijalizacija
	if (rank == root)
	{
		for (int i = 0; i < M; i++)
			for (int j = 0; j < N; j++)
				a[i][j] = i + j;

		for (int i = 0; i < N; i++)
			for (int j = 0; j < K; j++)
				b[i][j] = 1 + i + j;

		for (int i = 0; i < M; i++)
			for (int j = 0; j < K; j++)
				c[i][j] = 0;

		printf("A:\n");
		for (int i = 0; i < M; i++)
		{
			for (int j = 0; j < N; j++)
				printf("%d ", a[i][j]);
			printf("\n");
		}

		printf("B:\n");
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < K; j++)
				printf("%d ", b[i][j]);
			printf("\n");
		}

		printf("C:\n");
		for (int i = 0; i < M; i++)
		{
			for (int j = 0; j < K; j++)
				printf("%d ", c[i][j]);
			printf("\n");
		}
	}

	MPI_Scatter(a, 1, column, niz_a, M, MPI_INT, root, MCW);
	MPI_Scatter(b, K, MPI_INT, niz_b, K, MPI_INT, root, MCW);

	for (int i = 0; i < M; i++)
		for (int j = 0; j < K; j++)
			local_c[i][j] = niz_a[i] * niz_b[j];

	MPI_Reduce(local_c, c, M * K, MPI_INT, MPI_SUM, root, MCW);

	if (rank == root)
	{
		printf("Rezultujuca C:\n");
		for (int i = 0; i < M; i++)
		{
			for (int j = 0; j < K; j++)
				printf("%d ", c[i][j]);
			printf("\n");
		}
	}

	MPI_Finalize();

	return 0;
}