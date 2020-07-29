#include "headers.h"
#include "SSFCore.h"
#include "FES.h" 
#include "FESMemoryManagement.h"



//////
// class FESMemoryManagement
//////
FESMemoryManagement::FESMemoryManagement(int numEntries) {
SSFMultipleFeatures *feat;
int i;

	if (numEntries < 0) 
		ReportError("Invalid number of feature vectors to be allocated!");
	
	if (numEntries < 100)
		ReportWarning("Small number of allowed feature vectors!");
	
	for (i = 0; i < numEntries; i++) {
		feat = new SSFMultipleFeatures();
		features.push(feat);
	}
}



void FESMemoryManagement::ReleaseEntry(SSFMultipleFeatures *feat) {

	// return vector to the queue
	features.push(feat);
}


SSFMultipleFeatures *FESMemoryManagement::AllocateEntry() {
SSFMultipleFeatures *feat;

	// retrieve feature vector from the front of the queue
	feat = features.pop();

	// reset this feature
	feat->ResetAll();

	return feat;
}