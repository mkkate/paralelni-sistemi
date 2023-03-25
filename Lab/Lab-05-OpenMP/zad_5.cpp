#define N 900000
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	int* a, max_value = 0, i;
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
		if (a[i] > max_value)
			max_value = a[i];
	}
	end = omp_get_wtime();
	printf("max_value = %d\n", max_value);
	printf("Sequentially: time = %f sec\n", end - start);
#pragma endregion

#pragma region parallel_bad_critical
	max_value = 0;
	start = omp_get_wtime();
#pragma omp parallel for
	for (i = 0; i < N; i++)
	{
#pragma omp critical
		if (a[i] > max_value)
			max_value = a[i];
	}
	end = omp_get_wtime();
	printf("Bad critical: time = %f sec\n", end - start);
#pragma endregion

#pragma region parallel_good_critical
	int tmp_max = INT_MIN;
	int maxElem = INT_MIN;
	start = omp_get_wtime();
#pragma omp parallel firstprivate(tmp_max)
	{
#pragma omp for 
		for (i = 0; i < N; i++)
			if (a[i] > tmp_max)
				tmp_max = a[i];
		//printf("Thread %d: tmp max = %d\n", omp_get_thread_num(),tmp_max);
#pragma omp critical(maxElement)
		if (maxElem < tmp_max)
			maxElem = tmp_max;
	}
	end = omp_get_wtime();
	//printf("Good critical: max_value = %d\n", maxElem);
	printf("Good critical: time = %f sec\n", end - start);
#pragma endregion

	/* // kada bih koristila OpenMP 3.1
#pragma region parallel_reduction
	max_value = 0;
	start = omp_get_wtime();
	#pragma omp parallel for reduction(max:max_value)
		for (int i = 0; i < N; i++)
		{
			if (a[i] > max_value)
				max_value = a[i];
		}
	end = omp_get_wtime();
	printf("With red: max_value = %d\n", max_value);
	printf("With reduction: time = %f sec\n", end - start);
#pragma endregion
	*/

	return 0;
}