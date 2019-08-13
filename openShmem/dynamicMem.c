#include <stdio.h>
#include <shmem.h>
#include <stdlib.h>


// Global remote memory dynamic allocation, deallocation and reallocation.
int main(){
	char* Str="";
	shmem_init();

	int me = shmem_my_pe();
	int nprocs = shmem_n_pes();
	int n=10;
	int i = 0;
	int* Cal;

	Cal = (int*) shmem_calloc(n,sizeof(int));
	printf("CaL value: %d addr:%p\n",*Cal, Cal);
	Str = (char*) shmem_malloc(n*sizeof(char));
	if(Str == NULL){
		printf("Failed to allocate remote memory\n");
		shmem_global_exit(1);
	} else {
		for(i=0; i<n; ++i){
			Str[i] = 'a';
		}
		printf("Str to begin with is: %s addr: %p on PE #%d \n",Str,Str,me);
	}
	//	shmem_barrier_all();
	if(me ==0) {
		for(i=0; i<n; ++i){
			Str[i] = '5';
		}
		printf("I am %d and Str value: %s addr: %p\n",me, Str, Str);
	}

	shmem_barrier_all();
	if(me == nprocs-1){
		for(i=0;i<n; ++i){
			Str[i] = '9';
		}
		printf("I am %d and Str value: %s addr: %p\n", me, Str, Str);
	}

	shmem_barrier_all();
	// Lets now truncate the created memory from another PE
	if(me == (1 % nprocs)){
		Str = shmem_realloc(Str, (n-5)*sizeof(char));
		Str[n-5] = '\0';
		printf("I am %d and Str value: %s addr: %p\n", me, Str, Str);
	}
	shmem_free(Str);
	shmem_finalize();
}
