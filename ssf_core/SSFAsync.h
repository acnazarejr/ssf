#ifndef _SSF_ASYNC_DATA_H_
#define _SSF_ASYNC_DATA_H_
#include "SSFThreads.h" 
#include "SSFStorage.h"
#include "SSFData.h"


// Detwin structure
class SSFAsync {
CondMutexManager<SMIndexType> dataCond;	// conditional mutex to indicate the positions that have been written
SSFMutex mutex;
vector<SMIndexType> SMIDs;				// IDs of data in this list of data items
bool writeAllowed;						// indicates whether this structure has been completely written or the module can still write data
string itemDataType;					// type of data items (all entries must be of the same type)

protected:

	// retrieve the ID in the SM of a data item according to its position (return SM_INVALID_ELEMENT when no more data will be written here)
	SMIndexType GetDataEntry(SMIndexType position);

public:

	SSFAsync();

	// add a new data item - its ID in the shared memory
	void AddDataItem(SMIndexType smID);

	// writing in this structure finished, no more data can be written after this call
	void EndWriting();
};


#endif