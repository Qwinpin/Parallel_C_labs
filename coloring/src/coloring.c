/*
 ============================================================================
 Name        : coloring.c
 Author      : qwinpin
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void test();
void print_graph(int **a, int size){
	printf("\nGraph\n");
	for (int i = 0; i < size && i < 5; i++){
		for (int j = 0; j < size && j < 5; j++){
			printf("%d ", a[i][j]);
		}
		printf("\n");
	}
}


void print_color(int *vert, int size){
	printf("Graph color\n");
	for (int i = 0; i < size && i < 10; i++){
		printf("%d, ", vert[i]);
	}
	printf("\n");
}


void color(int **a, int *vert, int k){
	int i;
	vert[k] = 1;
	for (i = 0; i < k; i++){
		if (a[i][k] != 0 && vert[k] == vert[i]){
			vert[k] = vert[i] + 1;
		}
	}
}


void coloring(int **a, int *vert, int size){
	int i;
	for (i = 0; i < size; i++){
		color(a, vert, i);
	}
}


int main() {
	int evaluate;
	printf("Evaluate test or not? 1/0\n");
	scanf("%d", &evaluate);
	if (evaluate){
		test();
		return 0;
	}
	int size;
	int tmp;
	printf("Enter graph size\n");
	scanf("%d", &size);
	int *vert = (int*) malloc(size * sizeof(int));
	int **a = (int**) malloc(size * sizeof(int*));
	for (int i = 0; i < size; i++){
		a[i] = (int*) malloc(size * sizeof(int));
	}

	srand(time(NULL));

	printf("Start coloring");

	for (int i = 0; i < size; i++){
		a[i][i] = 0;
		for (int j = i + 1; j < size; j++){
			tmp = rand() % 2;
			a[i][j] = tmp;
			a[j][i] = tmp;
		}
	}

	for (int i = 0; i < size; i++){
		vert[i] = 0;
	}

	clock_t start = clock(), diff;
	coloring(a, vert, size);
	print_graph(a, size);
	print_color(vert, size);
	for (int i = 0; i < size; i++){
		free(a[i]);
	}
	free(a);
	diff = clock() - start;
	printf("It took %li sec %li milisec", diff / CLOCKS_PER_SEC, diff * 1000 / CLOCKS_PER_SEC % 1000);
	return 0;
}

void test(){
	int size = 1000;
	int size_step = 1000;
	int size_max = 15000;
	int tmp;
	srand(time(NULL));
	FILE *f = fopen("color_process.txt", "w");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}
	int *vert = (int*) malloc(size_max * sizeof(int));
	int **a = (int**) malloc(size_max * sizeof(int*));
	for (int i = 0; i < size_max; i++){
		a[i] = (int*) malloc(size_max * sizeof(int));
	}

	srand(time(NULL));

	printf("Start coloring");

	for (int i = 0; i < size_max; i++){
		a[i][i] = 0;
		for (int j = i + 1; j < size_max; j++){
			tmp = rand() % 2;
			a[i][j] = tmp;
			a[j][i] = tmp;
		}
	}

	for (int i = 0; i < size_max; i++){
		vert[i] = 0;
	}

	size = 1000;
	while (size <= size_max){
		for (int i = 0; i < size_max; i++){
			vert[i] = 0;
		}
		printf("Size %d\n", size);

		clock_t start = clock(), diff;
		coloring(a, vert, size);
		diff = clock() - start;

		fprintf(f, "%f, ", (double)diff / CLOCKS_PER_SEC * 1000);

		size = size + size_step;
	}
	for (int i = 0; i < size_max; i++){
		free(a[i]);
	}
	free(a);
}
