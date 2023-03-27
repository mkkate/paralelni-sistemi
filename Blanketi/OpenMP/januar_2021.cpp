#define N 5000000
#define NTHREADS 4
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	int t, i, * a_seq, * b, * c;
	int* a_par, * a2_par;

	double start, end;

	a_seq = (int*)calloc(N, sizeof(int));
	b = (int*)malloc(N * sizeof(int));
	c = (int*)malloc(N * sizeof(int));
	a_par = (int*)calloc(N, sizeof(int));
	a2_par = (int*)malloc(N * sizeof(int));

	if (a_seq == nullptr || b == nullptr || c == nullptr)
		return -1;
	if (a_par == nullptr || a2_par == nullptr)
		return -1;

	omp_set_num_threads(NTHREADS);

#pragma region sequentially
	start = omp_get_wtime();
	t = 1;
	for (i = 0; i < N - 1; i++)
	{
		a_seq[i] = a_seq[i + 1] + b[i] * c[i];
		t = t * a_seq[i];
	}
	end = omp_get_wtime();
	/*printf("t_seq = %d\na_seq = ", t);
	for (int i = 0; i < N; i++)
		printf("%d ", a_seq[i]);*/
	printf("\nSeq time = %f\n", end - start);
#pragma endregion

#pragma region parallel
	start = omp_get_wtime();
	t = 1;
#pragma omp parallel for
	for (i = 0; i < N - 1; i++)
		a2_par[i] = a_par[i + 1];
#pragma omp parallel for reduction(*:t)
	for (i = 0; i < N - 1; i++)
	{
		a_par[i] = a2_par[i] + b[i] * c[i];
		t = t * a_par[i];
	}
	end = omp_get_wtime();
	/*printf("t_par = %d\na_par = ", t);
	for (int i = 0; i < N; i++)
		printf("%d ", a_par[i]);*/
	printf("\nPar time = %f\n", end - start);
#pragma endregion

	return 0;
}