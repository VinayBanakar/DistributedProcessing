#include <stdio.h>
#include "shmem.h"

int main(){
	static int x = 10;
	int y = 100;
	static int dest[4];
	char name[SHMEM_MAX_NAME_LEN];
	int major, minor;
	shmem_init();

	int me = shmem_my_pe();
	int npes = shmem_n_pes();
	
	if(me==0) {
		int* ptr = shmem_ptr(dest, npes-1);
		if(ptr == NULL)
			printf("Can't use pointer to directly access PE #%d's array\n",npes-1);
		else
			for(int i = 0;i<4;i++)
				*ptr++ = i+1;
	
		shmem_info_get_version(&major, &minor);
		shmem_info_get_name(name);
		printf("%s and minor: major %d, %d\n",name, minor, major);
		printf("%u\n", ptr);
	}


	shmem_barrier_all();
	if(me == npes-1){
		printf("PE #%d's dest: %d, %d, %d, %d\n",
				me,dest[0],dest[1],dest[2],dest[3]);
	}
	shmem_finalize();
	return 0;

}
