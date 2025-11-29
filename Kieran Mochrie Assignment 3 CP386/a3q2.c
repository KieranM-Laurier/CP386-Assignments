/**
 * -------------------------------------
 * @file  a3q2.c
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
#include <stdbool.h>
//5 waiting room chairs
//1 barber
//1 barber chair
#define chairs 5//number of chairs
#define customerCount 15//number of customers
bool sleeping = true; //barber starts asleep since there are no customers
int waiting = 0; //no customers are waiting

pthread_mutex_t mutex;
pthread_cond_t barber;
pthread_cond_t customers;

int cutTime = 3; //change this variable to have different times to cut hair
int entryTime = 1; //change this to change how long between each customer entering the shop

void* barberThread(void *arg) { //named the condition first so barber gets a label of Thread
	while (true) {
		pthread_mutex_lock(&mutex);
		while (waiting == 0) {
			printf("Barber is sleeping\n");
			sleeping = true;
			pthread_cond_wait(&barber, &mutex); //sleep until a customer is inside
		}
		waiting--; //1 less waiting customer since they are in the chair
		sleeping = false; //barber is cutting hair so is awake
		printf("The barber is cutting hair\n");
		pthread_cond_signal(&customers);
		pthread_mutex_unlock(&mutex);
		sleep(cutTime); //time to get a haircut and everything else needed
	}
	return NULL;
}

void* customerThread(void *arg) {
	int customerNumber = *(int*) arg;
	pthread_mutex_lock(&mutex);
	if (waiting < chairs) {
		waiting++;
		printf("Customer %d is waiting\n", customerNumber); //show what customer is waiting
		if (sleeping) {
			pthread_cond_signal(&barber); //wake barber hes got work to do
		}
		pthread_cond_wait(&customers, &mutex); //is chair open for cutting
		printf("Customer %d is getting a haircut\n", customerNumber);
		pthread_mutex_unlock(&mutex);
		sleep(cutTime); //match time from barber function to keep it organized

	} else { //if there are already 5 waiting customers
		printf("No avaliable chairs, customer %d left.\n", customerNumber);
		pthread_mutex_unlock(&mutex);
	}
	free(arg); //free memory since we needed customerOrder for numbering
	pthread_exit(NULL);
	return NULL;
}

int main() {
	pthread_t threadBarber;
	pthread_t customerThreads[customerCount]; //15 customers come and go

	pthread_mutex_init(&mutex, NULL); //create the threads and mutex
	pthread_cond_init(&barber, NULL);
	pthread_cond_init(&customers, NULL);

	pthread_create(&threadBarber, NULL, barberThread, NULL);
	sleep(1);
	for (int i = 0; i < customerCount; i++) {
		int *customerOrder = malloc(sizeof(int)); //track order customers came in, dont order later since customers may have left
		*customerOrder = i;
		pthread_create(&customerThreads[i], NULL, customerThread,
				customerOrder); //check assignment 1 question 2 to get detail on ts
		sleep(entryTime); //customers come in every 5 time units for testing and whatnot
	}
	for (int i = 0; i < customerCount; i++) {
		pthread_join(customerThreads[i], NULL);
	}
	pthread_cancel(threadBarber); //this stops the barber thread since we cant stop it until all customers have been in and out of the shop
	pthread_mutex_destroy(&mutex); //rest of them go as normal since the program is done
	pthread_cond_destroy(&barber);
	pthread_cond_destroy(&customers);
	printf("Shop closed.\n");
	return 0;
}

