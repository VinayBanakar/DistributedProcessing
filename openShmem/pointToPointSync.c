#include <stdio.h>
#include <shmem.h>

// mechanisms for sync between particular PEs based on condition and value of object.


// Determine who updated a particular value 

int main(){
	shmem_init();
	const int me = shmem_my_pe();
	const int nprocs = shmem_n_pes();
	int target = 1 % nprocs;
	long* wait_vars = shmem_calloc(nprocs, sizeof(long));
	if(me==0){
		int id = 0;
		while(!shmem_test(&wait_vars[id], SHMEM_CMP_NE, 0))
			id = (id+1) % nprocs;
		printf("PE %d observed first update from PE %d\n", me, id);
	}
	if(me== target) {
		shmem_p(&wait_vars[me], me, 0);
	}

	shmem_free(wait_vars);
	shmem_finalize();
	return 0;

}
