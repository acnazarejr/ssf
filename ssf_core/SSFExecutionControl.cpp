#include "headers.h"
#include "SSFCore.h"
#include "SSFParameters.h"
#include "SSFExecutionSequence.h"
#include "FESControl.h"
#include "SSFThreads.h"
#include "LAP.h"
#include "SSFExecutionControl.h"





/****
 * Class SSFExecutionControl
 ****/
SSFExecutionControl::SSFExecutionControl() {

	ssfMethodControl = new SSFMethodControl();

	/* parameters */
	inputParams.Add("internalModulesIDs", &internalModulesIDs, "Identifiers of the internal modules to launch", true);
	inputParams.Add("userModulesIDs", &userModulesIDs, "Identifiers of the user modules that will be launched in the beginning", true);
	//inputParams.Add("SMID", &SMID, "Identifier for the shared memory setup", false);
	inputParams.Add("execSeqs", &execSeqs, "Identifiers of the execution sequences", false);
	inputParams.SetStructure((SSFStructure *)&glb_sharedMem, "SMID", &SMID, "Identifier for the shared memory setup", true);

	// set the main thread
	SSFMethodControl_SetThreadID("main");

	parameters = new SSFParameters();
}


SSFExecutionControl::~SSFExecutionControl() {
	

}



void SSFExecutionControl::Initialize(string paramFile, string controlID) {

	// set parameters
	this->parameters->SetFromFile(paramFile); 

	this->Initialize(this->parameters, controlID);
}



void SSFExecutionControl::Initialize(SSFParameters *params, string controlID) {
unordered_map<string, string> globalModMap;

	// set parameters
	this->parameters = params;

	// setup execution control
	((SSFStructure *)this)->Setup(params, controlID);

	// shared memory setup
	//((SSFStructure *)&glb_sharedMem)->Setup(parameters, SMID);


	// launch internal modules
	this->LaunchInternalModules();

	// instantiate user modules that will start from the beginning
	this->InstantiateReadyUserModules();

	// execute pipelines
	this->SetPipelines();



	/*
	// execute initial pipelines
	ExecutePipelines(params);



	// wait until the internal modules have been finished
	ctrlThreadsIntMods.WaitExecutionToFinish();
	*/
	// correct mapping to modules have the actual module names
	this->CorrectInstanceIDsUserModules();

	// before launching setup the shared memory with provided data types
	this->UserModulesSetupSharedMemory();

	// launch all user modules
	this->LaunchUserModules();
}


void SSFExecutionControl::Setup() {

}



void SSFExecutionControl::SetInstantiatedStructure(SSFStructure *structure) {

	ssfMethodControl->SetInstantiatedStructure(structure);
}





SSFMethodThread *SSFExecutionControl::RetrieveInternalModule(string methodName, bool ErrorMsg) {
unordered_map<string, SSFInternalModule *>::iterator it;

	mutex.Lock();
	it = internalModules.find(methodName);
	mutex.Unlock();

	if (it != internalModules.end())
		return it->second;

	if (ErrorMsg == true)
		ReportError("Internal module with name '%s' was not found (check if it has been set in the config file)", methodName.c_str());

	return NULL;
}


void SSFExecutionControl::LaunchInternalModules() {
string id, type, name;
SSFInternalModule *method;
size_t i;

	mutex.Lock();
	for (i = 0; i < internalModulesIDs.size(); i++) {

		// retrieve information
		id = internalModulesIDs[i];
		type = parameters->GetValue(id, "type");
		name = parameters->GetValue(id, "method");

		// check if this is really and internal module
		if (type != SSF_METHOD_TYPE_INTMOD)
			ReportError("Module '%s' is not an internal module '%s'", id.c_str(), type.c_str());

		// instantiate
		method = (SSFInternalModule *) glb_SSFExecCtrl.SSFMethodControl_InstanciateMethod(name, type, id);

		// setup method
		method->Setup(parameters, id);

		// launch method
		SSFMethodControl_LaunchMethod((SSFMethodThread *) method);

		// add it to the intermal modules list
		internalModules.insert(pair<string, SSFInternalModule *>(name, (SSFInternalModule *) method));
	}
	mutex.Unlock();
}



void SSFExecutionControl::InstantiateReadyUserModules() {
SSFUserModule *method;
string id, type, name;
size_t i;

	mutex.Lock();
	for (i = 0; i < userModulesIDs.size(); i++) {

		// retrieve information
		id = userModulesIDs[i];
		type = parameters->GetValue(id, "type");
		name = parameters->GetValue(id, "method");

		// check if this is really and internal module
		if (type != SSF_METHOD_TYPE_USERMOD)
			ReportError("Module '%s' is not an internal module '%s'", id.c_str(), type.c_str());

		// instantiate method
		method = (SSFUserModule *) glb_SSFExecCtrl.SSFMethodControl_InstanciateMethod(name, type, id);

		// setup method
		method->Setup(parameters, id);

		// add it to the intermal modules list
		userModules.insert(pair<string, SSFUserModule *>(id, method));
	}
	mutex.Unlock();
}


void SSFExecutionControl::SetPipelines() {
ParamsExecution exec;
ParamsPipeline pipe;
string execID;
size_t i;

	mutex.Lock();
	// set all execution sequences
	for (i = 0; i < execSeqs.size(); i++) {

		// ID
		execID = execSeqs[i];

		// info to the user
		ReportStatus("Seting execution sequence '%s'", execID.c_str());

		// retrieve parameters
		exec = parameters->RetrieveExecutionSequence(execID);
		pipe = parameters->RetrievePipeline(exec.pipeID);

		// create execution sequence
		CreateExecutionSequence(this->parameters, exec, pipe);
	}
	mutex.Unlock();
}



void SSFExecutionControl::CreateExecutionSequence(SSFParameters *params, ParamsExecution execParams, ParamsPipeline pipeParams) {
map<string, string>::iterator it, it2;
SSFExecutionSequence *info;
map<string, string> pipeMap;
vector<SSFUserModule *> modules;
size_t i;

	
	// allocation of the execution sequence
	info = new SSFExecutionSequence(execParams, pipeParams, execSeqControl.size());
	execSeqControl.insert(pair<string, SSFExecutionSequence *>(execParams.execID, info));

	// instantiate modules in the pipeline
	info->InstanciateModules(params, this->ssfMethodControl);

	// set modules dependencies
	info->SetModulesDependencies();

	// setup for all modules
	info->SetupModules(params);
	// retrieve pipeline module mapping
	pipeMap = info->RetrieveOutputToInstanceIDMapping();

	// add mapping to the global mapping
	for (it = pipeMap.begin(); it != pipeMap.end(); it++) {
		
		// check if entry is already in the map - error
		it2 = instanceIDsMap.find(it->first);
		if (it2 != instanceIDsMap.end())
			ReportError("Duplicated entry '%s' in the map", it->first.c_str());

		// insert into the map
		instanceIDsMap.insert(pair<string,string>(it->first, it->second));
	}

	// retrieve modules that were instantiated and add into the list
	modules = info->RetrieveModules();
	for (i = 0; i < modules.size(); i++) {
		userModules.insert(pair<string, SSFUserModule *>(modules[i]->GetInstanceID(), modules[i]));
	}	
}



void SSFExecutionControl::CorrectInstanceIDsUserModules() {
unordered_map<string, SSFUserModule *>::iterator it;
map<string, string>::iterator it2;
vector<SMDataKey> reqData;
SSFUserModule *module;
SMDataKey *req;
string id;
size_t i;

	mutex.Lock();
	for (it = userModules.begin(); it != userModules.end(); it++) {
		module = it->second;

		// check if the module is already launched, only correct if it has not been launched
		if (module->IsLaunched() == true)
			continue;

		// retrieve all input data for the current module
		reqData = module->modParams.GetRequiredDataType();

		for (i = 0; i < reqData.size(); i++) {
			req = &(reqData[i]);
			id = req->GetModFromVariableValue();

			// no module has been set for this input link
			if (id == "")
				continue;

			// search if there is a translation for the used instanceID
			it2 = instanceIDsMap.find(id);
			if (it2 != instanceIDsMap.end())
				req->SwapModFromVariableValue(id, it2->second); // set the translation
		}
	}
	mutex.Unlock();
}



void SSFExecutionControl::LaunchUserModules() {
unordered_map<string, SSFUserModule *>::iterator it;
SSFUserModule *module;

	mutex.Lock();
	for (it = userModules.begin(); it != userModules.end(); it++) {
		module = it->second;

		// check if the module is already launched, only correct if it has not been launched
		if (module->IsLaunched() == true)
			continue;

		SSFMethodControl_LaunchMethod((SSFMethodThread *) module);
	}
	mutex.Unlock();
}


void SSFExecutionControl::UserModulesSetupSharedMemory() {
unordered_map<string, SSFUserModule *>::iterator it;
SSFUserModule *module;

	mutex.Lock();
	for (it = userModules.begin(); it != userModules.end(); it++) {
		module = it->second;

		// check if the module is already launched, only correct if it has not been launched
		if (module->IsLaunched() == true)
			continue;

		module->SetupSharedMemory();
	}
	mutex.Unlock();

}


void SSFExecutionControl::SSFMethodControl_RegisterMethod(string methodName, string methodType, SSFMethod *methodPtr) { ; 

	ssfMethodControl->RegisterMethod(methodName, methodType, methodPtr);
}



SSFMethod *SSFExecutionControl::SSFMethodControl_InstanciateMethod(string methodName, string methodType, string instanceID) {

	return ssfMethodControl->InstanciateMethod(methodName, methodType, instanceID);
}


void SSFExecutionControl::SSFMethodControl_LaunchMethod(SSFMethodThread *method) {

	if (method->EnabletoLaunch() == false) {
		mutex.Unlock();
		ReportError("Method '%s' is not launchable!", method->GetName().c_str());
	}

	ssfMethodControl->LaunchThread(method);

	// if method is a user module, add it to the queue
	if (method->GetType() == SSF_METHOD_TYPE_USERMOD)
		runningUserModules.push((SSFUserModule *) method);
}


void SSFExecutionControl::SSFMethodControl_SetThreadID(string instanceID) {
void *ptr;

	mutexMapThreads.Lock();
	ptr = SSFThreadGetSelf();
	mapThreads.insert(pair<void *, string>(ptr, instanceID));
	mutexMapThreads.Unlock();
}






string SSFExecutionControl::GetInstanceIDCurrentThread(bool setError) {
map<void *, string>::iterator it;
void *ptr;
string name;

	mutexMapThreads.Lock();
	ptr = SSFThreadGetSelf();

	it = mapThreads.find(ptr);

	if (it == mapThreads.end() && setError == true) {
		mutexMapThreads.Unlock();
		printf("The current thread %p is unknown!\n", ptr);
		exit(2);
	}
	else if (it == mapThreads.end() && setError == false) {
		mutexMapThreads.Unlock();
		return "";
	}

	name = it->second;
	mutexMapThreads.Unlock();

	return name;
}



size_t SSFExecutionControl::FES_InitializeExtractionMethod(string setupID, string paramFile) {
FESControl *method;
SSFParameters params;
size_t signature;

	method = (FESControl *) RetrieveInternalModule(SSF_METHOD_INTERNALMOD_FES, true);

	// read parameters
	params.SetFromFile(paramFile);

	signature = method->LaunchExtractionMethodManager(&params, setupID);

	return signature;
}


void SSFExecutionControl::FES_SetRequest(FESRequest *request) {
FESControl *method;

	method = (FESControl *) RetrieveInternalModule(SSF_METHOD_INTERNALMOD_FES, true);

	method->SetRequestToProcess(request);
}


void SSFExecutionControl::KillRunningThreads() {

	ssfMethodControl->KillRunningThreads();
}


void SSFExecutionControl::WaitModulesToEnd() {
unordered_map<string, SSFUserModule *>::iterator itUserMod;
SSFUserModule *module;

	// first wait until all user modules to finish
	while (1) {
		module = runningUserModules.pop();

		// wait until current module end
		ssfMethodControl->WaitUserModuletoFinish(module->GetInstanceID());

		// check if the queue is empty
		mutex.Lock();
		if (runningUserModules.size() == 0) {
			mutex.Unlock();
			break;
		}
		mutex.Unlock();
	}

	ReportStatus("All user modules have been ended!");

	// request the end of all other methods
	ssfMethodControl->RequestEndThreadMethods();
}



void SSFExecutionControl::SaveMethodsParameters(string filename) {
SSFStorage storage;

	mutex.Lock();
	storage.open(filename, SSF_STORAGE_WRITE);
	
	if (storage.isOpened() == false)
		ReportError("Could not open file '%s' to write!", filename.c_str());

	ssfMethodControl->SaveParameters(storage);

	storage.release();
	mutex.Unlock();
}


void SSFExecutionControl::ListUserModulesAvailable() {

	ssfMethodControl->ListUserModulesAvailable();
}