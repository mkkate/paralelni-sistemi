// br. procesa = P = 2
#define MCW MPI_COMM_WORLD
#define M 4
#define N 3
#define K 4
#define P 2
#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	int rank, size;
	int A[M][N]{}, B[N][K]{}, C[M][K]{};
	int local_a[M][N], local_b[N][K / P], local_c[M][K / P];

	MPI_Datatype col, column_b, column_c;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	if (rank == 0)
	{
		printf("A:\n");
		for (int i = 0; i < M; i++)
		{
			for (int j = 0; j < N; j++)
			{
				A[i][j] = i * N + j + 1;
				printf("%d\t", A[i][j]);
			}
			printf("\n");
		}

		printf("B:\n");
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < K; j++)
			{
				B[i][j] = i * K + j + 1;
				printf("%d\t", B[i][j]);
			}
			printf("\n");
		}
	}

	MPI_Bcast(A, M * N, MPI_INT, 0, MCW);

	MPI_Type_vector(N, K / P, K, MPI_INT, &col);
	MPI_Type_commit(&col);
	MPI_Type_create_resized(col, 0, (K / P) * sizeof(int), &column_b);
	MPI_Type_commit(&column_b);

	MPI_Scatter(B, 1, column_b, local_b, N * (K / P), MPI_INT, 0, MCW);

	if (rank == 0)
	{
		printf("Scatter local_b:\n");
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < K / P; j++)
				printf("%d\t", local_b[i][j]);
			printf("\n");
		}
	}

	for (int i = 0; i < M; i++)
		for (int j = 0; j < K / P; j++)
		{
			local_c[i][j] = 0;
			for (int k = 0; k < N; k++)
				local_c[i][j] += A[i][k] * local_b[k][j];
		}

	// matrica local_c se salje u C, da bi se sklopila konacna C 
	MPI_Type_vector(M, K / P, K, MPI_INT, &col);
	MPI_Type_commit(&col);
	MPI_Type_create_resized(col, 0, (K / P) * sizeof(int), &column_c);
	MPI_Type_commit(&column_c);

	MPI_Gather(local_c, M * (K / P), MPI_INT, C, 1, column_c, 0, MCW);

	if (rank == 0)
	{
		printf("C:\n");
		for (int i = 0; i < M; i++)
		{
			for (int j = 0; j < K; j++)
			{
				printf("%d\t", C[i][j]);
			}
			printf("\n");
		}
	}

	MPI_Finalize();

	return 0;
}