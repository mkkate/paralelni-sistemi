#define N 50000000
#define NTHREADS 4
#include <omp.h>
#include <stdio.h>
#include "initializations.h"

int main()
{
	int prod, offset = 0, i, * a_seq{}, * a_par{}, * b{}, * c{}, * x{}, * acopy{};

	double start, end;

	initialize_vector(a_seq, N);
	initialize_vector(a_par, N);
	initialize_vector(b, N);
	initialize_vector(c, N);
	initialize_vector(x, N);
	initialize_vector(acopy, N);

	omp_set_num_threads(NTHREADS);

#pragma region sequentially
	start = omp_get_wtime();
	prod = 1;
	for (i = 0; i < N; i++)
	{
		a_seq[i] = a_seq[i + 1] + b[i] * c[offset + N - i - 1];
		prod = prod * x[i];
	}
	end = omp_get_wtime();
	printf("\nSeq time = %f\n", end - start);
#pragma endregion

#pragma region parallel
	start = omp_get_wtime();
	prod = 1;
#pragma omp parallel
	{
#pragma omp for
		for (i = 0; i < N; i++)
			acopy[i] = a_par[i + 1];
#pragma omp for reduction(*:prod)
		for (i = 0; i < N; i++)
		{
			a_par[i] = acopy[i] + b[i] * c[offset + N - i - 1];
			prod = prod * x[i];
		}
	}
	end = omp_get_wtime();
	printf("\nPar time = %f\n", end - start);
#pragma endregion

	return 0;
}