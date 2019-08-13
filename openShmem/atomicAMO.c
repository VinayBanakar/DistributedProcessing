#include <stdio.h>
#include <shmem.h>

// Atomic Memory Operation (AMO) in PEs
// There are also only performed on symmetric objects.

int main(){
	int x = 10;
	static int y = 0;
	
	shmem_init();
	int me = shmem_my_pe();
	int nprocs = shmem_n_pes();

	if(me==0){
		printf("Value of PE #0's y: %d\n",y);
		shmem_atomic_set(&y, x, nprocs-1);
		printf("Now the value of y on PE #%d is %d\n",nprocs-1, shmem_atomic_fetch(&y, nprocs-1));
		// shmem_atomic_swap does a conditional swap on remote data objects.
		printf("Now lets swap PE #%d's y with PE #0's y. Now PE 0's y is: %d\n",nprocs-1,
			       	shmem_atomic_swap(&y, y, nprocs-1));

		// shmem_atomic_fetch_inc --- shmem_atomic_inc
		// shmem_atomic_fetch_and
		// shmem_atomic_fetch_or
	}
	shmem_finalize();
}

