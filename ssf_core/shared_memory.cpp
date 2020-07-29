#include "headers.h"
#include "SSFCore.h"
#include "parameters.h"
#include "SMManager.h"
#include "shared_memory.h"


/****
 * class SharedMemory
 ****/
SharedMemory::SharedMemory() {

	smManager = new SMManager();

	inputParams.SetStructure((SSFStructure *) smManager, "smManagerSetup", &smManagerSetup, "SetupID to configure the SMManager", true);
}



void SharedMemory::Setup() {


}





void SharedMemory::StartProcessingModule(SSFModParameters &modParams, string modID) {
vector<SMDataKey> vProv;
size_t i;

	// retrieve data types provided by this module
	vProv = modParams.GetProvidedDataType();

	mutex.Lock();
	// send EOS for each of the data types
	for (i = 0; i < vProv.size(); i++) {
		CreateDataStream(modID, vProv[i].GetDataType(), vProv[i].GetAttribute());
	}
	mutex.Unlock();
}





void SharedMemory::FinishProcessingModule(SSFModParameters &modParams, unordered_map<SMIndexType, int> &entriesID) {
multimap<string, string>::iterator it;
vector<SMDataKey> vProv;
string modID;
size_t i;

	modID = glb_SSFExecCtrl.GetInstanceIDCurrentThread();

	// retrieve data types provided by this module
	vProv = modParams.GetProvidedDataType();

	mutex.Lock();
	// send EOS for each of the data types
	for (i = 0; i < vProv.size(); i++) {
		if (RetrieveDataStream(modID, vProv[i].GetDataType(), vProv[i].GetAttribute())->IsFinished() == true)
			continue;

		// set end of stream
		RetrieveDataStream(modID, vProv[i].GetDataType(), vProv[i].GetAttribute())->SetEndofStream();
	}

	// unlink any remaining data access of this module
	smManager->UnlinkData(entriesID);
	mutex.Unlock();
}






SSFData *SharedMemory::GetData(SMIndexType id) {

	return smManager->GetData(id);
}



SSFData *SharedMemory::GetData(string fromModID, string dataType) {

	ReportWarning("Function SharedMemory::GetData(string, string) is deprecated and will be removed in version %s. Use SharedMemory::GetData(string, string, SMIndexType) instead", SSF_NEXT_VERSION);

	return this->GetData(fromModID, dataType, SM_INVALID_ELEMENT);
}


SSFData *SharedMemory::GetData(string fromModID, string dataType, SMIndexType idx) {
SMDataStream *actualData;
SSFData *retdata = NULL;
string attribute;
size_t nentries;

	if (fromModID == "")
		ReportError("Trying to retrieve type '%s' without specifying fromModID", dataType.c_str());

	SplitModIDAttribute(fromModID, attribute);

	mutex.Lock();
	actualData = RetrieveDataStream(fromModID, dataType, attribute);

	// number of available entries
	nentries = actualData->GetNEntries();

	// retrieve the first unprocessed entry
	if (idx == SM_INVALID_ELEMENT) {
		idx = 0;

		// there are no unprocessed entries, return the last one
		if (actualData->GetNUnprocessedEntries() > 0) { 
			retdata = actualData->RetrieveOldestUnprocEntry();
			if (retdata == NULL && actualData->IsFinished() == false) {
				mutex.Unlock();
				ReportError("NULL data is being set to the shared memory");
			}
			if (retdata == NULL && actualData->IsFinished() == true) {
				ReportStatus("No more data type '%s' to be read from '%s'", dataType.c_str(), fromModID.c_str());
				mutex.Unlock();
				return retdata;
			}
		}
		else {
			idx = nentries - 1;
		}
	}

	if (nentries == 0)
		idx = 0;

	// return NULL if reached the end of the stream and it is finished
	if (idx >= ((SMIndexType) nentries - 1) && actualData->IsFinished() == true) {
		mutex.Unlock();
		return NULL;
	}

	mutex.Unlock();
	// retrieve data (it locks while the data is not available)
	if (retdata == NULL)		// only retrieve if that was not done earlier by the RetrieveOldestUnprocEntry
		retdata = actualData->RetrieveEntry(idx);
	mutex.Lock();

	// update creation condition for more entries of this module + dataType
	UpdateCreationCondition(fromModID, dataType, attribute);
	mutex.Unlock();

	return retdata;
}




SMIndexType SharedMemory::SetData(SSFData *data, string attribute) {
SMDataStream *actualData;
string dataType;
string modID;
size_t idx;

	if (data == NULL)
		ReportError("Attempting to add NULL data to the shared memory");


	mutex.Lock();
	data->SetAttribute(attribute);
	dataType = data->GetDataType();

	if (data->IsUnknownSource() == true)
		modID = "*";
	else
		modID = glb_SSFExecCtrl.GetInstanceIDCurrentThread();
	 
	actualData = RetrieveDataStream(modID, dataType, attribute);

	// check if a new frame can be created, if not wait
	if (CheckCreationCondition(modID, dataType, attribute) == false) {
		mutex.Unlock();
		userDataCond.CheckCondition("Creation" + modID + dataType + attribute);
		mutex.Lock();
	}

	// set the module that added this data entry
	data->SetModuleID(modID);

	// add this entry to the stream
	idx = actualData->AddEntry(data);

	mutex.Unlock();

	return idx;
}



bool SharedMemory::CheckCreationCondition(string module, string dataType, string attribute) {
unordered_map<string, SMIndexType>::iterator it;
bool cond = true;


	// if number of unprocessed entries is larger than allowed, does not create a new entry
	it = creationConditions.find(module + dataType + attribute);
	if (it != creationConditions.end()) {
		if (dataSMMap[module + dataType + attribute]->GetNUnprocessedEntries() > it->second) {
			cond = false;
		}
	}


	return cond;
}



void SharedMemory::UpdateCreationCondition(string module, string dataType, string attribute) {
unordered_map<string, SMIndexType>::iterator itCreateCond;

	// update the creation conditions for the creation module
	itCreateCond = creationConditions.find(module + dataType + attribute);
	if (itCreateCond != creationConditions.end()) {
		if (CheckCreationCondition(module, dataType, attribute) == true) {
			userDataCond.UnlockCondition("Creation" + module + dataType + attribute);
			userDataCond.EraseCondition("Creation" + module + dataType + attribute);
		}
	}
}




void SharedMemory::CreateDataStream(string module, string dataType, string attribute) {
unordered_map<string, SMDataStream *>::iterator it;
SMDataStream *d;

	mutexRetDataStream.Lock();
	it = dataSMMap.find(module + dataType + attribute);

	if (it == dataSMMap.end()) {  // create this new data type
		d = new SMDataStream(smManager);
		dataSMMap.insert(pair<string, SMDataStream *>(module + dataType + attribute, d));
	}
	else {
		ReportWarning("Data stream for module '%s' with DataType: '%s' and attribute '%s' already exist!", module.c_str(), dataType.c_str(), attribute.c_str());
	}
	mutexRetDataStream.Unlock();
}


void SharedMemory::SplitModIDAttribute(string &modID, string &attribute) {
size_t idx;
string modtmp;

	// format of modID ModIDDataType*:*Attr
	idx = modID.find("*:*");

	if (idx == string::npos)
		return;

	modtmp = modID;
	modID.assign(modtmp, 0, idx);
	attribute.assign(modtmp, idx+3, string::npos);
}






SMDataStream *SharedMemory::RetrieveDataStream(string module, string dataType, string attribute) {
unordered_map<string, SMDataStream *>::iterator it;
SMDataStream *d = NULL;

	mutexRetDataStream.Lock();
	it = dataSMMap.find(module + dataType + attribute);
	if (it == dataSMMap.end()) {  // create this new data type
		ReportError("Data stream for module '%s' with DataType: '%s' and attribute '%s' was not declared, call modParams.Provide() in the modules constructor!", module.c_str(), dataType.c_str(), attribute.c_str());
		//d = new SMDataStream(smManager);
		//dataSMMap.insert(pair<string, SMDataStream *>(module + dataType + attribute, d));
	}
	else {
		d = it->second;
	}
	mutexRetDataStream.Unlock();

	return d;
}



size_t SharedMemory::GetNumberEntries(string modID, string dataType) {
string attribute;

	SplitModIDAttribute(modID, attribute);

	return RetrieveDataStream(modID, dataType, attribute)->GetNEntries();
}



bool SharedMemory::IsProcessingDone(string modID, string dataType) {
string attribute;

	SplitModIDAttribute(modID, attribute);
	return RetrieveDataStream(modID, dataType, attribute)->IsFinished();
}


void SharedMemory::WaitUntilModuleEnd(string modID) {

	this->GetData(modID, SSF_DATA_MOD_EOD);
}



void SharedMemory::UnlinkDataItem(SSFData *data) {

	RetrieveDataStream(data->GetModID(), data->GetDataType(), data->GetAttribute())->ReleaseLink(data);
}


void SharedMemory::DumpSharedMemory(SSFStorage &storage) {
unordered_map<string, SMDataStream *>::iterator it;
size_t idx = 0;

	if (storage.isOpened() == false)
		ReportError("Invalid file storage!");

	storage << "SharedMemory" << "{";
	storage << "test" << "test";

	/* write data stream */
	storage << "Data" << "{";
	storage << "nentries" << aToString(dataSMMap.size());

	// write each entry
	for (it = dataSMMap.begin(); it != dataSMMap.end(); it++) {
	
		// new level
		storage << "entry" + aToString(idx++) << "{";
		
		storage << "name" << it->first;

		it->second->Save(storage);

		// return the level
		storage << "}";
	}
	storage << "}";

	storage << "test" << "test";

	/* finished writting */
	storage << "}";
}


void SharedMemory::WaitModToProcess(string modID, SMIndexType tagID) {

}



SMIndexType SharedMemory::SendSynchronizationTag(string dataType, string attribute) {
string modID;
SMDataStream *actualData;
SMIndexType id = 1;

	modID = glb_SSFExecCtrl.GetInstanceIDCurrentThread();
	 
	actualData = RetrieveDataStream(modID, dataType, attribute);
	actualData->SetSynchonizationTag(id);

	return id;
}