/*
zad3. Napisati MPI program koji čita celobrojne (integer) i vrednosti duple tačnosti
(double-precision) sa standardnog ulaza i saopštava to svim ostalim procesima.
*/

#define N 2
#define MCW MPI_COMM_WORLD
#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	int rank, size;
	struct {
		int a;
		double b;
	}value;
	int array_of_blocklengths[N];
	MPI_Aint array_of_displacements[N];
	MPI_Datatype array_of_types[N], newtype;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	array_of_blocklengths[0] = array_of_blocklengths[1] = 1;

	MPI_Get_address(&value.a, &array_of_displacements[0]);
	MPI_Get_address(&value.b, &array_of_displacements[1]);
	array_of_displacements[1] -= array_of_displacements[0];
	array_of_displacements[0] = 0;

	array_of_types[0] = MPI_INT;
	array_of_types[1] = MPI_DOUBLE;

	MPI_Type_create_struct(2, array_of_blocklengths, array_of_displacements, array_of_types, &newtype);
	MPI_Type_commit(&newtype);


	if (rank == 0)
		scanf_s("%d %lf", &value.a, &value.b);

	MPI_Bcast(&value, 1, newtype, 0, MCW);

	printf("P[%d] got %d and %lf\n", rank, value.a, value.b);

	MPI_Finalize();

	return 0;
}