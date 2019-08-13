#include <stdio.h>
#include <shmem.h>
#include <stdlib.h>

// Broadcast data items to set of or all PEs

int main(){
	static long pSync[SHMEM_BCAST_SYNC_SIZE];
	for (int i=0; i< SHMEM_BCAST_SYNC_SIZE; i++)
		pSync[i] = SHMEM_SYNC_VALUE;
	static long src[3], dest[3];
	
	shmem_init();
	int me = shmem_my_pe();
	int nprocs = shmem_n_pes();

	if(me==0)
		for(int i = 0; i<3;++i)
			src[i] = i+1;
	shmem_broadcast64(dest, src, 3, 0, 0, 0, nprocs, pSync);
	// If the current PE is not the root PE, the dest data object is updated.
	printf("PE #%d, %ld, %ld, %ld \n",me, dest[0], dest[1], dest[2]);
	shmem_finalize();
	return 0;
}
