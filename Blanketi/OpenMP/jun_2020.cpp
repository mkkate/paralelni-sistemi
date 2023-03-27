#define N 5000
#include "initializations.h"

int main()
{
	int s = 1, z, k = 2, i, j, * a_seq, * a_par, * b;
	double start, end;

	omp_set_num_threads(NTHREADS);

	initialize_vector(a_seq, N);
	initialize_vector(a_par, N);
	initialize_vector(b, s + k * N * N);

#pragma region sequentially
	start = omp_get_wtime();
	z = s;
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
		{
			a_seq[j] += b[z];
			z += k;
		}
	end = omp_get_wtime();
	printf("Seq time = %f\n", end - start);
#pragma endregion

#pragma region parallel
	start = omp_get_wtime();

	z = s;
#pragma omp parallel for reduction(+:z)
	for (i = 0; i < N * N; i++)
	{
#pragma omp critical
		a_par[i % N] += b[s + k * i];
		z += k;
	}

	end = omp_get_wtime();
	printf("Par time = %f\n", end - start);
#pragma endregion

	return 0;
}