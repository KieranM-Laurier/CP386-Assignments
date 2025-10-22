#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
/**
 * -------------------------------------
 * @file  a2q1.c
 * file description
 * -------------------------------------
 * @Kieran M, 169048254, moch8254@mylaurier.ca
 *
 * @version 2025-10-05
 *
 * --------v-----------------------------
 */
/**
 * [a b c]   [1 2 3]
 * [d e f] X [4 5 6]
 * [h i j]   [7 8 9]
 *
 * [a1b4c7 a2b5c8 a3b6c9]
 * [d1e4f7 d2e5f8 d3e6f9]
 * [h1i4j7 h2i5j8 h3i6j9]
 * row of left X columns of right
 */
typedef struct {
	int *row;
	int cols;
	int top;
	int **array;

} threadInfo;

void* matrixMath(void *arg) {
	threadInfo *info = (threadInfo*) arg;

	// Result row: one value per column of B
	int temp[info->cols];
	for (int j = 0; j < info->cols; j++) {
		temp[j] = 0;
		for (int k = 0; k < info->top; k++) {
			temp[j] += info->row[k] * info->array[k][j];
		}
	}

	// Print the resulting row
	for (int j = 0; j < info->cols; j++) {
		printf("%d ", temp[j]);
	}
	printf("\n");

	return NULL;
}

void fillMatrix(int ***matrix, int row, int col) {
	*matrix = (int**) malloc(row * sizeof(int*));

	for (int i = 0; i < row; i++) {
		(*matrix)[i] = (int*) malloc(col * sizeof(int));
		for (int j = 0; j < col; j++) {
			scanf("%d", &((*matrix)[i][j]));
		}
	}
}
int main(int argc, char *argv[]) {
	//get dimensions of both martix
	int rowA, colA, rowB, colB;
	printf("Enter dimensions of matrix A (rows AND columns):");
	scanf("%d %d", &rowA, &colA);
	while (rowA < 1 || colA < 1) { //make sure the matrix has real dimensions
		printf("Dimensions of the matrix must be greater than 0\n");
		printf("Enter dimensions of matrix A (rows AND columns):");
		scanf("%d %d", &rowA, &colA);
	}

	printf("Enter dimensions of matrix B (rows AND columns):");
	scanf("%d %d", &rowB, &colB);
	while (rowB < 1 || colB < 1) { //make sure the matrix has real dimensions
		printf("Dimensions of the matrix must be greater than 0\n");
		printf("Enter dimensions of matrix B (rows AND columns):");
		scanf("%d %d", &rowB, &colB);
	}

	int **matrixA = NULL;
	printf("Enter values of Matrix A\n");
	fillMatrix(&matrixA, rowA, colA);

	int **matrixB = NULL;
	printf("Enter values of Matrix B\n");
	fillMatrix(&matrixB, rowB, colB);

	int num_threads = colB; // This would be determined at runtime
	pthread_t *threads = (pthread_t*) malloc(num_threads * sizeof(pthread_t));
	threadInfo *matrices[num_threads];
	printf("Resulting Matrix C:\n");
	for (int i = 0; i < num_threads; i++) {

		matrices[i] = (threadInfo*) malloc(sizeof(threadInfo));
		matrices[i]->row = matrixA[i]; //gets the rows from matrix A in top to bottom
		matrices[i]->top = colA;
		matrices[i]->cols = colB;
		matrices[i]->array = matrixB;

		pthread_create(&threads[i], NULL, matrixMath, (void*) matrices[i]);
		pthread_join(threads[i], NULL);

	}
	return 0;
}

