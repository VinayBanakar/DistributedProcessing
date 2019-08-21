#include <shmem.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define N_PER_PE 10
#define N_SEARCHES 5

/* N_PER_PE*N_PES sorted, distributed shared array */
int keys[N_PER_PE];

static bool binary_search(int key) {
    int low, mid, high;

    low = 0;
    high = shmem_n_pes()*N_PER_PE;

    while(low < high) {
        int val;

        mid = low + (high-low)/2;
        val = shmem_int_g(&keys[mid%N_PER_PE], mid/N_PER_PE);

        if(val == key) {
            return true;
        } else if(val < key) {
            low = mid;
        } else {
            high = mid;
        }
    }

    return -1;
}

void populateSearchValues(int lower, int upper, int searchList[]) {
	srand(shmem_my_pe()*time(0));
	for(int i=0; i < N_SEARCHES; ++i){	
		searchList[i] = (rand() % (upper - lower + 1)) + lower;
	}
}

int main(int argc, char **argv) {
    int i, errors = 0;

    shmem_init();

    int searchList[N_SEARCHES];

    for (i = 0; i < N_PER_PE; i++)
        keys[i] = N_PER_PE * shmem_my_pe() + i;

    
    shmem_barrier_all();

    populateSearchValues(0, N_PER_PE * shmem_n_pes(), searchList);

    for(int i = 0; i < N_SEARCHES; ++i)
//	    printf("PE #%d \t i:%d \t %d\n",shmem_my_pe(),i,*(searchList+i));



    for (i = 0; i < N_SEARCHES; i++) {
        bool check = binary_search(*(searchList+i));
        if (check) {
		printf("PE #%d \t %d \t FOUND!\n", shmem_my_pe(), *(searchList+i));
	} else {
            printf("Error searching for number %d in PE #%d was not found.\n",
                   *(searchList+i), shmem_my_pe());
            errors++;
        }
    }

    shmem_finalize();

    return errors;
}
