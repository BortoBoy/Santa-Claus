#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#include "barrier.h"

pthread_t *CreateThread ( void *(* f)(void *),void *a) {
	pthread_t *t = malloc(sizeof(pthread_t));
	assert(t != NULL);
	int ret = pthread_create(t,NULL,f,a);
	assert(ret == 0);
	return t;
}

static const int n_loops = 100;
static const int n_threads = 4;

static barrier_t barrier;

void *Loop (void *arg) {
	int id = (int) arg;
	for (int l=0; l<n_loops; l++) {
		sleep(random() % 10 + 1);
		printf("Thread %d in loop %d\n",id,l);
		BarrierReached(barrier);
	}
	return arg;
}

int main ( int ac, char **av ) {
	barrier = CreateBarrier(n_threads);

	pthread_t *threads[n_threads];
	for (int t=0; t<n_threads; t++)
		threads[t] = CreateThread(Loop,(void *) t);

	pthread_join(*threads[0],NULL);

	for (int t=0; t<n_threads; t++)
		free(threads[t]);

	CloseBarrier(barrier);
}