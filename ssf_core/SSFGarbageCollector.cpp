#include "headers.h"
#include "SSFCore.h"
#include "SSFData.h"
#include "SSFGarbageCollector.h"



SSFGarbageCollector::SSFGarbageCollector() {


}


SSFGarbageCollector::~SSFGarbageCollector() {

}



void SSFGarbageCollector::AddEntry(SSFData *entry) {
unordered_map<string, SSFGarbageMod *>::iterator it;
SSFGarbageMod *garbMod;
string modID;

	modID = glb_SSFExecCtrl.GetInstanceIDCurrentThread(false);

	if (modID == "")
		return;

	mutex.Lock();
	it = data.find(modID);
	if (it == data.end()) {
		garbMod = new SSFGarbageMod();
		data.insert(pair<string, SSFGarbageMod *>(modID, garbMod));
	}
	else {
		garbMod = it->second;
	}

	garbMod->entries.insert(entry);
	mutex.Unlock();
}




void SSFGarbageCollector::RemoveEntry(SSFData *entry) {
unordered_map<string, SSFGarbageMod *>::iterator it;
SSFGarbageMod *garbMod;
string modID;

	modID = glb_SSFExecCtrl.GetInstanceIDCurrentThread();

	mutex.Lock();
	it = data.find(modID);
	if (it == data.end()) 
		ReportError("Invalid status - module ID must have been found");
	garbMod = it->second;
	garbMod->entries.erase(entry);
	mutex.Unlock();
}



void SSFGarbageCollector::CleanAllModEntries() {
unordered_map<string, SSFGarbageMod *>::iterator it;
unordered_set<SSFData *>::iterator setIt;
SSFGarbageMod *garbMod;
string modID;

	modID = glb_SSFExecCtrl.GetInstanceIDCurrentThread();

	mutex.Lock();
	it = data.find(modID);
	if (it == data.end()) {
		ReportStatus("Garbage collection [No SSF data allocation for this module]");
		mutex.Unlock();
		return;
	}
	
	garbMod = it->second;

	if (garbMod->entries.size() > 0) {
		// list which are the types
		for (setIt = garbMod->entries.begin(); setIt != garbMod->entries.end(); setIt++) {
			printf("[%s] Data type: '%s' unreleased\n", modID.c_str(), (*setIt)->GetDataType().c_str());
		}
		ReportError("Garbage collection [Memory leak (%d SSF entries)]", garbMod->entries.size());
	}


	ReportStatus("Garbage collection [no memory leak]");

	mutex.Unlock();
}