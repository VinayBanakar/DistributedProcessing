#include <stdio.h>
#include "shmem.h"

int main(void){
	static int targ = 0;
	static long x = 101010;
	long y = -1;
	shmem_init();
	int me = shmem_my_pe();
	int  receiver = 1 % shmem_n_pes();
	printf("I am #%d of %d PEs executing the program\n", me, shmem_n_pes());
	
	// Remote memory access - PUT and GET

	if(me==0) {
		int src = 66;
		shmem_put(&targ, &src, 1, receiver); // puts src value to target variable in receiver PE.
		printf("long y before PE #%d remote copies is %ld \n",me,y);
		y = shmem_g(&x, receiver);
		printf("long y after a remote copy from PE #%d is %ld \n", receiver, y);
	}


	shmem_barrier_all(); // Without sync the target PE does not know when the data is available.

	if(me==receiver)
		printf("PE %d targ=%d (expect 66)\n", me, targ);
	shmem_finalize();
	return 0;
}
