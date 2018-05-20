/*
 ============================================================================
 Name        : deikstra.c
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

#define INF 1000000
#define TRUE 1
#define FALSE 0

void print_graph(int **a, int size);
void test();


int find_single(int **a, int size) {
	int tmp;
	int *v = (int*) malloc(size * sizeof(int));
	int *d = (int*) malloc(size * sizeof(int));

	srand(time(NULL));

	for (int i = 0; i < size; i++){
		v[i] = TRUE;
		d[i] = INF;
	}

	d[0] = 0;
	int minindex = 0;
	int min = 0;
	while (minindex < INF){
		minindex = INF;
		min = INF;

		for (int i = 0; i < size; i++){
			if ((v[i]) && (d[i] < min)){
				min = d[i];
				minindex = i;
			}
		}

		if (minindex != INF){
			for (int i = 0; i < size; i++){
				if (a[minindex][i] > 0){
					tmp = a[minindex][i] + min;
					if (tmp < d[i]){
						d[i] = tmp;
					}
				}
			}
			v[minindex] = FALSE;
		}
	}
	return 0;
}


int find_parallel(int **a, int size, int threads){
	int tmp, step;
	int *minArr = (int*) malloc(threads * sizeof(int));
	int *minInd = (int*) malloc(threads * sizeof(int));
	int i;
	int *v = (int*) malloc(size * sizeof(int));
	int *d = (int*) malloc(size * sizeof(int));

	step = size / threads;

	#pragma omp parallel num_threads(threads)
	{
	#pragma omp parallel for
	for (i = 0; i < size; i++){
		v[i] = TRUE;
		d[i] = INF;
	}
	}

	d[0] = 0;
	int minindex = 0;
	int min = 0;
	#pragma omp parallel num_threads(threads)
		{
		#pragma omp parallel for
		for (i = 0; i < threads; i++){
			minArr[i] = INF;
			minInd[i] = INF;
		}
		}
	while (minindex < INF){
		minindex = INF;
		min = INF;

		// parallel part - split vector into chunks, find min in each, then compare
		#pragma omp parallel num_threads(threads)
		{
		#pragma omp for
		for (int j = 0; j < threads; j++){

			for (int k = j * step; k < (j + 1)*step ; k++){
				if ((v[k]) && (d[k]) < minArr[j]){
					minArr[j] = d[k];
					minInd[j] = k;
				}
			}
		}
		}

		for (int j = 0; j < threads; j++){
			if (minArr[j] < min){
				min = minArr[j];
				minindex = minInd[j];
			}
			minArr[j] = INF;
			minInd[j] = INF;
		}
		// process each vector element in parallel
		if (minindex != INF){
			// #pragma omp parallel num_threads(threads)
			{
			// #pragma omp parallel for
			for (i = 0; i < size; i++){
				if (a[minindex][i] > 0){
					tmp = a[minindex][i] + min;
					if (tmp < d[i]){
						d[i] = tmp;
					}
				}
			}
			v[minindex] = FALSE;
			}
		}
	}
	return 0;
}


void print_graph(int **a, int size){
	printf("\nGraph\n");
	for (int i = 0; i < size && i < 5; i++){
		for (int j = 0; j < size && j < 5; j++){
			printf("%d ", a[i][j]);
		}
		printf("\n");
	}
}


int main(){
	int evaluate;
	printf("Evaluate test or not? 1/0\n");
	scanf("%d", &evaluate);
	if (evaluate){
		test();
		return 0;
	}
	int size;
	printf("Enter graph size\n");
	scanf("%d", &size);
	if (size < 0 || size > 30000){
		size = 10000;
	}
	int th;
	printf("Threads number\n");
	scanf("%d", &th);
	if (th == 0){
		printf("Bye");
	}
	int **a = (int**) malloc(size * sizeof(int*));
	int tmp;

	srand(time(NULL));

	for (int i = 0; i < size; i++){
		a[i] = (int*) malloc(size * sizeof(int));
	}

	for (int i = 0; i < size; i++){
		a[i][i] = 0;
		for (int j = i + 1; j < size; j++){
			tmp = rand() % 10;
			a[i][j] = tmp;
			a[j][i] = tmp;
		}
	}

	clock_t start = clock(), diff;
	if (th == 1){
		find_single(a, size);
	} else{
		find_parallel(a, size, th);
	}
	diff = clock() - start;
	print_graph(a, size);
	printf("It took %li sec %li milisec", diff / CLOCKS_PER_SEC, diff * 1000 / CLOCKS_PER_SEC % 1000);
	for (int i = 0; i < size; i++){
		free(a[i]);
	}
	free(a);

	return 0;
}


void test(){
	int size = 1000;
	int threads = 17;
	int size_step = 1000;
	int size_max = 22000;
	unsigned short int tmp;

	srand(time(NULL));
	FILE *f = fopen("d_process.txt", "w");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	int **a = (int**) malloc(size_max * sizeof(int*));
	for (int i = 0; i < size_max; i++){
		a[i] = (int*) malloc(size_max * sizeof(unsigned short int*));
	}

	for (int i = 0; i < size_max; i++){
		a[i][i] = 0;
		for (int j = i + 1; j < size_max; j++){
			tmp = rand() % 10;
			a[i][j] = tmp;
			a[j][i] = tmp;
		}
	}

	for (int th = 1; th < threads; th++){
		printf("Threads num %d\n", th);
		fprintf(f, "\nThreads_num: %d\n ", th);
		size = 1000;
		while (size <= size_max){
			printf("Size %d\n", size);

			clock_t start = clock(), diff;
			if (th == 1){
				find_single(a, size);
			} else{
				find_parallel(a, size, th);
			}
			diff = clock() - start;

			fprintf(f, "%f, ", (double)diff / CLOCKS_PER_SEC * 1000);

			size = size + size_step;
		}
	}
	for (int i = 0; i < size_max; i++){
		free(a[i]);
	}
	free(a);
}
