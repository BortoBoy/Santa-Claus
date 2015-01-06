#include <pthread.h>
#include <assert.h>
#include <stdlib.h>

#include "barrier.h"
#include "semaphore.h"

/*	********************************************************************************
	How to implement a barrier using semaphores
	******************************************************************************** */

struct barrier_state {
	semaphore_t mutex;
	semaphore_t q_enter;
	semaphore_t q_leave;
	int n;
	int inside;	
};

barrier_t CreateBarrier ( int n ) {
	struct barrier_state *b = (struct barrier_state *) malloc(sizeof(struct barrier_state));
	b->mutex = CreateSemaphore(1);
	b->q_enter = CreateSemaphore(0);
	b->q_leave = CreateSemaphore(0);
	b->n = n;
	b->inside = 0;
	return (barrier_t) b;
}

void CloseBarrier ( barrier_t b ) {
	struct barrier_state *bs = (struct barrier_state *) b;
	CloseSemaphore(bs->mutex);
	CloseSemaphore(bs->q_enter);
	CloseSemaphore(bs->q_leave);
	free((char *) bs);
}

void BarrierReached ( barrier_t b ) {
	struct barrier_state *bs = (struct barrier_state *) b;
	P(bs->mutex);
	bs->inside++;
	if (bs->inside < bs->n) {
		V(bs->mutex);
		P(bs->q_enter);
		P(bs->q_leave);
	}
	else {
		assert(bs->inside == bs->n);
		for (int t=0; t<bs->n-1; t++)
			V(bs->q_enter);
		bs->inside = 0;
		for (int t=0; t<bs->n-1; t++)
			V(bs->q_leave);
		V(bs->mutex);
	}
}