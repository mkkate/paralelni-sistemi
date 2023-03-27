#pragma once

#define NTHREADS 4
#include <omp.h>
#include <stdio.h>
#include "initializations.h"
#include <stdlib.h>

int initialize_vector(int*& vector, int size)
{
	vector = (int*)malloc(size * sizeof(int));
	if (vector == nullptr)
		return -1;
	for (int i = 0; i < size; i++)
		vector[i] = i;
	return 0;
}

