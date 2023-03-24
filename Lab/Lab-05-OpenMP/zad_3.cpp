#define N 12000
#include <omp.h>
#include <stdio.h>
#include "math.h"

int isPrimeNumber(int a)
{
	if (a == 2)
		return 1;
	for (int i = 2; i <= sqrt(a); i++)
		if (a % i == 0)
			return 0;
	return 1;
}

int main()
{
	int nprimes = 0, i;
	double start, end;

	int nthreads = omp_get_num_procs();
	printf("Number of system's cores is %d\n", nthreads);
	omp_set_num_threads(nthreads);

#pragma region sequentially
	start = omp_get_wtime();
	for (int i = 2; i <= N; i++)
	{
		nprimes += isPrimeNumber(i);
	}
	end = omp_get_wtime();

	printf("Prime numbers from 1 to %d = %d\n", N, nprimes);
	printf("Sequentially time = %f\n", end - start);

#pragma endregion

#pragma region parallel_without_reduction
	nprimes = 0;
	start = omp_get_wtime();
#pragma omp parallel for
	for (i = 2; i <= N; i++)
	{
		nprimes += isPrimeNumber(i);
	}
	end = omp_get_wtime();
	printf("W/o reduction time = %f\n", end - start);
#pragma endregion

#pragma region parallel_with_reduction
	nprimes = 0;
	start = omp_get_wtime();
#pragma omp parallel for reduction(+:nprimes)
	for (i = 2; i <= N; i++)
	{
		nprimes += isPrimeNumber(i);
	}
	end = omp_get_wtime();

	printf("With reduction time = %f\n", end - start);
#pragma endregion

	return 0;
}