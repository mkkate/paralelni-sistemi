/*
	v = start;
	sum = 0;
	for(int i = 0; i < N; i++)
	{
		sum = sum + f(v);
		v = v+ step;
	}
*/

#define N 500
#define NTHREADS 4
#include <omp.h>
#include <stdio.h>

int f(int v)
{
	return v;
}

int main()
{
	int start = 0, step = 2, v_seq, v_par, sum_seq, sum_par;

	double start_time, end_time;

	omp_set_num_threads(NTHREADS);

#pragma region sequentially
	start_time = omp_get_wtime();

	v_seq = start;
	sum_seq = 0;
	for (int i = 0; i < N; i++)
	{
		sum_seq = sum_seq + f(v_seq);
		v_seq = v_seq + step;
	}

	end_time = omp_get_wtime();
	printf("Sequential time = %f\n", end_time - start_time);
#pragma endregion


#pragma region parallel
	start_time = omp_get_wtime();

	v_par = start;
	sum_par = 0;
#pragma omp parallel for reduction(+:sum_par,v_par)
	for (int i = 0; i < N; i++)
	{
		sum_par = sum_par + f(start + i * step);
		v_par = v_par + step;
	}

	end_time = omp_get_wtime();
	printf("Parallel time = %f\n", end_time - start_time);
#pragma endregion


	return 0;
}