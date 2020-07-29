#ifndef _FES_MEMORY_MANAGEMENT_H_
#define _FES_MEMORY_MANAGEMENT_H_




// memory management for feature extraction
class FESMemoryManagement {
friend class ExtractionMethodManager;
friend class FESMethodManager;
friend class FESRegion;
SSFQueue<SSFMultipleFeatures *> features;
//queue<SSFMultipleFeatures *> features;
//CondMutexManager<bool> availableVectors;
SSFMutex mutex;

protected:
	// release an entry
	void ReleaseEntry(SSFMultipleFeatures *feat);

	// allocate an entry
	SSFMultipleFeatures *AllocateEntry();

public:
	FESMemoryManagement(int numEntries);
};


#endif