#ifndef _SSF_TRACKLET_H_
#define _SSF_TRACKLET_H_
#include "SSFLAPData.h" 
#include "SSFAsync.h" 
#include "SSFRect.h"
#include "SSFFeatures.h" 


// class to store tracklet information
class SSFTracklet : public SSFLAPData, protected SSFAsync {
friend class LAP;
SSFMutex mutex;
//vector<SMIndexType> samplesID;	// list of samples from this tracklet
SSFFeatures *feats;			// features for this tracklet
// to add: actions

	// set features to this tracklet (used for temporal feature extraction)
	void SetFeatures(SSFFeatures *feats);

public:
	SSFTracklet();
	~SSFTracklet();

	// set all samples to the tracklet at once (already set that the write is finished)
	void SetSamples(vector<SMIndexType> &samplesID);

	// retrieve features
	SSFFeatures *RetrieveFeatures();

	// retrieve ids of samples in this tracklet (wait untill all have been written)
	vector<SMIndexType> GetSamplesID();

	// retrieve tracklet ID
	SMIndexType GetTrackletID();

	// return sample ID at position (return SM_INVALID_ELEMENT if this position won't exist, lock processing if this position is not yet set)
	SMIndexType GetSampleEntry(SMIndexType position);
};



#endif