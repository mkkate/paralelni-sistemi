/*
br.procesa = bilo koji broj
Anxn, gde je n broj procesa koji su deljivi sa 3
*/
#define MCW MPI_COMM_WORLD
#define V 45
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char** argv)
{
	int rank, size, n = 0, rank_comm1, size_comm1, local_sum = 0;
	int* A, * local_a;

	MPI_Comm comm1;
	MPI_Status st;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	MPI_Comm_split(MCW, rank % 3 == 0, rank / 3, &comm1);
	MPI_Comm_rank(comm1, &rank_comm1);
	MPI_Comm_size(comm1, &size_comm1);

	for (int i = 0; i < size; i++)
		if (i % 3 == 0)
			n++;

	A = (int*)malloc(n * n * sizeof(int));
	local_a = (int*)malloc(n * sizeof(int));
	if (A == nullptr || local_a == nullptr)
		return -1;

	if (rank % 3 == 0)
	{
		// inicijalizacija matrice 
		for (int i = 0; i < n * n; i++)
			A[i] = i;

		// slanje po jedne vrste matrice svim procesima iz comm1
		/*if (rank_comm1 == 0)
		{
			for (int i = 0; i < n; i++)
				local_a[i] = A[i];

			for (int i = 1; i < size_comm1; i++)
				MPI_Send(&A[n * i], n, MPI_INT, i, 0, comm1);
		}
		else
			MPI_Recv(local_a, n, MPI_INT, 0, 0, comm1, &st);
		*/
		// slanje vrste sa Scatter umesto Send i Recv
		MPI_Scatter(A, n, MPI_INT, local_a, n, MPI_INT, 0, comm1);

		for (int i = 0; i < n; i++)
			local_sum += local_a[i];

		if (local_sum < V)
			printf("MCW: P[%d], comm1: P[%d], suma = %d\n", rank, rank_comm1, local_sum);
	}

	MPI_Finalize();

	return 0;
}