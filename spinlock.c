#include "spinlock.h"

/*	********************************************************************************
	Entering the critical section
	******************************************************************************** */

/*	using the implementation shown on the web:
	http://stackoverflow.com/questions/6935442/x86-spinlock-using-cmpxchg
*/

void SpinlockEnter ( int *addr ) {
	while(!__sync_bool_compare_and_swap(addr,0,1));
}

/*	********************************************************************************
	Leaving the critical section
	******************************************************************************** */

void SpinlockLeave ( int volatile *addr ) {
	asm volatile("");
	*addr = 0;
}
