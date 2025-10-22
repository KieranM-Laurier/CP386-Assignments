#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
/**
 * -------------------------------------
 * @file  a1q2b.c
 * file description
 * -------------------------------------
 * @Kieran M, 169048254, moch8254@mylaurier.ca
 *
 * @version 2025-09-30
 *
 * -------------------------------------
 */

void* parallel(void *arg) {

	printf("Parallel\n");

	char *cmd = arg;
	printf("logic now starting: %s\n", cmd);
	fflush(stdout);
	system(cmd);
	free(cmd);
	return NULL;
}
int main(int argc, char *argv[]) {
	FILE *fptr;
	fptr = fopen(argv[1], "r+");
	// Store the content of the file
	char myString[100];
	fgets(myString, 100, fptr);

	int num_threads = atoi(myString); // This would be determined at runtime
	pthread_t *threads = (pthread_t*) malloc(num_threads * sizeof(pthread_t));

	for (int i = 0; i < num_threads; i++) {
		if (!fgets(myString, sizeof(myString), fptr)) {
			fprintf(stderr, "Not enough commands in file\n");
			num_threads = i; // adjust count
			break;
		}

		// strip newline
		myString[strcspn(myString, "\r\n")] = '\0';

		// duplicate command string for this thread
		char *cmd = malloc(strlen(myString) + 1);
		strcpy(cmd, myString);
		pthread_create(&threads[i], NULL, parallel, cmd);
		pthread_join(threads[i], NULL);
		printf("End of for loop\n");
	}
}

