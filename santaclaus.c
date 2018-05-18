#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

#include "semaphore.h"

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

static const int N_ELFOS = 10;
static const int N_RENAS = 9;

static int elfos;
static int renas;
static semaphore_t santaSem;
static semaphore_t renasSem;
static semaphore_t elfTex;
static semaphore_t mutex;

/*	********************************************************************************
	Papai Noel
	******************************************************************************** */

void *PapaiNoel (void *arg) {
	printf("Papai Noel Begins\n");
	while (true) {
		P(santaSem);
		P(mutex);
		if (renas == N_RENAS) {
			printf("Papai Noel praparando o trenó\n");
			for (int r=0; r<N_RENAS; r++)
				V(renasSem);
			printf("Papi Noel indo pro natal\n");
			renas = 0;
		}
		else if (elfos == 3) {
			printf("Papai Noel ajudando elfos\n");
		}
		V(mutex);
	}
	return arg;
}

/*	********************************************************************************
	Renas
	******************************************************************************** */

void *Rena (void *arg) {
	int id = (int) arg;
	printf("Rena %d\n",id);
	while (true) {
			P(mutex);
			renas++;
			if (renas == N_RENAS)
				V(santaSem);
			V(mutex);
			P(renasSem);
			printf("rena %d está presa\n",id);
			sleep(20);
	}
	return arg;
}

/*	********************************************************************************
	Elfo
	******************************************************************************** */

void *Elfo (void *arg) {
	int id = (int) arg;
	printf("Elfo %d executando\n",id);
	while (true) {
		bool need_help = random() % 100 < 10;
		if (need_help) {
			P(elfTex);
			P(mutex);
			elfos++;
			if (elfos == 3)
				V(santaSem);
			else
				V(elfTex);
			V(mutex);

			printf("elfo %d solicitando ajuda do Papai Noel\n",id);
			sleep(10);

			P(mutex);
			elfos--;
			if (elfos == 0)
				V(elfTex);
			V(mutex);
		}
		printf("Elfo %d trabalhando\n",id);
		sleep(2 + random() % 5);
	}
	return arg;
}

int main ( int ac, char **av ) {
	elfos = 0;
	renas = 0;
	santaSem = CreateSemaphore(0);
	renasSem = CreateSemaphore(0);
	elfTex = CreateSemaphore(1);
	mutex = CreateSemaphore(1);

	pthread_t *tPapaiNoel = CreateThread(PapaiNoel,0);

	pthread_t *tVetordeRenas[N_RENAS];
	for (int r=0; r<N_RENAS; r++)
		tVetordeRenas[r] = CreateThread(Rena,(void *) r+1);

	pthread_t *tVetordeElfos[N_ELFOS];
	for (int e=0; e<N_ELFOS; e++)
		tVetordeElfos[e] = CreateThread(Elfo,(void *) e+1);

	int ret = pthread_join(*tPapaiNoel,NULL);
	assert(ret == 0);
}