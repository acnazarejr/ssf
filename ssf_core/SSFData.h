#ifndef _SSF_DATA_H_
#define _SSF_DATA_H_



class SSFData {
friend class SMManager;
friend class SharedMemory;
friend class LAP;
friend class SMDataStream;

protected:
string dataType;		// type of data provided
string modID;			// module that generate this sample
string attribute;		// extra attribute in the data type
bool unknownSrc;		// the module that created the data won't be known
size_t timeStamp;		// time stamp in which this data was created
SMIndexType idSM;		// unique ID for the entry (unique over all shared memory)
SMIndexType syncTag;	// indicates whether this entry contains a synchronization tag

	void SetDataType(string dataType);

	void SetModuleID(string modID);

	void SetAttribute(string attribute);

	void SetTimeStamp(size_t timeStamp);

	void SetSMID(SMIndexType idSM);

	void SetUnknown();
	
	void SetSynchonizationTag(SMIndexType tagID);

	SMIndexType GetSynchonizationTag();

public:

	SSFData();

	~SSFData();

	string GetDataType();

	string GetModID();

	string GetAttribute();

	size_t GetTimeStamp();

	bool IsUnknownSource();

	// if error == true, abort execution if ID is invalid (data entry is not in the shared memory)
	SMIndexType GetSMID(bool error = true);

	// save this feed to a storage
	virtual void Save(SSFStorage &storage) { 
		ReportError("Method Save() has not been implemented"); 
	}

	// load feed from a storage
	virtual void Load(const FileNode &node) { 
		ReportError("Method Save() has not been implemented"); 
	}
};


#endif