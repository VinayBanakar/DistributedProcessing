#include <stdio.h>
#include <shmem.h>


int main(void)
{
	int src = 99;
	long source[4] = { 1, 2, 3, 4};
	static long dest[10];
	static int targ;
	shmem_init();
	int me = shmem_my_pe();
	int nprocs = shmem_n_pes();

	if (me == 0) {
		shmem_put(dest, source, 4, nprocs-1);
		shmem_put(dest, source, 4, nprocs-1);
		shmem_fence();
		shmem_put(&targ, &src, 1, nprocs-1); 
		shmem_put(&targ, &src, 1, nprocs-1);

	}
	shmem_barrier_all();

	// shmem_quite is not needed if shmem_barier_all is called as it assures completion of all
	// outstanding PUT, AMO, memory store and nbi routines to symmetric data objects.

	printf("dest[0] on PE %d is %ld\n", me, dest[0]);
	shmem_finalize();
	return 0;
}
