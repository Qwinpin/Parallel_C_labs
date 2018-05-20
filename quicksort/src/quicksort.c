/*
 ============================================================================
 Name        : quicksort.c
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


void quickSort_main(int* a, int size, int threads);
void quickSort(int* a, int left, int right, int tail);

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int main(){
	int size;
	printf("Enter array size\n");
	scanf("%d", &size);
	int mp;
	printf("Use parallel/qsort? 0/1\n");
	scanf("%d", &mp);
	int threads;
	printf("Enter threads number\n");
	scanf("%d", &threads);

	int* a = (int*)malloc(size * sizeof(int));

	int i;
	srand(time(NULL));

	for (i = 0; i < size; i++){
		a[i] = rand() % 10;
	}

	for (i = 0; i <= 10 && i < size; i++){
		printf("%d ", a[i]);
	}
	printf("\n");

	clock_t start = clock(), diff;
	if (mp == 0){
		printf("Parallel\n");
		quickSort_main(a, size, threads);
	} else{
		printf("Library\n");
		qsort(a, size, sizeof(int), cmpfunc);
	}
	diff = clock() - start;

	for (i = 0; i <= 10 && i < size; i++){
		printf("%d ", a[i]);
	}
	printf("\n");
	printf("It took %li sec %li milisec", diff / CLOCKS_PER_SEC, diff * 1000 / CLOCKS_PER_SEC % 1000);
	printf("\n%f\n", (double)diff / CLOCKS_PER_SEC * 1000);

	for (i = 1; i < size - 1; i++){
		if (a[i] < a[i-1]){
			printf("\nbad ");
			printf("%d - %d, %d - %d\n", i, a[i], i-1, a[i-1]);
			//break;
		}
	}

	free(a);
	return 0;
}


int test(){
	int size = 1000;
	int* a;
	int threads = 17;
	int size_step = 100000;
	int size_max = 10000000;

	srand(time(NULL));
	FILE *f = fopen("quick_process.txt", "w");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	a = (int*)malloc(size * sizeof(int));
	for (int i = 1; i < threads; i++){
		printf("Threads num %d\n", i);
		fprintf(f, "\nThreads_num: %d \n", i);
		size = 1000;
		while (size < size_max){
			printf("Size %d\n", size);
			for (int j = 0; j < size; j++){
				a[j] = rand() % 10;
			}

			clock_t start = clock(), diff;
			quickSort_main(a, size, i);
			diff = clock() - start;

			fprintf(f, "%f, ", (double)diff / CLOCKS_PER_SEC * 1000);

			size = size + size_step;
			a = (int*)realloc(a, size * sizeof(int));
		}
	}
	free(a);
	return 0;
}

void quickSort_main(int* a, int size, int threads) {
	int tail = 1000;

	#pragma omp parallel num_threads(threads)
	{
		#pragma omp single nowait
		{
			quickSort(a, 0, size - 1, tail);
		}
	}
}

void quickSort(int* a, int left, int right, int tail){
	int tmp;
	int i_left = left;
	int i_right = right;
	int pivot = a[(left + right) / 2];

	while (i_left <= i_right){
		while (a[i_left] < pivot) i_left++;
		while (a[i_right] > pivot) i_right--;

		if (i_left <= i_right){
			tmp = a[i_left];
			a[i_left] = a[i_right];
			a[i_right] = tmp;
			i_left++;
			i_right--;
		}
	}

	if ((right - left) < tail){
		if (left < i_right){
			quickSort(a, left, i_right, tail);
		}
		if (i_left < right){
			quickSort(a, i_left, right, tail);
		}
	} else{
		#pragma omp task
			{quickSort(a, left, i_right, tail);}
		#pragma omp task
			{quickSort(a, i_left, right, tail);}
	}
}
