#ifndef _SHARED_MEMORY_H_
#define _SHARED_MEMORY_H_
#include "SSFCore.h"
#include "SSFData.h" 
#include "SMDataStream.h"



class SharedMemory : public SSFStructure {
friend class SSFExecutionControl;
friend class SSFUserModule;
friend class LAP;
SSFMutex mutex;
SSFMutex mutexRetDataStream;
SMManager *smManager;								// shared memory manager
unordered_map<string, SMDataStream *> dataSMMap;	// map to the data

// parameter variables
unordered_map<string, SMIndexType> creationConditions;	// modules that will generate data only if a max number of unprocessed data is available
CondMutexManager<string> userDataCond;					// conditional mutex
string smManagerSetup;									// setupID to configure smManager

	// function to generate a key from modID, dataType and attribute
	string GenerateKey(const string modID, const string dataType, const string attribute);

	// retrieve data key parts modID::dataType(attribute)
	void RetrieveDataKeys(string key, string &modID, string &dataType, string &attribute);

	// check if creation condition for 'module' has been satisfied
	bool CheckCreationCondition(string module, string dataType, string attribute = "");

	// update creation condition
	void UpdateCreationCondition(string module, string dataType, string attribute = "");

	// create data stream
	void CreateDataStream(string module, string dataType, string attribute = "");

	// auxiliar function to split modID into modID and attribute (format of modID ModIDDataType*:*Attr)
	void SplitModIDAttribute(string &modID, string &attribute);

	// Return the name of the module
	string GetName() { return SSF_STRUCTURE_SM; }

	// retrieve the type of the method
	string GetType() { return SSF_METHOD_TYPE_STRUCTURE; }

	// version
	string GetVersion() { return "0.0.1"; }

protected:

	// retrieve data stream 
	SMDataStream *RetrieveDataStream(string module, string dataType, string attribute);

	// retrieve a data element by its ID
	SSFData *GetData(SMIndexType id);

	// the processing of a module is about to start - set the provided data to the shared memory
	void StartProcessingModule(SSFModParameters &modParams, string modID);

	// the processing of a module is finished (all module information will be updated)
	void FinishProcessingModule(SSFModParameters &modParams, unordered_map<SMIndexType, int> &entriesID);

	// retrieve data getting the first unprocessed entry (return NULL if is the end of data)
	SSFData *GetData(string fromModID, string dataType);

public:
	SharedMemory();

	// function to check if input modules have been set correctly
	void Setup();

	/* user data */
	// retrieve data according to the index (return NULL if idx is larger than the number of entries and EOS is set)
	SSFData *GetData(string fromModID, string dataType, SMIndexType idx);

	// set data
	SMIndexType SetData(SSFData *data, string attribute);

	// check if a combination module + data type will no longer provide data
	bool IsProcessingDone(string modID, string dataType);

	// retrieve the number of entries in a processing module
	size_t GetNumberEntries(string modID, string dataType);

	// wait until module modID finishes
	void WaitUntilModuleEnd(string modID);

	// write the current state of the shared memory to a file
	void DumpSharedMemory(SSFStorage &storage);

	// unlink one entry of a data item
	void UnlinkDataItem(SSFData *data);

	// wait for module modID to receive the synchronization tag tagID
	void WaitModToProcess(string modID, SMIndexType tagID);

	// send synchronization tag to mark the last data with type dataType having the attribute - returns an ID
	SMIndexType SendSynchronizationTag(string dataType, string attribute);
};







#endif