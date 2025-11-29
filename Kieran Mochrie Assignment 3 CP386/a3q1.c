/**
 * -------------------------------------
 * @file  a3q1.c
 * file description
 * -------------------------------------
 * @Kieran M, 169048254, moch8254@mylaurier.ca
 *
 * @version 2025-11-06
 *
 * -------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX 5 //buffer size is a max of 10
#define total 10 //producer produces 10 items

int buffer[MAX]; //shared buffer of size 5

int fill = 0;
int use = 0;

sem_t empty;
sem_t full;
sem_t consumable;
sem_t count; //semaphore of items in the buffer
pthread_mutex_t mutex;

void put(int value) {
	buffer[fill] = value;
	fill = (fill + 1) % MAX;
}
int get() {
	int tmp = buffer[use];
	use = (use + 1) % MAX;
	return tmp;
}

void* producer(void *arg) {
	for (int i = 0; i < total; i++) {
		sem_wait(&empty); //make sure there is room to produce an item
		pthread_mutex_lock(&mutex); //lock thread

		put(i); //if room, produce item

		sem_post(&count); //increase count of items in buffer
		int tmpCount; //temporary count
		sem_getvalue(&count, &tmpCount); //semaphores are not ints and cannot be used in print so store value of count into tmpCount to print value
		printf("Produced %d (Buffer count: %d)\n", i, tmpCount);
		if (tmpCount >= 3) {
			sem_post(&consumable); //more then 3 items in the bufer so consumer can consume
		}
		pthread_mutex_unlock(&mutex);
		sem_post(&full);
	}
	pthread_exit(NULL);
}
void* consumer(void *arg) {
	sem_wait(&consumable); //can the consumer consume?
	int taken = 0, tmp = 0;
	while (taken < total) { //while not all products have been consumed
		sem_wait(&full); //buffer must not be empty
		pthread_mutex_lock(&mutex);

		tmp = get();

		sem_wait(&count); //decrase item count
		int tmpCount = 0;
		sem_getvalue(&count, &tmpCount); //semaphores are not ints and cannot be used in print so store value of count into tmpCount to print value
		printf("\tConsumed %d (Buffer count: %d)\n", tmp, tmpCount);
		taken++;

		pthread_mutex_unlock(&mutex);
		sem_post(&empty);
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	pthread_t production;
	pthread_t consumption;

	pthread_mutex_init(&mutex, NULL);
	sem_init(&empty, 0, MAX); //5 slots for itmes are open
	sem_init(&full, 0, 0); //0	items to take
	sem_init(&count, 0, 0); //0 items in buffer at start
	sem_init(&consumable, 0, 0); //no items in buffer so must wait

	pthread_create(&production, NULL, producer, NULL); //start threads
	pthread_create(&consumption, NULL, consumer, NULL);
	pthread_join(production, NULL); //finish threads
	pthread_join(consumption, NULL);

	sem_destroy(&empty); //free memory and exit
	sem_destroy(&full);
	sem_destroy(&count);
	sem_destroy(&consumable);
	pthread_mutex_destroy(&mutex);
	printf("Program finished\n");
	return 0;
}

