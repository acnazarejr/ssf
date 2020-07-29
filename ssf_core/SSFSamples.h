#ifndef _SSF_SAMPLES_H_
#define _SSF_SAMPLES_H_
#include "SSFAsync.h" 
#include "SSFLAPData.h"
#include "SSFSample.h" 



// class to store sample information
class SSFSamples : public SSFLAPData, protected SSFAsync {
friend class LAP;
SSFMutex mutex;

	// save this sample
	void Save(SSFStorage &storage) { ; }

	// load a sample
	void Load(const FileNode &node) { ; }

public:
	SSFSamples();
	~SSFSamples();
	
	// set all samples at once (already set that the write is finished)
	void SetSamples(vector<SMIndexType> &samplesID);

	// retrieve a vector with all samples (wait untill all have been written)
	vector<SMIndexType> GetSamples();

	// return sample ID at position (return SM_INVALID_ELEMENT if this position won't exist, lock processing if this position is not yet set)
	SMIndexType GetSampleEntry(SMIndexType position);
};

#endif