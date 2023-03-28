// m = 8, p = 16 => q = 4
#define MCW MPI_COMM_WORLD
#define m 8
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char** argv)
{
	int rank, p, A[m][m], b[m]{}, c[m], q;
	int row_rank, col_rank, irow, jcol;
	int* local_A, ma, * local_b, * blocklengths, * displacements, * local_c, * local_part_c;
	struct {
		int value;
		int rank;
	}max, local_max;

	MPI_Status status;
	MPI_Comm row_comm, col_comm;
	MPI_Datatype type_a, type_b, type_c;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &p);

	q = sqrt(p);
	ma = m / q;
	local_A = (int*)malloc(ma * ma * sizeof(int));
	local_b = (int*)malloc(ma * sizeof(int));
	local_c = (int*)malloc(ma * sizeof(int));
	local_part_c = (int*)malloc(ma * sizeof(int));
	blocklengths = (int*)malloc(ma * ma * sizeof(int));
	displacements = (int*)malloc(ma * ma * sizeof(int));

	//if (local_b == nullptr)
		//return -1;

#pragma region inicijalizacija
	if (rank == 0)
	{
		for (int i = 0; i < m; i++)
		{
			b[i] = i;
			c[i] = 0;
			for (int j = 0; j < m; j++)
				A[i][j] = i * m + j;
		}
		printf("A:\n");
		for (int i = 0; i < m; i++)
		{
			for (int j = 0; j < m; j++)
				printf("%d\t", A[i][j]);
			printf("\n");
		}
		printf("B: ");
		for (int i = 0; i < m; i++)
			printf("%d\t", b[i]);
		printf("\n");
	}
#pragma endregion

#pragma region grid_procesa
	// uredjivanje procesa kao matrica qxq
	MPI_Comm_split(MCW, rank / q, rank % q, &row_comm);
	MPI_Comm_split(MCW, rank % q, rank / q, &col_comm);
	MPI_Comm_rank(row_comm, &row_rank);
	MPI_Comm_rank(col_comm, &col_rank);
#pragma endregion

#pragma region distribucija_A
	for (int i = 0; i < ma; i++)
		for (int j = 0; j < ma; j++)
		{
			blocklengths[i * ma + j] = 1;
			displacements[i * ma + j] = i * q * ma + j * q;
		}
	MPI_Type_indexed(ma * ma, blocklengths, displacements, MPI_INT, &type_a);
	MPI_Type_commit(&type_a);
	//MPI_Type_create_resized(vector,0,)

	if (rank == 0)
	{
		//salje sebi
		for (int i = 0; i < ma; i++)
			for (int j = 0; j < ma; j++)
				local_A[i * ma + j] = A[i * q][j * q];

		// svakom procesu odjednom se salje blok matrice A
		for (int i = 1; i < p; i++)
			MPI_Send(&A[i / q][i % q], 1, type_a, i, 0, MCW);
	}
	else
		MPI_Recv(&local_A[0], ma * ma, MPI_INT, 0, 0, MCW, &status);
#pragma endregion

#pragma region distribucija_b
	// salje "root" procesima iz col_comm => P0, P1, P2, P3
	if (col_rank == 0)
	{
		MPI_Type_vector(ma, 1, q, MPI_INT, &type_b);
		MPI_Type_create_resized(type_b, 0, 1 * sizeof(int), &type_b);
		MPI_Type_commit(&type_b);

		MPI_Scatter(b, 1, type_b, local_b, ma, MPI_INT, 0, row_comm);
	}

	// sada ti "root" procesi broadcast-uju svoje local_b do ostalih procesa sa istim col_rank
	// P0 do P4, P8, P12;	P1 do P5, P9, P13;	P2 do P6, P10, P14	P3 do P7, P11, P15
	MPI_Bcast(local_b, ma, MPI_INT, 0, col_comm);
#pragma endregion

#pragma region izracunavanja
	local_max.value = INT_MIN;
	// u local_part_c od P0 je P0[a00 a01 \n a40 a44] * [b0 b4]
	// u local_part_c od P4 je P4[a10 a11 \n a50 a54] * [b0 b4] ... itd. za P8 i P12
	// u local_part_c od P1 je P1[a01 a05 \n a11 a45] * [b1 b5] ... itd. za P5, P9 i P13
	// ... itd za local_part_c od P2,6,10,14 * [b2 b6] i P3,7,11,15 * [b3 b7]
	for (int i = 0; i < ma; i++)
	{
		local_part_c[i] = 0;
		for (int j = 0; j < ma; j++)
		{
			int indeks = i * ma + j;
			local_part_c[i] += local_A[indeks] * local_b[j];
			if (local_max.value < local_A[indeks])
				local_max.value = local_A[indeks];
		}
	}

	// trazi se najveci element u A, pa zato MCW
	MPI_Reduce(&local_max, &max, 1, MPI_2INT, MPI_MAXLOC, 0, MCW);
	MPI_Bcast(&max, 1, MPI_2INT, 0, MCW);

	// local_c je
	// rezultat u procesu sa max element u A, tj. u njegovom row_rank 
	MPI_Reduce(local_part_c, local_c, ma, MPI_INT, MPI_SUM, max.rank % q, row_comm);
#pragma endregion

#pragma region prikaz
	// svi local_c-ovi iz max row_rank se skupljaju u konacno c, i to sa Gather u col_comm
	// samo za procese iz odg. row_rank
	if (row_rank == max.rank % q)
	{
		MPI_Type_vector(ma, 1, q, MPI_INT, &type_c);
		MPI_Type_create_resized(type_c, 0, 1 * sizeof(int), &type_c);
		MPI_Type_commit(&type_c);

		MPI_Gather(local_c, ma, MPI_INT, c, 1, type_c, max.rank / q, col_comm);

		// prikaz iz procesa sa max el. od A
		if (rank == max.rank)
		{
			printf("C: ");
			for (int i = 0; i < m; i++)
				printf("%d\t", c[i]);
			printf("\nMax_rank = %d, max_value = %d\n", max.rank, max.value);
		}
	}
#pragma endregion

	MPI_Finalize();

	return 0;
}