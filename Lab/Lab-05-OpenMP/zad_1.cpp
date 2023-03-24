#define N 206
#include "omp.h"
#include <stdio.h>
#include <cstdio>
#include <ctime>

int main()
{
	int a[N][N], b[N][N], c[N][N];
	int a2[N][N], b2[N][N], c2[N][N];

	double start, end, start2, end2;

	// inicijalizacija matrica A, B i C
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
		{
			a[i][j] = i * N + j + 1;
			a2[i][j] = a[i][j];

			b[i][j] = i + j + 1;
			b2[i][j] = b[i][j];

			c[i][j] = 0;
			c2[i][j] = 0;
		}

	/*printf("A:\n");
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
			printf("%d\t", a[i][j]);
		printf("\n");
	}
	printf("B:\n");
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
			printf("%d\t", b[i][j]);
		printf("\n");
	}*/

	start = omp_get_wtime();

	// sekvencijalno racunanje matrice C
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			for (int k = 0; k < N; k++)
				c[i][j] += a[i][k] * b[k][j];


	end = omp_get_wtime();
	printf("Sekvencijalna C je izracunata za %f sec\n", (end - start));
	/*for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
			printf("%d\t", c[i][j]);
		printf("\n");
	}*/


	int i, j, k;
	int nthreads = omp_get_num_procs();
	printf("Broj jezgara u sistemu je %d \n", nthreads);

	omp_set_num_threads(nthreads);

	start2 = omp_get_wtime();

#pragma omp parallel for private(j,k)//shared(a2,b2,c2) private(i,j,k)
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			//printf("Nit %d racuna iteraciju %d, %d\n", omp_get_thread_num(), i, j);
			for (k = 0; k < N; k++)
			{
				c2[i][j] += a2[i][k] * b2[k][j];
			}
		}
	}

	end2 = omp_get_wtime();

	printf("Paralelizovana C2 je izracunata za %f sec:\n", (end2 - start2));
	/*for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
			printf("%d\t", c2[i][j]);
		printf("\n");
	}*/

}