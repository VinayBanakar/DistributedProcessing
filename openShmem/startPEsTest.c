#include <stdio.h>
#include <shmem.h>

int main(int argc, char* argv[]) {
	int nprocs,me;
	start_pes(4); //deprecated
	nprocs  = shmem_n_pes(); me = shmem_my_pe();
	printf("Hello from %d of %d\n",me,nprocs);
	return 0;
}
