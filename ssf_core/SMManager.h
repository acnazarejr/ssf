#ifndef _SM_MANAGER_H_
#define _SM_MANAGER_H_


class SMManager : public SSFStructure {
friend class SMDataStream;
friend class SharedMemory;
SSFMutex mutex;
size_t nAllocatedBytes;					// number of currenty allocated bytes in RAM
vector<SSFData *> dataEntries;			// actual data storage (if NULL, the data is in the disk)
vector<int> nlinks;						// number of links to each data entry

// input parameters
int maxMemory;		// maximum number of bytes in the memory
string directory;	// directory to store the memory

	// recover data from disk
	void RecoverData(SMIndexType idx);
	
	// save data to the disk
	void SaveData();

protected:

	// set a new data entry - returns its unique ID
	SMIndexType SetData(SSFData *data);

	// retrieve a data entry with identifier id 
	SSFData *GetData(SMIndexType id);

	// decrease a link for a data item (to control is a item can be write in disk, when it is 0, can be released)
	void UnlinkData(SMIndexType id);

	// decrease links for a set of data entries
	void UnlinkData(unordered_map<SMIndexType, int> &entriesID);

public:
	SMManager();
	~SMManager();


	// retrieve the name of the method 
	string GetName() { return SSF_STRUCTURE_SMManager; } 

	// retrieve the type of the method
	string GetType() { return SSF_METHOD_TYPE_STRUCTURE; } 

	// retrieve structure version
	string GetVersion() { return "0.0.1"; }

	// structure setup - function written by the user and called after parameters have been set
	void Setup(); 
};



#endif