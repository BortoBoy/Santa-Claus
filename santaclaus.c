#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

#include "semaphore.h"
#include "barrier.h"

pthread_t *CreateThread ( void *(* f)(void *),void *a) {
	pthread_t *t = malloc(sizeof(pthread_t));
	assert(t != NULL);
	int ret = pthread_create(t,NULL,f,a);
	assert(ret == 0);
	return t;
}

/*	********************************************************************************
	Santa Claus Problem

	Solution with semaphores is based on the "The Little Book of Semaphores"
	written by Allen B. Downey
	******************************************************************************** */

static const int N_ELVES = 10;
static const int N_REINDEER = 9;

static int elves;
static int reindeer;
static semaphore_t santaSem;
static semaphore_t reindeerSem;
static semaphore_t elfTex;
static semaphore_t mutex;

static barrier_t sleigh;
static barrier_t workbench;

/*	********************************************************************************
	Santa Claus
	******************************************************************************** */

void *SantaClaus (void *arg) {
	printf("Santa Claus: Hoho, here I am\n");
	while (true) {
		P(santaSem);
		P(mutex);
		if (reindeer == N_REINDEER) {
			printf("Santa Claus: preparing sleigh\n");
			for (int r=0; r<N_REINDEER; r++)
				V(reindeerSem);
			BarrierReached(sleigh);
			printf("Santa Claus: make all kids in the world happy\n");
			BarrierReached(sleigh);
			reindeer = 0;
		}
		else if (elves == 3) {
			BarrierReached(workbench);
			printf("Santa Claus: helping elves\n");
			BarrierReached(workbench);
		}
		V(mutex);
	}
	return arg;
}

/*	********************************************************************************
	Reindeer
	******************************************************************************** */

void *Reindeer (void *arg) {
	int id = (int) arg;
	printf("This is reindeer %d\n",id);
	while (true) {
			P(mutex);
			reindeer++;
			if (reindeer == N_REINDEER)
				V(santaSem);
			V(mutex);
			P(reindeerSem);
			BarrierReached(sleigh);
			printf("Reindeer %d getting hitched\n",id);
			BarrierReached(sleigh);
			sleep(20);
	}
	return arg;
}

/*	********************************************************************************
	Elve
	******************************************************************************** */

void *Elve (void *arg) {
	int id = (int) arg;
	printf("This is elve %d\n",id);
	while (true) {
		bool need_help = random() % 100 < 10;
		if (need_help) {
			printf("elve %d needs help\n",id);
			P(elfTex);
			P(mutex);
			elves++;
			if (elves == 3)
				V(santaSem);
			else
				V(elfTex);
			V(mutex);

			BarrierReached(workbench);
			printf("elve %d will get help from Santa Claus\n",id);
			BarrierReached(workbench);
			sleep(10);

			P(mutex);
			elves--;
			if (elves == 0)
				V(elfTex);
			V(mutex);
		}
		// Do some work
		printf("elve %d at work\n",id);
		sleep(2 + random() % 5);
	}
	return arg;
}

/*	********************************************************************************
	Build the world with Santa Claus, the reindeer, and the elves
	******************************************************************************** */

int main ( int ac, char **av ) {
	elves = 0;
	reindeer = 0;
	santaSem = CreateSemaphore(0);
	reindeerSem = CreateSemaphore(0);
	elfTex = CreateSemaphore(1);
	mutex = CreateSemaphore(1);
	sleigh = CreateBarrier(N_REINDEER+1);
	workbench = CreateBarrier(3+1);

	pthread_t *santa_claus = CreateThread(SantaClaus,0);

	pthread_t *reindeers[N_REINDEER];
	for (int r=0; r<N_REINDEER; r++)
		reindeers[r] = CreateThread(Reindeer,(void *) r+1);

	pthread_t *elves[N_ELVES];
	for (int e=0; e<N_ELVES; e++)
		elves[e] = CreateThread(Elve,(void *) e+1);

	int ret = pthread_join(*santa_claus,NULL);
	assert(ret == 0);
}