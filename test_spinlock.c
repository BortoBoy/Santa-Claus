#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#include "spinlock.h"

pthread_t *CreateThread ( void *(* f)(void *),void *a) {
	pthread_t *t = malloc(sizeof(pthread_t));
	assert(t != NULL);
	int ret = pthread_create(t,NULL,f,a);
	assert(ret == 0);
	return t;
}

static const int n_loops = 100000;
static int lock = 0;
static int shared_counter = 0;

void *Loop (void *arg) {
	// int id = (int) arg;
	for (int l=0; l<n_loops; l++) {
		SpinlockEnter(&lock);
		// counter++
		int copy = shared_counter;
		copy += 1;
		// pthread_yield();
		sleep(0);
		shared_counter = copy;
		SpinlockLeave(&lock);
	}
	return arg;
}

int main ( int ac, char **av ) {
	pthread_t *t1 = CreateThread(Loop,(void *) 1);
	pthread_t *t2 = CreateThread(Loop,(void *) 2);	
	pthread_join(*t1,NULL);
	pthread_join(*t2,NULL);
		int violations = 2*n_loops - shared_counter;
	if (violations == 0)
		printf("Spinlock seems to work\n");
	else
		printf("Mutual exclusion condition has been violated %d times\n",violations);
	free(t1);
	free(t2);
}