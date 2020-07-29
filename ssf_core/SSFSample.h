#ifndef _SSF_SAMPLE_H_
#define _SSF_SAMPLE_H_
#include "SSFLAPData.h"
#include "SSFRect.h"
#include "SSFFeatures.h" 
#include "SSFImage.h"


// class to store sample information
class SSFSample : public SSFLAPData  {
friend class LAP;
friend class SSFSamples;
SSFMutex mutex;
SSFFeatures *feats;						// features for this sample
SSFRect window;							// location of the sample in the frame
float response;                         // confidence score assigned by a detector
// to add: pose and gesture

	// retrieve features extracted for this sample	
	void SetFeatures(SSFFeatures *feats);

	// save this sample
	void Save(SSFStorage &storage) { ; }

	// load a sample
	void Load(const FileNode &node) { ; }

public:
	SSFSample();
	~SSFSample();

	// set patch for this sample in the corresponding location in the image
	void SetPatch(SSFImage *patch, SSFRect window);

	// set patch for this sample in the corresponding location in the image
	void SetPatch(SSFRect window);

	// retrieve detection window
	SSFRect GetWindow();

    // set response value
    void SetResponse(float r);

    // retrieve response value
    float GetResponse();

	// retrieve features extracted for this sample
	SSFFeatures *RetrieveFeatures();

	// get sampleID
	SMIndexType GetSampleID();
};

#endif