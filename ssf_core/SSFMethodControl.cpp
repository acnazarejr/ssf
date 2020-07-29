#include "headers.h"
#include "SSFCore.h"
#include "SSFMethodControl.h"





/*********************
 * class SSFMethodType 
 *********************/
SSFMethod *SSFMethodType::RetrieveMethod(string methodName) {
unordered_map<string, SSFMethod *>::iterator it;

	it = methodList.find(methodName);
	if (it != methodList.end())
		return it->second;

	ReportError("Method name '%s' was not found", methodName.c_str());

	return NULL;
}




SSFMethod *SSFMethodType::InstanciateMethod(string methodName, string instanceID) {
SSFMethod *method;

	// retrieve method pointer
	method = RetrieveMethod(methodName);

	// create new instance
	return method->Instantiate(instanceID);
}


void SSFMethodType::RegisterModule(string methodName, SSFMethod *methodPtr) {
unordered_map<string, SSFMethod *>::iterator it;
string modID;

	it = methodList.find(methodName);
	if (it == methodList.end()) {
		methodList.insert(pair<string, SSFMethod *>(methodName, methodPtr));
	}
	else {
		ReportError("Method '%s' is already registered", methodName.c_str());
	}
}





/************************
 * class SSFMethodControl 
 ************************/
string SSFMethodControl::GenerateInstanceID(string methodName, string methodType) {
unordered_map<string, SSFMethod *>::iterator it;
string instanceID;
int i;

	for (i = 0; ; i++) {
		instanceID = methodType + "-" + methodName + "-" + aToString(i, "%5.5d");
		it = instantiatedMethods.find(instanceID);
		if (it == instantiatedMethods.end())
			break;	// found the instanceID
	}

	return instanceID;
}


void SSFMethodControl::RegisterMethod(string methodName, string methodType, SSFMethod *methodPtr) {
unordered_map<string, SSFMethodType *>::iterator it;
SSFMethodType *type;

	mutex.Lock();
	it = methodTypes.find(methodType);
	if (it == methodTypes.end()) {
		type = new SSFMethodType();
		methodTypes.insert(pair<string, SSFMethodType *>(methodType, type));
	}
	else {
		type = it->second;
	}

	type->RegisterModule(methodName, methodPtr);
	mutex.Unlock();
}


SSFMethod *SSFMethodControl::InstanciateMethod(string methodName, string methodType, string instanceID) {
unordered_map<string, SSFMethodType *>::iterator it;
unordered_map<string, SSFMethod *> ::iterator itAll;
SSFMethod *newInstance;
SSFMethodType *type;

	mutex.Lock();
	// check if need to generate instanceID
	if (instanceID == "")
		instanceID = GenerateInstanceID(methodName, methodType);

	// check if this instanceID is already used
	itAll = instantiatedMethods.find(instanceID);
	if (itAll != instantiatedMethods.end()) {
		mutex.Unlock();
		ReportError("Method with ID '%s' is already intantiated", instanceID.c_str());
	}

	// locate method type
	it = methodTypes.find(methodType);
	if (it == methodTypes.end()) {
		mutex.Unlock();
		ReportError("Method type '%s' does not exist", methodType.c_str());
	}
	
	type = it->second;
	newInstance = type->InstanciateMethod(methodName, instanceID);

	ReportStatus("Instantiating thread '%s'", newInstance->GetInstanceID().c_str());

	// add this instance to the all instances map
	instantiatedMethods.insert(pair<string, SSFMethod *>(instanceID, newInstance));
	mutex.Unlock();


	return newInstance;
}


void SSFMethodControl::LaunchThread(SSFMethodThread *method) {

	mutex.Lock();

	// launch thread for this instance
	method->InitializeThread();

	// set to the mapping
	runningMethods.insert(pair<string, SSFMethodThread *>(method->GetInstanceID(), method));

	ReportStatus("Launching thread '%s'", method->GetInstanceID().c_str());

	mutex.Unlock();
}


void SSFMethodControl::KillRunningThreads() {
unordered_map<string, SSFMethodThread *>::iterator it;
SSFMethodThread *method;

	mutex.Lock();
	for (it = runningMethods.begin(); it != runningMethods.end(); it++) {
		method = it->second;
		method->KillExecution();
		delete method;
	}

	runningMethods.clear();
	mutex.Unlock();
}


void SSFMethodControl::RequestEndThreadMethods() {
unordered_map<string, SSFMethodThread *>::iterator it;
SSFMethodThread *method;

	mutex_finish.Lock();
	mutex.Lock();
	for (it = runningMethods.begin(); it != runningMethods.end(); it++) {
		method = it->second;
		
		// request thread to finish
		method->RequestFinish();

		ReportStatus("Waiting '%s' method '%s' to end...", method->GetType().c_str(), method->GetInstanceID().c_str());

		// wait until the thread ends
		method->ssfThreadExecute->WaitToFinish();
	}

	runningMethods.clear();

	ReportStatus("All SSFMethodThread have been finished!");

	mutex.Unlock();
	mutex_finish.Unlock();
}


void SSFMethodControl::WaitUserModuletoFinish(string instanceID) {
unordered_map<string, SSFMethodThread *>::iterator it;
SSFMethodThread *method;

	mutex.Lock();
	it = runningMethods.find(instanceID);

	// method is not running
	if (it == runningMethods.end()) {
		mutex.Unlock();
		ReportError("Method with instanceID '%s' is not running", instanceID.c_str());
	}
	mutex.Unlock();

	method = it->second;

	// check if the method is actually an user module
	if (method->GetType() != SSF_METHOD_TYPE_USERMOD)
		ReportError("Method '%s' is not a user module", instanceID.c_str());


	/* wait until module to end */
	mutex_finish.Lock();

	// request thread to finish
	method->RequestFinish();

	ReportStatus("Waiting for user module '%s' (%s) to finish ...", method->GetInstanceID().c_str(), method->GetName().c_str());

	// wait until the thread ends
	method->ssfThreadExecute->WaitToFinish();

	ReportStatus("User module '%s' (%s) has concluded its execution.", method->GetInstanceID().c_str(), method->GetName().c_str());
	
	mutex_finish.Unlock();


	/* remove entry from the runningMethods */
	mutex.Lock();
	it = runningMethods.find(instanceID);
	runningMethods.erase(it);
	mutex.Unlock();
}


void SSFMethodControl::SaveParameters(SSFStorage &storage) {
unordered_map<string, SSFMethod *>::iterator it;
int i;

	if (storage.isOpened() == false)
		ReportError("Invalid file storage!");

	// save structures
	for (i = 0; i < instantiatedStructure.size(); i++) {
		instantiatedStructure[i]->SaveParameters(storage);
	}

	// save methods
	for (it = instantiatedMethods.begin(); it != instantiatedMethods.end(); it++) {
		it->second->SaveParameters(storage);
	}
}


void SSFMethodControl::SetInstantiatedStructure(SSFStructure *structure) {

	if (structure->IsSSFMethod() == true)
		ReportError("'%s' should not be a SSFMethod!", structure->GetName());

	instantiatedStructure.push_back(structure);
}


void SSFMethodControl::ListUserModulesAvailable() {
unordered_map<string, SSFMethodType *>::iterator itType;
map<string, vector<string> > modules;

	itType = methodTypes.find(SSF_METHOD_TYPE_USERMOD);
	if (itType == methodTypes.end())
		ReportError("No user modules were found");
}