#ifndef _MODULE_FES_METHOD_MANAGER_
#define _MODULE_FES_METHOD_MANAGER_

class FESMethodInstance;
class FESMemoryManagement;
class FESRequestRegions;
class FESRequestInterestPoints;
class FESCache;

class FESMethodManager : public SSFMethodThread {
SSFMutex mutex;
friend class ExtractionFeatureInstance;
friend class FESControl;
SSFQueue<FESRequest *> requests;
vector<FESMethodInstance *> instances;			// instances to the launched
FESMemoryManagement *memory;					// memory management to feature vectors
FESCache *cache;								// feature cache
int ninstances;									// number of processing threads
int processingType;
int dataType;
bool useCache;
int nCacheBuckets;								// number of cache divisions
int nCacheEntries;								// number of cache entries
string extractionSetup;							// setup ID for the extraction method
string extractionMethod;						// name of the feature extraction method
SSFQueue<size_t> availableProcessingUnit;		// IDs of instances available to process

    static FESMethodManager& Instance() {
        static FESMethodManager instance("");
        return instance;
    }

	// launch a processing unit of the feature extraction method
	void LaunchFeatureInstances(SSFParameters *params);

	// 

	// function to process a request with blocks
	void ProcessRegionsRequest(FESRequestRegions *request);

	// function to process a request with interest points
	void ProcessInterestPointsRequest(FESRequestInterestPoints *request);

	// function to process a request to GPU
	void ProcessGPURequest(FESRequest *request);

	// set pointer to the memory management for the features
	void SetMemoryManagerPointer(FESMemoryManagement *memory);

	// function to request the module to end (finish the execution first)
	void RequestFinish();

	// return current version of the method
	string GetVersion() { return "0.0.1"; }

public:
	FESMethodManager(string instanceID);

	// Return the name of the method
	string GetName() { return SSF_METHOD_FES_MANAGER; }

	// Return the type of the method
	string GetType() { return SSF_METHOD_TYPE_THREAD_MISC; }
	
	// function to generate a new instatiation of the manager
	SSFMethod *Instantiate(string instanceID);

	// function to check if input modules have been set correctly
	void Setup();

	// execute
	void Execute();

	// function to launch this method
	void Launch();

	/* specific functions */
	// receive a feature extraction request
	void SetRequest(FESRequest *request);

	// function to set a request as processed (called by the feature extraction instance)
	void SetRequestProcessed(int instanceID);

	// retrieve feature extraction method signature
	size_t GetExtractionMethodSignature();
};

#endif