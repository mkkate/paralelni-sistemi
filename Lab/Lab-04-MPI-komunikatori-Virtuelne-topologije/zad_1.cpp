//br. procesa = 9, matrica A cisto ilustrativno
#define MCW MPI_COMM_WORLD
#define N 3
#include <mpi.h>
#include <stdio.h>
#include <math.h>

int main(int argc, char** argv)
{
	int rank, size, q, row_rank, col_rank, diag_rank, message;
	int A[N][N];
	int irow, jcol;

	MPI_Comm row_comm, col_comm, diag_comm;
	MPI_Status st;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	if (rank == 0)
	{
		printf("MCW -- A:\n");
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				A[i][j] = i * N + j;
				printf("%d\t", A[i][j]);
			}
			printf("\n");
		}
	}

	q = sqrt(size);

	irow = rank / q;
	jcol = rank % q;

	MPI_Comm_split(MCW, irow, jcol, &row_comm);
	MPI_Comm_rank(row_comm, &row_rank);

	MPI_Comm_split(MCW, jcol, irow, &col_comm);
	MPI_Comm_rank(col_comm, &col_rank);

	MPI_Comm_split(MCW, row_rank == col_rank, jcol, &diag_comm);
	MPI_Comm_rank(diag_comm, &diag_rank);

	if (row_rank == col_rank)
	{
		message = 0;
		if (diag_rank == 0)
			for (int i = 1; i < q; i++)
				MPI_Send(&i, 1, MPI_INT, i, 0, diag_comm);
		else
			MPI_Recv(&message, 1, MPI_INT, 0, 0, diag_comm, &st);

		printf("P[%d]: poruka = %d, novi id = %d, stari id = %d,%d\n", rank, message, diag_rank, irow, jcol);
	}

	MPI_Finalize();

	return 0;
}