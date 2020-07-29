#include "headers.h"
#include "SSFCore.h"
#include "SMDataStream.h"
#include "SMManager.h"



SMManager::SMManager() {

	maxMemory = 500000000;
	directory = "";
	nAllocatedBytes = 0;

	// add parameters
	inputParams.Add("maxMemory", &maxMemory, "Maximum number of Bytes to keep in RAM", false);
	inputParams.Add("directory", &directory, "Directory where the images will be stored", true);
}



void SMManager::Setup() {

}



void SMManager::RecoverData(SMIndexType idx) {

	// fetch from disk

	// mutex.Lock() to add the result to the dataEntries
}


void SaveData() {

	// only save data items that have no links to it

	// apply to police (least used)

	// free and set to NULL the entries saved to the disk

}


SMIndexType SMManager::SetData(SSFData *data) { 
SMIndexType id;

	mutex.Lock();
	id = dataEntries.size();
	dataEntries.push_back(data);
	if (data != NULL) {
		data->SetSMID(id);
		nlinks.push_back(0);
	}
	else {
		nlinks.push_back(1);
	}
	mutex.Unlock();

	return id;
}



SSFData *SMManager::GetData(SMIndexType id) {
SSFData *data;

	mutex.Lock();
	if (id >= (SMIndexType) dataEntries.size()) {
		mutex.Unlock();
		ReportError("Invalid memory position");
	}

	// if data is not available, recover from disk
	if (dataEntries[id] == NULL) {
		mutex.Unlock();
		RecoverData(id);
		mutex.Lock();
	}

	// get data content
	data = dataEntries[id];

	// increase one link to it
	nlinks[id]++;
	mutex.Unlock();

	return data;
}


void SMManager::UnlinkData(SMIndexType id) {
	
	mutex.Lock();
	if (nlinks[id] == 0) {
		mutex.Unlock();
		ReportError("Data entry '%d' has been unlinked more than linked - inconsistent!", id);
	}

	// reduce the number of links by one
	nlinks[id]--;
	mutex.Unlock();
}



void SMManager::UnlinkData(unordered_map<SMIndexType, int> &entriesID) {
unordered_map<SMIndexType, int>::iterator it;

	mutex.Lock();
	for (it = entriesID.begin(); it != entriesID.end(); it++) {
		if (nlinks[it->first] < it->second) {
			mutex.Unlock();
			ReportError("Data entry '%d' has been unlinked more than linked - inconsistent!", it->first);
		}

		nlinks[it->first] -= it->second;
	}
	mutex.Unlock();
}