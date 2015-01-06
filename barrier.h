#ifndef _asysob_barrier_
#define _asysob_barrier_

/*	********************************************************************************
	How to implement a barrier using some semaphores
	******************************************************************************** */

typedef void *barrier_t;

barrier_t CreateBarrier ( int n );
void CloseBarrier ( barrier_t b);

void BarrierReached ( barrier_t b );

#endif // _asysob_barrier_