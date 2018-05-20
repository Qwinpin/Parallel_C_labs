/*
 ============================================================================
 Name        : merge.c
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

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

void merge(int *a, int *b, int start, int mid, int end){
  int i_start, i_mid, i;

  for(i_start = start, i_mid = mid + 1, i = start; i_start <= mid && i_mid <= end; i++){
    if(a[i_start] <= a[i_mid]){
      b[i] = a[i_start++];
    } else{
      b[i] = a[i_mid++];
    }
  }

  while(i_start <= mid){
    b[i++] = a[i_start++];
  }

  while(i_mid <= end){
    b[i++] = a[i_mid++];
  }

  for(i = start; i <= end; i++){
    a[i] = b[i];
  }
}


void splitter(int *a, int *b, int start, int end){
	if(start < end){
		int mid = (start + end) / 2;
		splitter(a, b, start, mid);
		splitter(a, b, mid + 1, end);
		merge(a, b, start, mid, end);
	}
}

int main(){
	printf("hello\n");

	int max;
	printf("Enter array size\n");
	scanf("%d", &max);

	int mp;
	printf("Use single/parallel/qsort? 0/1/2\n");
	scanf("%d", &mp);

	int split = 4;
	int st;
	int st2;

	int* a = (int*)malloc(max * sizeof(int));
	int* b = (int*)malloc(max * sizeof(int));

	int i, j;
	srand(time(NULL));

	for (i = 0; i < max; i++){
		a[i] = rand();
	}

	for (i = 0; i <= 10 && i < max; i++){
		printf("%d ", a[i]);
	}
	printf("\n");

	clock_t start = clock(), diff;
	if (mp == 1){
		printf("Parallel");
		#pragma omp parallel for ordered
			for (j = 0; j < split; j++){
				st = j * (max / split);
				st2 = (st + max / split) - 1;
				#pragma omp parallel
				splitter(a, b, st, st2);
			}
		st = max / split;
		for (i = 0; i < split-1; i++){
			st = st + max / split;
			merge(a, b, 0, st - max / split - 1, st - 1);
		}
	}
	if (mp == 0){
		printf("Single");
		splitter(a, b, 0, max-1);
	}
	if (mp == 2){
		printf("Library");
		qsort(a, max, sizeof(int), cmpfunc);
	}

	diff = clock() - start;

	for (i = 1; i < max - 1; i++){
		if (a[i] < a[i-1]){
			printf("\nbad ");
			printf("%d - %d, %d - %d\n", i, a[i], i-1, a[i-1]);
			//break;
		}
	}

//	for (i = 0; i < max; i++){
//		printf("%d, ", a[i]);
//	}
	printf("\n");
	printf("It took %li sec %li milisec", diff / CLOCKS_PER_SEC, diff * 1000 / CLOCKS_PER_SEC % 1000);
	printf("\n%f\n", (double)diff / CLOCKS_PER_SEC * 1000);
	free(a);
	free(b);
	return 0;
}

int main2(){
	int size = 1000;
	int size_max = 2000000;
	int size_step = 100000;
	int i, j;
	int* a;
	int* b;
	int st, st2;

	srand(time(NULL));
	FILE *f = fopen("single_process.txt", "w");
	if (f == NULL)
	{
	    printf("Error opening file!\n");
	    exit(1);
	}
	a = (int*)malloc(size * sizeof(int));
	b = (int*)malloc(size * sizeof(int));
	while (size < size_max){
		for (i = 0; i < size; i++){
			a[i] = rand();
		}
		clock_t start = clock(), diff;
		splitter(a, b, 0, size-1);
		diff = clock() - start;
		fprintf(f, "%f, ", (double)(diff) / CLOCKS_PER_SEC * 1000);
		size = size + size_step;
		a = (int*)realloc(a, size * sizeof(int));
		b = (int*)realloc(b, size * sizeof(int));
	}
	fclose(f);

	FILE *f2 = fopen("parallel_process.txt", "w");
	if (f2 == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	int split = 10;
	while (split <= 100){
		printf("Splits num: %d\n", split);
		size = 1000;
		a = (int*)realloc(a, size * sizeof(int));
		b = (int*)realloc(b, size * sizeof(int));
		fprintf(f2, "\nSplits - %d:\n ", split);
		while (size < size_max){
			printf("Size %d\n", size);
			for (j = 0; j < size; j++){
				a[j] = rand();
			}
			clock_t start = clock(), diff;
			#pragma omp parallel for
				for (j = 0; j < split; j++){
					st = j * (size / split);
					st2 = (st + size / split) - 1;
					#pragma omp parallel
					splitter(a, b, st, st2);
				}

			int st = size / split;
			for (j = 0; j < split - 1; j++){
				st = st + size / split;
				merge(a, b, 0, st - size / split - 1, st - 1);
			}
			diff = clock() - start;
			printf("%li", diff);
			fprintf(f2, "%f, ", (double)(diff) / CLOCKS_PER_SEC * 1000);
			size = size + size_step;
			a = (int*)realloc(a, size * sizeof(int));
			b = (int*)realloc(b, size * sizeof(int));
		}
		split = split + 10;
	}
	fclose(f2);

	FILE *f3 = fopen("qsort_process.txt", "w");
	if (f3 == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	size = 100;
	a = (int*)realloc(a, size * sizeof(int));
	while (size < size_max){
		for (i = 0; i < size; i++){
			a[i] = rand();
		}
		clock_t start = clock(), diff;
		qsort(a, size, sizeof(int), cmpfunc);
		diff = clock() - start;
		fprintf(f3, "%f, ", (double)(diff) / CLOCKS_PER_SEC * 1000);

		size = size + size_step;
		a = (int*)realloc(a, size * sizeof(int));
	}
	fclose(f3);
	free(a);
	free(b);
	return 0;
}
