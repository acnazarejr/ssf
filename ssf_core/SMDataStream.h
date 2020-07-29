#ifndef _SM_DATASTREAM_H_
#define _SM_DATASTREAM_H_
#include "SSFData.h" 


class SMManager;


// class to keep track of unprocessed data (by each module that is reading)
class SMDataStreamProcessed {
set<SMIndexType> unprocSet;
SSFMutex mutex;

public:
	void SetProcessedItem(SMIndexType idx);

	void SetUnprocessedItem(SMIndexType idx);

	SMIndexType GetNewestUnprocessedItem();

	SMIndexType GetOldestUnprocessedItem();

	SMIndexType GetNUmprocessedData();
};




// class used to set the communication among modules using a stream of data
class SMDataStream {
friend class SharedMemory;
SMManager *smManager;
SSFMutex mutex, mutexEntry;
CondMutexManager<SMIndexType> accessCond;				// condition to access the data (if an entry is not available, wait until it is)
unordered_map<string, SMIndexType> processedEntries;	// keep track of the last position that have been processed
unordered_map<string, SMDataStreamProcessed *> unprocessed;	// track of unprocessed data
unordered_map<SMIndexType, int> entriesID;				// keep entries that haven't be released
vector<size_t> data;									// data written in this module
bool EOS;												// no more data will be written

	// retrieve control for the specific module - if modID still haven't retrieved data, create a control
	SMDataStreamProcessed *GetUnprocessedEntries(string modID);

protected:
	// add a new entry (return its index)
	SMIndexType AddEntry(SSFData *entry);

	// retrieve a entry at position index, if lock is true, it will only retrieve data when index + 1 be available
	SSFData *RetrieveEntry(SMIndexType index);

	// retrieve the oldest unprocessed entry (according to the module that is calling)
	SSFData *RetrieveOldestUnprocEntry();

	// return the number of items 
	SMIndexType GetNEntries();

	// return the number of unprocessed entries (considering the slowest method processing the data)
	SMIndexType GetNUnprocessedEntries();

	// check if the stream of data is finalized
	bool IsFinished();

	// set this data stream as finalized
	void SetEndofStream();

	// save data stream
	void Save(SSFStorage &storage);

	// load data stream
	void Load(const FileNode &node);

	// release one link to the idx data entry
	void ReleaseLink(SSFData *data);

	// set synchronization tag in the most recent entry of the stream
	void SetSynchonizationTag(SMIndexType tagID);

public:
	SMDataStream(SMManager *smManager);
};




#endif