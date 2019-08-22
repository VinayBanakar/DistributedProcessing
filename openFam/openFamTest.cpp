
#include "common/fam_options.h"
#include <fam/fam.h>
#include <fam/fam_exception.h>
#include <iostream>

#define REGION_NAME "test"

using namespace std;
using namespace openfam;

fam *my_fam;
int myPE, numPEs;


void dummy_func(Fam_Region_Descriptor *region) {
	// does nothing
}

int main(){

	Fam_Region_Descriptor *region = NULL;
	Fam_Options fam_opts;
	int *val;
	// Initialize FAM
	my_fam = new fam();
	memset((void *)&fam_opts, 0, sizeof(Fam_Options));
	cout << "Size of Fam_Options " << sizeof(Fam_Options) << endl;
	fam_opts.allocator = strdup("NVMM");
	fam_opts.runtime = strdup("NONE");

	try {
		my_fam->fam_initialize("default", &fam_opts);
	} catch(Fam_Exception &e) {
		cout << "fam initialization failed:" << e.fam_error_msg() << endl;
		exit(1);
	}
	val = (int *)my_fam->fam_get_option(strdup("PE_ID"));
	myPE = *val;
	val = (int *)my_fam->fam_get_option(strdup("PE_COUNT"));
	numPEs = *val;

	cout << "Total PEs : " << numPEs << endl;
	cout << "My PE: " <<  myPE << endl;

	// Lets create a region with REGION_NAME
	 uint64_t size = 1024 * 1024;
	try {
		cout << "Creating region " << REGION_NAME << endl;
		region = my_fam->fam_create_region(REGION_NAME, size, 0777, NONE);
	} catch (Fam_Exception &e) {
		cout << "Fam region creation failed " << e.fam_error_msg() << endl;
	}
	
	Fam_Descriptor *dataitem;
	int64_t *local = NULL;
	// Lets access the region with REGION_NAME that was just created
	try {
		cout << "Lookup created region" << endl;
		region = my_fam->fam_lookup_region(REGION_NAME);
		if(region!=NULL) {		
			try {
				cout << "Lets alllocate a dataitem " << endl;
				dataitem = my_fam->fam_allocate("testData", (sizeof(int64_t)), 0777, region);
				if(dataitem){
					cout << "Map dataitem to PE's VAS" << endl;
					local = (int64_t *)my_fam->fam_map(dataitem);
				}
		
				local[0] = 66;

				// Flushing modified data
				cout << "Flushing local data to FAM" << endl;
				my_fam->fam_flush(local,sizeof(int64_t));

				// Dlete dataitem?
				if(dataitem) {
					cout << "Delete dataitem." << endl;
					delete dataitem; // This just deletes the descriptor not the actual dataitem. Do fam_get for that.
				}
			} catch (Fam_Exception &e) {
				cout << "Error: " << e.fam_error_msg() << endl;
				//exit(1);
			}
			try {
				// Lets now destroy region REGION_NAME
				cout << "Destroying created region " << REGION_NAME << endl;
				my_fam->fam_destroy_region(region);
			} catch (Fam_Exception &e) {
				cout << "Can not destroy region " << REGION_NAME << ": " << e.fam_error_msg() << endl;
			        cout << "Error: " <<  e.fam_error() << endl;
				exit(1);
			}
		}
	} catch (...) {
		cout << "fam lookup failed for region :" << REGION_NAME << endl;
		exit(1);
	}

	

	dummy_func(region);
}
