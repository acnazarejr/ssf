#include "headers.h"
#include "SSFCore.h"
#include "structures.h"
#include "SMManager.h"
#include "SMDataStream.h"


/********
 * class SMDataStreamProcessed
 *******/
void SMDataStreamProcessed::SetProcessedItem(SMIndexType idx) {
	
	mutex.Lock();
	unprocSet.erase(idx);
	mutex.Unlock();
}


void SMDataStreamProcessed::SetUnprocessedItem(SMIndexType idx) {

	mutex.Lock();
	unprocSet.insert(idx);
	mutex.Unlock();
}


SMIndexType SMDataStreamProcessed::GetNewestUnprocessedItem() {
set<SMIndexType>::reverse_iterator rit;

	mutex.Lock();
	rit = unprocSet.rbegin();
	mutex.Unlock();

	return *rit;
}


SMIndexType SMDataStreamProcessed::GetOldestUnprocessedItem() {
set<SMIndexType>::iterator it;

	mutex.Lock();
	it = unprocSet.begin();
	mutex.Unlock();

	return *it;
}


SMIndexType SMDataStreamProcessed::GetNUmprocessedData() {
SMIndexType n;

	mutex.Lock();
	n = (SMIndexType) unprocSet.size();
	mutex.Unlock();

	return n;
}



/********
 * class SMDataStream
 *******/
SMDataStream::SMDataStream(SMManager *smManager) {

	mutex.Lock();
	// set the memory manager
	if (smManager == NULL) {
		mutex.Unlock();
		ReportError("Memory manager is invalid");
	}

	this->smManager = smManager;
	this->EOS = false;
	mutex.Unlock();
}


SMIndexType SMDataStream::GetNEntries() {
SMIndexType n;
	
	mutex.Lock();
	n = data.size();
	mutex.Unlock();

	return n;
}


SMIndexType SMDataStream::GetNUnprocessedEntries() {
//unordered_map<string, SMIndexType>::iterator it;
SMDataStreamProcessed *smdata;
SMIndexType maxUnproc;

	/* removed and added new approach that keeps a list of unprocessed entries for each module */
	/*mutex.Lock();
	// update the max unprocessed
	for (it = processedEntries.begin(); it != processedEntries.end(); ++it) {
		maxUnproc = max(maxUnproc, data.size() - it->second);
	}
	mutex.Unlock();
	*/

	mutex.Lock();
	smdata = GetUnprocessedEntries(glb_SSFExecCtrl.GetInstanceIDCurrentThread());
	maxUnproc = smdata->GetNUmprocessedData();
	mutex.Unlock();

	return maxUnproc;
}


SMIndexType SMDataStream::AddEntry(SSFData *entry) {
unordered_map<string, SMDataStreamProcessed *>::iterator it;
SMIndexType pos;
SMIndexType id;
SMIndexType idx; 

	mutex.Lock();
	pos = data.size();
	id = smManager->SetData(entry);
	data.push_back(id);
	accessCond.UnlockCondition(pos);
	idx = pos;
	while (accessCond.EraseCondition(--idx) == true);

	// add as an unlinked entry
	entriesID.insert(pair<SMIndexType, int>(id, 0));

	// set data as unprocessed by every module reading
	for (it = unprocessed.begin(); it != unprocessed.end(); it++)
		it->second->SetUnprocessedItem(pos);

	mutex.Unlock();

	return id;
}


SMDataStreamProcessed *SMDataStream::GetUnprocessedEntries(string modID) {
unordered_map<string, SMDataStreamProcessed *>::iterator itProc;
SMDataStreamProcessed *unprocessedMod;

	// set this data item as processed by the current module
	itProc = unprocessed.find(modID);
	if (itProc == unprocessed.end()) {
		unprocessedMod = new SMDataStreamProcessed();
		unprocessed.insert(pair<string, SMDataStreamProcessed *>(modID, unprocessedMod));
		// set all as unprocessed
		for (size_t i = 0; i < data.size(); i++)
			unprocessedMod->SetUnprocessedItem(i);
	}
	else { 
		unprocessedMod = itProc->second;
	}

	return unprocessedMod;
}


SSFData *SMDataStream::RetrieveOldestUnprocEntry() {
SMDataStreamProcessed *unprocessedMod;
SSFData *data;

	mutexEntry.Lock();
	unprocessedMod = GetUnprocessedEntries(glb_SSFExecCtrl.GetInstanceIDCurrentThread());

	data = RetrieveEntry(unprocessedMod->GetOldestUnprocessedItem());

	if (data != NULL)
		ReportStatus("# unprocessed entries: '%d' (retrieved ID: %d)", unprocessedMod->GetNUmprocessedData(), data->GetSMID());
	
	mutexEntry.Unlock();

	return data;
}


SSFData *SMDataStream::RetrieveEntry(SMIndexType index) {
unordered_map<string, size_t>::iterator it;
SMDataStreamProcessed *unprocessedMod;
SSFData *d;
SMIndexType gnent;
	
	gnent = GetNEntries();

	mutex.Lock();
	// wait for condition to be valid
	if (index >= gnent) {
		mutex.Unlock();
		accessCond.CheckCondition(index);
		mutex.Lock();  
	}

	d = smManager->GetData(data[index]);

	// increase the number of links to this data item
	if (d != NULL)
		entriesID[d->GetSMID()]++;

	processedEntries[glb_SSFExecCtrl.GetInstanceIDCurrentThread()] = index; // add the index of the last processed entry
	
	// set this data item as processed by the current module
	unprocessedMod = GetUnprocessedEntries(glb_SSFExecCtrl.GetInstanceIDCurrentThread());
	unprocessedMod->SetProcessedItem(index);
	mutex.Unlock();

	return d;
}



bool SMDataStream::IsFinished() {
bool v;

	mutex.Lock();
	v = EOS;
	mutex.Unlock();

	return v;
}

void SMDataStream::SetEndofStream() {

	mutex.Lock();
	EOS = true;
	mutex.Unlock();

	// add new entry to remove conditional locks
	AddEntry(NULL);
}


void SMDataStream::Save(SSFStorage &storage) {
#if 0
	size_t i;

	if (storage.isOpened() == false)
		ReportError("Invalid file storage!");


	storage << "SMDataStream" << "{";
	storage << "EOS" << EOS;
	storage << "nentries" << aToString(data.size());

	// write each entry
	for (i = 0; i < data.size(); i++) {

		// new level
		storage << "entry" + aToString(i) << "{";

		if (data[i] != NULL) {
		
			storage << "modID" << data[i]->GetModID();

			storage << "dataType" << data[i]->GetDataType();

			storage << "attribute" << "";

			storage << "unknownSrc" << data[i]->IsUnknownSource();

			storage << "timeStamp" << aToString(data[i]->GetTimeStamp());

			data[i]->Save(storage);
		}

		// return the level
		storage << "}";
	}
	storage << "}";
#endif
}


void SMDataStream::Load(const FileNode &node) {

}



void SMDataStream::ReleaseLink(SSFData *data) {

	mutex.Lock();
	smManager->UnlinkData(data->GetSMID());
	entriesID[data->GetSMID()]--;
	mutex.Unlock();
}


void SMDataStream::SetSynchonizationTag(SMIndexType tagID) {
SSFData *d;
SMIndexType gnent;
	
	gnent = GetNEntries();

	mutex.Unlock();
	d = smManager->GetData(data[gnent-1]);
	d->SetSynchonizationTag(tagID);
	mutex.Unlock();
}