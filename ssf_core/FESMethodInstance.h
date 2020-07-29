#ifndef _FES_METHOD_INSTANCE_H_
#define _FES_METHOD_INSTANCE_H_



// feature extraction instance 
class FESMethodInstance : public SSFMethodThread {
friend class FESMethodManager;
SSFMutex mutex;

protected:
FESMethodManager *extMetManager;
int instanceID;
FESCache *cache;

protected:
	void SetExtractionManager(FESMethodManager *extMetManager);

	void SetInstanceID(int instanceID);

	// set cache to the extraction instance
	void SetCache(FESCache *cache);

	// set an already setup feature extraction method
	virtual void SetFeatureExtractionMethod(SSFMethodFeatureExtraction *method) = 0;

public:
	FESMethodInstance(string instanceID);

	// retrieve method signature
	virtual size_t GetExtractionSignature() = 0;

	// Return number of features in this method according to the block size
	//virtual int GetNFeatures(SSFRect &rect) = 0;
};




////
// FESMethodInstanceGPU: feature extraction for GPU
////
class FESMethodInstanceGPU : public FESMethodInstance {
friend class FESMethodManager;

protected:

	virtual void ExtractFeatures(FESRequest *request, FESBlocks *blocks) = 0;

	// set an already setup feature extraction method
	void SetFeatureExtractionMethod(FESMethodInstance *method); 

public:

};




////
// FESMethodInstanceCPU: feature extraction for CPU
////
class FESMethodInstanceCPU : public FESMethodInstance {
friend class FESMethodManager;

protected:
	virtual void SetData(FESData *data, FESBlocks *blocks, FESRequestRegions *request) = 0;

	// set request to feature extraction
	virtual void SetRequest(FESRegion *region) = 0;

	// function to request the module to end (finish the execution first)
	void RequestFinish();

public:
	FESMethodInstanceCPU(string instanceID);

};




////
// FESMethodInstanceCPU2D: 2D feature extraction for CPU
////
class FESMethodInstanceCPU2D : public FESMethodInstanceCPU {
FESBlocks *blocks;
SSFQueue<FESRegionWindow2D *> regions;
FeatureExtractionCPU2D *method;
FESRequestRegions *currentRequest;		// keep the current request to write the response

protected:
	void SetFeatureExtractionMethod(SSFMethodFeatureExtraction *method) { this->method = (FeatureExtractionCPU2D *) method; } 

	// return current version of the method
	string GetVersion() { return "0.0.1"; }

public:
	FESMethodInstanceCPU2D(string instanceID);

	// function to check if input modules have been set correctly
	void Setup();

	// function to launch this method
	void Launch();

	// execute
	void Execute();

	// function to generate a new instatiation of the detector
	SSFMethod *Instantiate(string instanceID);

	// method name
	string GetName() { return SSF_METHOD_FES_INSTANCE_CPU2D; }

	// method type
	string GetType() { return SSF_METHOD_TYPE_THREAD_MISC; }

	// set image
	void SetData(FESData *data, FESBlocks *blocks, FESRequestRegions *request);

	// retrieve method signature
	size_t GetExtractionSignature();

	// set a request for this instance
	void SetRequest(FESRegion *region);
};




////
// FESMethodInstanceGPU2D: 2D feature extraction for GPU
////
class FESMethodInstanceGPU2D : public FESMethodInstanceGPU {

protected:

	void ExtractFeatures(FESRequest *request);
};

#endif