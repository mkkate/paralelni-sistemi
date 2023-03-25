#define N 4000000
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	int* a, sum = 0, i;
	double average = 0.0;
	double start, end;

	int nthreads = omp_get_num_procs();
	omp_set_num_threads(nthreads);
	printf("Number of cores in the system = %d\n", nthreads);

	a = (int*)malloc(N * sizeof(int));
	if (a == nullptr)
		return -1;

	for (int i = 0; i < N; i++)
		a[i] = i;

#pragma region sequentially
	start = omp_get_wtime();
	for (int i = 0; i < N; i++)
	{
		sum += a[i];
	}
	average = (double)sum / N;
	end = omp_get_wtime();
	printf("Average = %f\n", average);
	printf("Sequentially: time = %f sec\n", end - start);
#pragma endregion

#pragma region parallel_no_reduction
	sum = 0;
	average = 0.0;
	start = omp_get_wtime();
#pragma omp parallel for
	for (i = 0; i < N; i++)
	{
#pragma omp critical
		sum += a[i];
	}
	average = (double)sum / N;
	end = omp_get_wtime();
	printf("W/o reduction: time = %f sec\n", end - start);
#pragma endregion

#pragma region parallel_reduction
	sum = 0;
	average = 0.0;
	start = omp_get_wtime();
#pragma omp parallel for reduction(+:sum)
	for (i = 0; i < N; i++)
	{
		sum += a[i];
	}
	average = (double)sum / N;
	end = omp_get_wtime();
	printf("With reduction: time = %f sec\n", end - start);
#pragma endregion

	return 0;
}