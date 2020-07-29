#include "headers.h"
#include "SSFCore.h"
#include "FESCache.h"
#include "FESMemoryManagement.h"
#include "FES.h"
#include "FESMethodInstance.h"
#include "FESMethodManager.h"


FESMethodManager instance("");


/******
 * class ExtractionMethodManager
 ******/
FESMethodManager::FESMethodManager(string instanceID) : SSFMethodThread(instanceID) {

	mutex.Lock();
	/* local variables */
	ninstances = 1;
	useCache = true;
	nCacheEntries = 1000;
	nCacheBuckets = 1;
	extractionMethod = "";
	processingType = FES_PROCESSING_INVALID;
	dataType = FES_DATATYPE_INVALID;

	// set pointer to the feature extraction memory management
	this->memory = NULL;
	this->cache = NULL;

	/* parameters */
	inputParams.Add("ninstances", &ninstances, "Number of processing instances to extract features", false);
	inputParams.Add("useCache", &useCache, "Set to use cache", false);
	inputParams.Add("nCacheEntries", &nCacheEntries, "Set the maximum number of cache entries", false);
	inputParams.Add("nCacheBuckets", &nCacheBuckets, "Number of divisions for the cache", false);
	inputParams.Add("extractionSetup", &extractionSetup, "Feature extraction setup used to extract features", true);
	inputParams.Add("extractionMethod", &extractionMethod, "Name of the feature extraction method", true);

	/* register the method */
	this->Register(this, instanceID);

	mutex.Unlock();
}


void FESMethodManager::SetMemoryManagerPointer(FESMemoryManagement *memory) {

	this->memory = memory;
}



SSFMethod *FESMethodManager::Instantiate(string instanceID) {
	
	return new FESMethodManager(instanceID); 
}



void FESMethodManager::Setup() {


}


size_t FESMethodManager::GetExtractionMethodSignature() {

	if (instances.size() == 0)
		ReportError("Must instantiate methods first");

	return instances[0]->GetExtractionSignature();
}


void FESMethodManager::SetRequestProcessed(int instanceID) {

	availableProcessingUnit.push((size_t) instanceID);
}





void FESMethodManager::ProcessRegionsRequest(FESRequestRegions *request) {
FESRegion *region;
SSFMultipleFeatures *feat;
size_t nextProcessingUnit;
FESBlocks *blocks = NULL;
size_t i, idx;
int index = 0;

	glb_TP.Startx();
	// check if blocks where filled, if not, create blocks of data size
	if (request->GetBlocks() == NULL) {
		if (request->GetData()->GetDataType() == FES_DATATYPE_2D) {
			blocks = (FESBlocks *) new FESBlocks2D(((ExtractionData2D *) request->GetData())->GetImage());
		}
		else {
			ReportError("Not implemented yet!");
		}
	}
	else {
		blocks = request->GetBlocks();
	}

	// set cache if it is to be used, reset it to the new image size - cache only for 2D
	if (request->GetData()->GetDataType() == FES_DATATYPE_2D) {
		if (useCache == true) {
			cache->ResetCache((ExtractionData2D *) request->GetData());
		}
	}

	// set data element for all processing units
	glb_TP.Startx();
	for (i = 0; i < instances.size(); i++) {
		((FESMethodInstanceCPU *) instances[i])->SetData(request->GetData(), blocks, request);
	}
	glb_TP.Stopx();

	glb_TP.Startx();
	// set all regions to be processed
	while (1) {
		// retrieve next region to be processed
		glb_TP.Startx();
		region = request->GetRegionToProcess();
		glb_TP.Stopx();

		// no more regions to be processed
		if (region == NULL)
			break;

		// retrieve the next processing unit available
		glb_TP.Startx();
		nextProcessingUnit = availableProcessingUnit.pop();
		glb_TP.Stopx();

		// request memory for features
		glb_TP.Startx();
		feat = memory->AllocateEntry();
		glb_TP.Stopx();

		// set feature to the region
		region->SetFeatures(feat, memory);
		

		// set the current element to one of the feature instances
		glb_TP.Startx();
		((FESMethodInstanceCPU *) instances[nextProcessingUnit])->SetRequest(region);
		glb_TP.Stopx();

		//ReportStatus("[%d] Sending request to unit %d for method '%s' (id %s)", index++, nextProcessingUnit, 
		//	instances[nextProcessingUnit]->GetName().c_str(),  instances[nextProcessingUnit]->GetInstanceID().c_str());
	}
	glb_TP.Stopx();

	// wait until all processing units are ready to process
	idx = 0;
	do {
		availableProcessingUnit.pop();
		idx++;
	} while (idx < instances.size());


	// set processing units back to available
	for (i = 0; i < instances.size(); i++) {
		availableProcessingUnit.push(i);
	}

	// release blocks if necessary (when created here)
	if (request->GetBlocks() == NULL) {
		blocks->ClearMemory();
		delete blocks;
	}
	glb_TP.Stopx();
}


void FESMethodManager::ProcessInterestPointsRequest(FESRequestInterestPoints *request) {


}


void FESMethodManager::ProcessGPURequest(FESRequest *request) {
size_t nextProcessingUnit;

	nextProcessingUnit = availableProcessingUnit.pop();

	//((FESMethodInstanceGPU *) instances[nextProcessingUnit])->se(request);
}

void FESMethodManager::Launch() {

	this->Execute();

}

void FESMethodManager::RequestFinish() {

	// report cache status
	if (cache != NULL)
		cache->ReportCacheStatus();

	// set a NULL request
	requests.push(NULL);
}



void FESMethodManager::Execute() {
FESRequest *request;

	// execute until finish request has been sent
	while (1) {

		request = requests.pop();

		if (request == NULL)
			break; // end of processing

		// finish has not been set, execute request	
		mutex.Lock();

		// if this feature extraction method is in GPU, set  the request
		if (processingType == FES_PROCESSING_GPU)
			this->ProcessGPURequest(request);

		// if request it for regions execute it 	
		else if (request->GetRequestType() == FES_REQUESTTYPE_REGIONS) 
			this->ProcessRegionsRequest((FESRequestRegions *) request);

		// if request is for interest points execute it
		else if (request->GetRequestType() == FES_REQUESTTYPE_INTPOINTS)
			this->ProcessInterestPointsRequest((FESRequestInterestPoints *) request);

		else {
			ReportError("Invalid request type: %d!", request->GetRequestType());
		}

		mutex.Unlock();
	}
}



void FESMethodManager::SetRequest(FESRequest *request) {

	// set request and wait until it has been processed
	requests.push(request);
}




void FESMethodManager::LaunchFeatureInstances(SSFParameters *params) {
SSFMethodFeatureExtraction *method = NULL;
FESMethodInstance *methodInstance;
int i;

	mutex.Lock();

	// create all instances
	for (i = 0; i < ninstances; i++) {

		methodInstance = NULL;

		// instantiate and setup the feature extraction method
		method = (SSFMethodFeatureExtraction *) glb_SSFExecCtrl.SSFMethodControl_InstanciateMethod(this->extractionMethod, 
					SSF_METHOD_TYPE_FEATURE, "");

		// setup extraction method
		method->Setup(params, this->extractionSetup);

		/* set instance according to the type */
		// CPU
		if (method->GetProcessingType() == FES_PROCESSING_CPU) {

			this->processingType = FES_PROCESSING_CPU;

			// 2D
			if (method->GetDataType() == FES_DATATYPE_2D) {

				this->dataType = FES_DATATYPE_2D;

				// set cache if it is to be used
				if (useCache == true) {
					// initialize cache
					if (this->cache == NULL)
						this->cache = new FESCache(nCacheBuckets, nCacheEntries);
				}

				// instantiate method 
				methodInstance = (FESMethodInstance *) glb_SSFExecCtrl.SSFMethodControl_InstanciateMethod(SSF_METHOD_FES_INSTANCE_CPU2D, 
					SSF_METHOD_TYPE_THREAD_MISC, "");
			}
		}

		// GPU
		else if (method->GetProcessingType() == FES_PROCESSING_GPU) {

			this->processingType = FES_PROCESSING_GPU;
		}

		if (methodInstance == NULL)
			ReportError("Couldnt instantiate method instance!");

		// set feature method
		methodInstance->SetFeatureExtractionMethod(method);

		// set its ID number
		methodInstance->SetInstanceID(i);

		// set cache
		methodInstance->SetCache(cache);

		// set the extraction manager
		methodInstance->SetExtractionManager(this);

		// add it to the 
		availableProcessingUnit.push(i);

		// add instance to the vector
		instances.push_back(methodInstance);
	}

	// launch all instances
	for (i = 0; i < instances.size(); i++) {
		glb_SSFExecCtrl.SSFMethodControl_LaunchMethod(instances[i]);
	}

	mutex.Unlock();
}