#include <stdio.h>
#include <shmem.h>
#include <stdlib.h>

// Concatenates blocks of data from multiple PEs to an array in every PE.

// fcollect requires number of elements in source array to be constant amongst PEs, collect has no such restriction.
//

int main(){
	static long pSync[SHMEM_COLLECT_SYNC_SIZE];
	for(int i =0; i< SHMEM_COLLECT_SYNC_SIZE; ++i)
		pSync[i] = SHMEM_SYNC_VALUE;
	static long lock =0;

	shmem_init();
	int me = shmem_my_pe();
	int nprocs = shmem_n_pes();

	int* src = (int*) shmem_malloc(nprocs*sizeof(int));
	int* dest = (int*) shmem_malloc(nprocs * nprocs * sizeof(int));

	for(int i=0; i<nprocs; ++i)
	       src[i] = i+1;
	for(int i=0; i < nprocs*nprocs; ++i)
		dest[i] = -99;	
	
	shmem_barrier_all();

	shmem_collect32(dest, src, nprocs, 0, 0, nprocs, pSync);

	shmem_set_lock(&lock);
	printf("PE #%d %d \n",me, dest[0]);
	shmem_clear_lock(&lock);

	shmem_free(src);
	shmem_free(dest);

	shmem_finalize();
	return 0;
}
