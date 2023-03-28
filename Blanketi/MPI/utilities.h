#pragma once
#include <stdio.h>
#include <stdlib.h>

void initialize_static(int* a, int size, int value)
{
	for (int i = 0; i < size; i++)
		a[i] = value == 0 ? 0 : i;
}

int initialize_dynamic(int*& a, int size, int value)
{
	a = (int*)malloc(size * sizeof(int));
	if (a == nullptr)
		return -1;
	for (int i = 0; i < size; i++)
		a[i] = value == 0 ? 0 : i;
	return 0;
}

void print_vector(const char* name, int* a, int size)
{
	printf("%s: ", name);
	for (int i = 0; i < size; i++)
		printf("%d ", a[i]);
	printf("\n\n");
}

void print_matrix(const char* name, int* a, int rows, int cols)
{
	printf("%s:\n", name);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
			printf("%d\t", a[i * cols + j]);
		printf("\n");
	}
	printf("\n");
}
