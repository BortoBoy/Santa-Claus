#ifndef _asysob_spinlock_
#define _asysob_spinlock_

/*	********************************************************************************
	How to implement a spinlock using inline assembler
	******************************************************************************** */

void SpinlockEnter ( int *addr );
void SpinlockLeave ( int volatile *addr );

#endif // _asysob_spinlock_