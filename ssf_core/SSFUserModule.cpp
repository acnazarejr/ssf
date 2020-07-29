#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "SSFGarbageCollector.h"
#include "SSFUserModule.h"


/********
 * class ModParameters
 ********/
void SSFModParameters::Require(string modID, string dataType, string *modFromVariable, string attribute) {
map<string, SMDataKey>::iterator it;
SMDataKey datakey(modID, dataType, attribute);

	if (*modFromVariable != "")
		ReportError("Variable 'modFromVariable' in SSFModParameters::Require() must be an empty string!");

	// set the address of the variable that will keep the name of the module that will provide this data
	datakey.SetModFromVariable(modFromVariable);

	it = required.find(datakey.ComputeKey());
	if (it != required.end())
		ReportError("Datatype '%s' and attribute '%s' are already added, set an attribute to differenciate them", dataType.c_str(),  attribute.c_str());

	required.insert(pair<string, SMDataKey>(datakey.ComputeKey(), datakey));
}



void SSFModParameters::Require(string modID, string dataType, vector<string> *modFromVariable, string attribute) {
map<string, SMDataKey>::iterator it;
SMDataKey datakey(modID, dataType, attribute);

	// set the address of the variable that will keep the name of the module that will provide this data
	datakey.SetModFromVariable(modFromVariable);

	it = required.find(datakey.ComputeKey());
	if (it != required.end())
		ReportError("Datatype '%s' and attribute '%s' are already added, set an attribute to differenciate them", dataType.c_str(),  attribute.c_str());

	required.insert(pair<string, SMDataKey>(datakey.ComputeKey(), datakey));

}



void SSFModParameters::Provide(string modID, string dataType, string attribute) {
map<string, SMDataKey>::iterator it;
SMDataKey datakey(modID, dataType, attribute);

	it = provided.find(datakey.ComputeKey());
	if (it != provided.end())
		ReportError("Datatype '%s' and attribute '%s' are already added", dataType.c_str(),  "");

	provided.insert(pair<string, SMDataKey>(datakey.ComputeKey(), datakey));
}



vector<SMDataKey> SSFModParameters::GetRequiredDataType() {
map<string, SMDataKey>::iterator it;
vector<SMDataKey> v;

	for (it = required.begin(); it != required.end(); it++) {
		v.push_back(it->second);
	}

	return v;
}


vector<SMDataKey> SSFModParameters::GetProvidedDataType() {
map<string, SMDataKey>::iterator it;
vector<SMDataKey> v;

	for (it = provided.begin(); it != provided.end(); it++) {
		v.push_back(it->second);
	}

	return v;
}


/******
 * class SSFUserModule 
 ******/
SSFUserModule::SSFUserModule(string instanceID) : SSFMethodThread(instanceID) {

	this->launched = false;
}


void SSFUserModule::Launch() {

	// set as launched
	mutex.Lock();
	this->launched = true;
	mutex.Unlock();

	// call execution of the module
	this->Execute();
	
	// inform SM that the module is finishing its execution
	glb_sharedMem.FinishProcessingModule(this->modParams, this->entriesID);

	// garbage collector for any remaining structures
	glb_SSFGarbageCollector.CleanAllModEntries();
}



void SSFUserModule::SetupSharedMemory() {

	// data structure used to sync modules (each module provides SSF_DATA_MOD_EOD, which can be used to wait a module to finish before processing)
	modParams.Provide(this->GetInstanceID(), SSF_DATA_MOD_EOD);

	// set provide data
	glb_sharedMem.StartProcessingModule(modParams, this->GetInstanceID());
}




bool SSFUserModule::IsLaunched() {
bool v;

	mutex.Lock();
	v = this->launched;
	mutex.Unlock();

	return v;
}