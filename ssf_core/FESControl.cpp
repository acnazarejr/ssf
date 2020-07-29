#include "headers.h"
#include "SSFCore.h"
#include "FESMethodManager.h"
#include "FESMemoryManagement.h"
#include "FESCache.h"
#include "FES.h"
#include "FESControl.h"


FESControl instance("");


//////
// class FESControl
//////
FESControl::FESControl(string instanceID) : SSFInternalModule(instanceID) {

	memoryEntries = 1000;	

	/* parameters */
	inputParams.Add("memoryEntries", &memoryEntries, "Number of entries for memory for the FES", false);
	
	this->memory = NULL;

	this->Register(this, instanceID);
}






SSFMethod *FESControl::Instantiate(string instanceID) {

	return new FESControl(instanceID); 
}


void FESControl::Setup() {

	this->memory = new FESMemoryManagement(this->memoryEntries);
}




void FESControl::Execute() {
FESRequest *request;
vector<size_t> featSignatures;
unordered_map<size_t, FESMethodManager *>::iterator it;
size_t i;

	glb_TP.Startx();
	while (1) {
		glb_TP.Startx();
		// wait for a request
		request = requests.pop();
		glb_TP.Stopx();
		
		// end of processing
		if (request == NULL)
			break;

		featSignatures = request->GetFeatSignatures();

		glb_TP.Startx();
		mutex.Lock();
		for (i = 0; i < featSignatures.size(); i++) {
			it = featManagers.find(featSignatures[i]);
	
			if (it == featManagers.end())
				ReportError("Feature extraction method with signature '%d' is not available - set it first!", featSignatures[i]);

			// set request to the correct method
			it->second->SetRequest(request);
		}
		mutex.Unlock();
		glb_TP.Stopx();
	}
	glb_TP.Stopx(); 
}




void FESControl::SetRequestToProcess(FESRequest *request) {

	requests.push(request); 
}



size_t FESControl::LaunchExtractionMethodManager(SSFParameters *params, string setupID) {
unordered_map<size_t, FESMethodManager *>::iterator it;
FESMethodManager *method;
size_t signature;

	mutex.Lock();
	// create a new instance of the method manager
	method = (FESMethodManager *) glb_SSFExecCtrl.SSFMethodControl_InstanciateMethod(SSF_METHOD_FES_MANAGER, 
			SSF_METHOD_TYPE_THREAD_MISC, setupID);

	// set parameters for the new manager
	((SSFMethod *) method)->Setup(params, setupID);

	// launch extraction methods
	method->LaunchFeatureInstances(params);

	// retrieve unique signature
	signature = method->GetExtractionMethodSignature();

	// check if this specific setup is already set, if so, use the existing one
	it = featManagers.find(signature);

	if (it != featManagers.end()) {
		ReportWarning("This feature setup has already been set!");
		// ??? Remove all stuff that has been allocated

		// do not need the method - repeated
		delete method;

		mutex.Unlock();
		return signature;
	}

	// add method to the list
	featManagers.insert(pair<size_t, FESMethodManager *>(signature, method));

	// set memory manager
	method->SetMemoryManagerPointer(this->memory);

	// launches method
	glb_SSFExecCtrl.SSFMethodControl_LaunchMethod(method);

	mutex.Unlock();

	return signature;
}



void FESControl::RequestFinish() {

	// set a NULL request
	requests.push(NULL);
}
