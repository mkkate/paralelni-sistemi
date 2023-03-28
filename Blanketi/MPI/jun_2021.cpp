// k = 8, p = 16 => q = 4, s = 2
#define MCW MPI_COMM_WORLD
#define k 8
#include <mpi.h>
#include <math.h>
#include "utilities.h"

int main(int argc, char** argv)
{
	int rank, p, q, s, A[k][k]{}, b[k]{}, c[k]{};
	int* local_A, * local_b, * local_c, * local_part_c;
	int count, * displacements, * blocklengths;
	int row_rank, col_rank;
	struct {
		int value;
		int rank;
	}local_min, min;

	MPI_Status status;
	MPI_Comm row_comm, col_comm;
	MPI_Datatype type_a, type_b;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &p);

	q = sqrt(p);
	s = k / q;

#pragma region initializations
	if (rank == 0)
	{
		initialize_static(*A, k * k, 1);
		initialize_static(b, k, 1);
		print_matrix("A", *A, k, k);
		print_vector("b", b, k);
	}
#pragma endregion


#pragma region A_distribution
	initialize_dynamic(local_A, s, 0);
	initialize_dynamic(blocklengths, s, 0);
	initialize_dynamic(displacements, s, 0);

	MPI_Type_vector(s, s, k * q, MPI_INT, &type_a);
	MPI_Type_create_resized(type_a, 0, s * sizeof(int), &type_a);
	MPI_Type_commit(&type_a);

#pragma region a)
	if (rank == 0)
	{
		// samom sebi prvi blok
		for (int i = 0; i < s; i++)
			for (int j = 0; j < s; j++)
				local_A[i * s + j] = A[i * q][j * q];

		// ostalima ostatak
		for (int i = 1; i < p; i++)
			MPI_Send(&A[i / q][i % q * 2], 1, type_a, i, 0, MCW);
	}
	else
		MPI_Recv(local_A, s * s, MPI_INT, 0, 0, MCW, &status);
#pragma endregion a)

#pragma region b)
	//MPI_Scatter(A, 1, type_a, local_A, s * s, MPI_INT, 0, MCW);
#pragma endregion b)

#pragma endregion

#pragma region process_matrix
	MPI_Comm_split(MCW, rank / q, rank % q, &row_comm);
	MPI_Comm_rank(row_comm, &row_rank);
	MPI_Comm_split(MCW, rank % q, rank / q, &col_comm);
	MPI_Comm_rank(col_comm, &col_rank);
#pragma endregion

#pragma b_distribution
	initialize_dynamic(local_b, s, 0);
	// saljem u P0, P1, P2, P3 u row_rank 0
	if (row_rank == 0)
	{
		// nije potreban tip jer su uzastopne pozicije
		//MPI_Type_contiguous(2, MPI_INT, &type_b);
		//MPI_Type_commit(&type_b);
		//MPI_Scatter(b, 1, type_b, local_b, s, MPI_INT, 0, row_comm);

		MPI_Scatter(b, s, MPI_INT, local_b, s, MPI_INT, 0, row_comm);
	}

	// sada oni salju ostalim procesima sa istim col_rank
	MPI_Bcast(local_b, s, MPI_INT, 0, col_comm);
#pragma endregion

#pragma region multiplication
	local_min.value = INT_MAX;
	initialize_dynamic(local_part_c, s, 0);
	initialize_dynamic(local_c, s, 0);
	for (int i = 0; i < s; i++)
		for (int j = 0; j < s; j++)
		{
			int indeks = i * s + j;
			local_part_c[j] += local_A[indeks] * local_b[j];
			if (local_min.value > local_A[indeks])
				local_min.value = local_A[indeks];
		}
	// local_min je minimalni od 4 elemenata iz local_A, pa iz tih lokalnih minimuma trazimo 
	// min cele matrice A, zato je MCW
	MPI_Reduce(&local_min, &min, 1, MPI_2INT, MPI_MINLOC, 0, MCW);
	MPI_Bcast(&min, 1, MPI_2INT, 0, MCW);	// cisto da svi porcesi imaju info o min

	//MPI_Reduce(local_part_c, local_c, s, MPI_INT, MPI_SUM, min.rank / q, row_comm);
	MPI_Reduce(local_part_c, local_c, s, MPI_INT, MPI_SUM, min.rank % q, col_comm);
#pragma endregion

#pragma region display_result
	if (row_rank == min.rank / q)
	{
		initialize_static(c, k, 0);

		MPI_Gather(local_c, 2, MPI_INT, c, k, MPI_INT, row_rank, col_comm);
		if (rank == min.rank)
		{
			print_vector("c", c, k);
			printf("Min_rank = %d, min_value = %d\n", min.rank, min.value);
		}
	}

#pragma endregion

	MPI_Finalize();

	return 0;
}