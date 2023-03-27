/*
	z = m;
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
		{
			x[j] += y[z];
			z += 2;
		}

Dinamicki alociraj nizove da bi se testiralno za vece vrednosti !!
*/

#define N 200
#define NTHREADS 4
#define m 1
#include <omp.h>
#include <stdio.h>

int main()
{
	int z_seq, x_seq[N]{}, y_seq[m + 2 * N * N]{}, i, j;
	int z_par, x_par[N]{}, y_par[m + 2 * N * N]{};

	double start, end;

	omp_set_num_threads(NTHREADS);

#pragma region sequentially
	start = omp_get_wtime();

	z_seq = m;
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
		{
			x_seq[j] += y_seq[z_seq];
			z_seq += 2;
		}

	end = omp_get_wtime();
	printf("Seq time = %f\n", end - start);
#pragma endregion

#pragma region parallel
	start = omp_get_wtime();

	z_par = m;
#pragma omp paraller for lastprivate(z_par)
	for (i = 0; i < N * N; i++)
	{
		z_par = m + 2 * i;
#pragma omp critical
		x_par[i % N] += y_par[z_par];
		z_par += 2;	// treba nam zbog lastprivate, da bude vrednost +=2 u odnosu na poslednje m+2*i
	}

	end = omp_get_wtime();
	printf("Par time = %f\n", end - start);
#pragma endregion

	return 0;
}