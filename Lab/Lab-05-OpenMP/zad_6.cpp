#define M 350
#define N 350
#include <omp.h>
#include <stdio.h>

int main()
{
	// zavisnost u iteracijama po indeksu i
	int i, j, a[M][N]{}, a_seq[M][N]{};
	double start, end;

	for (i = 0; i < M; i++)
		for (j = 0; j < N; j++)
			a_seq[i][j] = a[i][j] = i * N + j;

#pragma region sequentially
	start = omp_get_wtime();
	for (i = 1; i < M; i++)
		for (j = 0; j < N; j++)
			a_seq[i][j] = 2 * a[i - 1][j];
	end = omp_get_wtime();
	printf("Seq: time = %f sec\n", end - start);
#pragma endregion

	int nthreads = omp_get_num_procs();
	omp_set_num_threads(nthreads);

#pragma region parallel
	start = omp_get_wtime();
#pragma omp parallel for
	for (j = 0; j < N; j++)
		for (int i = 1; i < M; i++)
			a[i][j] = 2 * a[i - 1][j];
	end = omp_get_wtime();
	printf("Par: time = %f sec\n", end - start);
#pragma endregion

	return 0;
}