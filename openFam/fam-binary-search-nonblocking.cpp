#include <fam/fam_exception.h>
#include <fam/fam.h>
#include <common/fam_options.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

// fam
#define REGION_NAME "search_area"

// BS
#define N_PER_PE 100
#define N_SEARCHES 50

using namespace std;
using namespace openfam;

fam *my_fam;
int myPE, numPEs;

void createRegionAndItem(uint64_t size, Fam_Region_Descriptor *region, Fam_Descriptor *item) {
        try {
                cout << "PE 0 creating region: " << REGION_NAME << endl;
                region = my_fam->fam_create_region(REGION_NAME, size, 0777, NONE);
                cout << "Created region "<< REGION_NAME <<" size: " << my_fam->fam_size(region) << endl;
        } catch(Fam_Exception &e) {
                cout << "Fam region creation failed " << e.fam_error_msg() << endl;
                //exit(1);
        }
        try {
                cout << "PE 0 creating data item 'keys'" << endl;
                // Array of size N_PER_PE * Total number of PEs
                //item = my_fam->fam_allocate("keys", (N_PER_PE * numPEs * sizeof(int)), 0777, region);
                //cout << "Created item 'keys' size: " << my_fam->fam_size(item) << endl;
        } catch(Fam_Exception &e) {
                cout << "Fam allocation failed for 'keys': " << e.fam_error_msg() << endl;
        }
}

// populate searchList array with random number of range lower to upper.
void populateSearchValues(int lower, int upper, uint64_t searchList[]){
        srand( (static_cast<unsigned int>(time(0))));
        for(int i=0; i < N_SEARCHES; ++i)
                searchList[i] = (rand() % (upper - lower + 1)) + lower;
}

bool binary_search(uint64_t key, Fam_Descriptor *item) {
        uint64_t low, mid, high;
        low = 0;
        high = numPEs * N_PER_PE;

        while(low < high){
                uint64_t val;
                mid = low + (high-low)/2;
                // Basically column, row in the dataitem.
                /**
                * Dataitem
                * 
                *                   (mid % N_PER_PE) -> {columns}
                * ((mid/NP_PER_PE)  [][][][][].... [N_PER_PE]
                *    * N_PER_PE)    [][][][][]....  
                *       ^           [][][][][]....
                *     {Rows}        .
                *                   .
                */
                uint64_t offset = (mid % N_PER_PE + (mid/N_PER_PE)* N_PER_PE ) * sizeof(uint64_t);
                my_fam->fam_get_nonblocking(&val, item, offset, sizeof(uint64_t));
                if(val == key){
                        return true;
                }
                else if(val < key)
                        low = mid;
                else
                        high = mid;
        }
        return -1;
}

int main(){
        Fam_Region_Descriptor *region = NULL;
        Fam_Descriptor *item = NULL;
        Fam_Options fam_opts;
        int *val;

        //Initialize FAM
        my_fam = new fam();
        memset((void *)&fam_opts, 0, sizeof(Fam_Options));
        // fam_opts.allocator = strdup("NVMM");
        fam_opts.runtime = strdup("NONE");
        try {
                my_fam->fam_initialize("default", &fam_opts);
        } catch(Fam_Exception &e){
                cout << "Fam initialization failed: " << e.fam_error_msg() << endl;
                exit(1);
        }
        val = (int *)my_fam->fam_get_option(strdup("PE_ID"));
        myPE = *val;
        val = (int *)my_fam->fam_get_option(strdup("PE_COUNT"));
        numPEs = *val;

        // Let PE 0 create FAM region and dataitem containing the array.
        if(myPE == 0){
                uint64_t size = (4 * 2048UL * 2048UL);
                createRegionAndItem(size, region, item);
        }

        uint64_t pe_keys[N_PER_PE];
        uint64_t searchList[N_SEARCHES];
        // Lookup region and item
        try {
                region = my_fam->fam_lookup_region(REGION_NAME);
                if(region!=NULL) {
                        cout << "PE " << myPE << " region lookup successful" << endl;
                        item = my_fam->fam_lookup("keys", REGION_NAME);
                        if(item !=NULL){
                                cout << "PE " << myPE << " item lookup successful" << endl;
                                // Each PE is responsible to populate its part of the array in FAM.
                                // lets populate local pe_keys
                                for(int i = 0; i< N_PER_PE; ++i)
                                        pe_keys[i] = N_PER_PE * myPE + i;
                                // Now lets put pe_keys to item at appropriate offset.
                                uint64_t offset = N_PER_PE * myPE * sizeof(uint64_t);
                                //int ret = my_fam->fam_put_blocking(pe_keys, item, offset, N_PER_PE * sizeof(uint64_t));
                                my_fam->fam_put_nonblocking(pe_keys, item, offset, N_PER_PE * sizeof(uint64_t));
                                // if(ret <0){
                                //         cout << "fam_put_blocking failed" << endl;
                                // }

                                // Wait for all PEs to complete
                                my_fam->fam_barrier_all();

                                populateSearchValues(0, N_PER_PE * numPEs, searchList);

                                // for(int i =0; i < N_SEARCHES; ++i) 
                                //         cout << searchList[i] << endl;
                                // Now let each PE search its searchList values in FAM
                                for(int i = 0; i< N_SEARCHES; ++i){
                                        bool check = binary_search(*(searchList+i), item);
                                        if(check){
                                                cout << "PE#" << myPE << "\t" << *(searchList+i);
                                                cout << "\t" << "FOUND!" << endl;
                                        } else {
                                                cout << "PE#" << myPE << "FAILED!!!! ";
                                                cout << *(searchList+i) << "Not found.";
                                        }
                                }
                        }
                }
        } catch(Fam_Exception &e) {
                cout << "Error: " << e.fam_error_msg() << endl;
                exit(1);
        }
}
