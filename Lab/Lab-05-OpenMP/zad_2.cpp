#define N 3000
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	int* a_seq, * b_seq, result_seq = 0;
	int* a_no_red, * b_no_red, result_no_red = 0;
	int* a_red, * b_red, result_red = 0;

	double start_seq, start_no_red, start_red, end_seq, end_no_red, end_red;

	a_seq = (int*)malloc(N * sizeof(int));
	a_no_red = (int*)malloc(N * sizeof(int));
	a_red = (int*)malloc(N * sizeof(int));
	b_seq = (int*)malloc(N * sizeof(int));
	b_no_red = (int*)malloc(N * sizeof(int));
	b_red = (int*)malloc(N * sizeof(int));

	if (a_seq == nullptr || a_no_red == nullptr || a_red == nullptr ||
		b_seq == nullptr || b_no_red == nullptr || b_red == nullptr)
		return -1;

	for (int i = 0; i < N; i++)
	{
		a_red[i] = a_no_red[i] = a_seq[i] = i + 1;
		b_red[i] = b_no_red[i] = b_seq[i] = i + 3;
	}

#pragma region sekvencijalno
	start_seq = omp_get_wtime();
	for (int i = 0; i < N; i++)
		result_seq += a_seq[i] * b_seq[i];
	end_seq = omp_get_wtime();

	printf("Sekvencijalno: result_seq = %d, za vreme = %f sec\n", result_seq, (end_seq - start_seq));

#pragma endregion sekvencijalno

	int nthreads = omp_get_num_procs();
	omp_set_num_threads(nthreads);
	printf("Broj jezgara u sistemu je %d\n", nthreads);

#pragma region paralelno_bez_reduction
	int i;
	start_no_red = omp_get_wtime();
#pragma omp parallel for
	for (i = 0; i < N; i++)
	{
		//printf("Nit no_red %d mnozi indekse %d\n", omp_get_thread_num(), i);
		result_no_red += a_no_red[i] * b_no_red[i];
	}
	end_no_red = omp_get_wtime();
	printf("Bez redukcije: result_no_red = %d, za vreme = %f sec\n", result_no_red, (end_no_red - start_no_red));

#pragma endregion paralelno_bez_reduction

#pragma region paralelno_sa_reduction
	int j;
	start_red = omp_get_wtime();
#pragma omp parallel for reduction(+:result_red)
	for (j = 0; j < N; j++)
	{
		//printf("Nit red %d mnozi indekse %d\n", omp_get_thread_num(), j);
		result_red += a_red[j] * b_red[j];
	}
	end_red = omp_get_wtime();
	printf("Sa redukcijom: result_red = %d, za vreme = %f sec\n", result_red, (end_red - start_red));
#pragma endregion paralelno_sa_reduction

	return 0;
}