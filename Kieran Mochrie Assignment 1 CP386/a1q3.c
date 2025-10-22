/**
 * -------------------------------------
 * @file  a1q3.c
 * file description
 * -------------------------------------
 * @Kieran M, 169048254, moch8254@mylaurier.ca
 *
 * @version 2025-09-30
 *
 * -------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	int pipe1[2];  // Between ps and sort
	int pipe2[2];  // Between sort and wc

	if (pipe(pipe1) == -1) {
		perror("pipe1");
		exit(EXIT_FAILURE);
	}

	if (pipe(pipe2) == -1) {
		perror("pipe2");
		exit(EXIT_FAILURE);
	}

	pid_t pid1, pid2, pid3;

	// First process: ps xao sid
	pid1 = fork();
	if (pid1 < 0) {
		perror("fork ps");
		exit(EXIT_FAILURE);
	}

	if (pid1 == 0) {
		// Child: ps
		dup2(pipe1[1], STDOUT_FILENO); // stdout -> pipe1 write
		close(pipe1[0]);  // not needed
		close(pipe1[1]);
		close(pipe2[0]);
		close(pipe2[1]);

		execlp("ps", "ps", "xao", "sid", (char*) NULL);
		perror("exec ps");
		exit(EXIT_FAILURE);
	}

	// Second process: sort -u
	pid2 = fork();
	if (pid2 < 0) {
		perror("fork sort");
		exit(EXIT_FAILURE);
	}

	if (pid2 == 0) {
		// Child: sort
		dup2(pipe1[0], STDIN_FILENO);   // stdin <- pipe1 read
		dup2(pipe2[1], STDOUT_FILENO);  // stdout -> pipe2 write
		close(pipe1[0]);
		close(pipe1[1]);
		close(pipe2[0]);
		close(pipe2[1]);

		execlp("sort", "sort", "-u", (char*) NULL);
		perror("exec sort");
		exit(EXIT_FAILURE);
	}

	// Third process: wc -l
	pid3 = fork();
	if (pid3 < 0) {
		perror("fork wc");
		exit(EXIT_FAILURE);
	}

	if (pid3 == 0) {
		// Child: wc
		dup2(pipe2[0], STDIN_FILENO); // stdin <- pipe2 read
		close(pipe1[0]);
		close(pipe1[1]);
		close(pipe2[0]);
		close(pipe2[1]);

		execlp("wc", "wc", "-l", (char*) NULL);
		perror("exec wc");
		exit(EXIT_FAILURE);
	}

	// Parent: close all pipe FDs
	close(pipe1[0]);
	close(pipe1[1]);
	close(pipe2[0]);
	close(pipe2[1]);

	// Wait for all children
	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);
	waitpid(pid3, NULL, 0);

	return 0;
}

