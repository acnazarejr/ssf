#ifndef _FES_CONTROL_H_
#define _FES_CONTROL_H_

class FESMemoryManagement;


// extraction control
class FESControl : public SSFInternalModule {
unordered_map<size_t, FESMethodManager *> featManagers;
SSFQueue<FESRequest *> requests;
FESMemoryManagement *memory;	// feature memory management
int memoryEntries;				// maximum number of entries for features
SSFMutex mutex;


    static FESControl& Instance() {
        static FESControl instance("");
        return instance;
    }

protected:
	// function to request the module to end (finish the execution first)
	void RequestFinish();

	string GetVersion() { return "0.0.1"; }

public:
	FESControl(string instanceID);

	// Return the name of the module
	string GetName() { return SSF_METHOD_INTERNALMOD_FES; }

	// function to generate a new instatiation of the detector
	SSFMethod *Instantiate(string instanceID);

	// function to check if input modules have been set correctly
	void Setup();

	// execute
	void Execute();

	/* specific functions */
	// set  feature extraction request
	void SetRequestToProcess(FESRequest *request);

	// launch feature extraction manager with setupID with parameters in paramFile - return its unique signature
	size_t LaunchExtractionMethodManager(SSFParameters *params, string setupID);

};


#endif