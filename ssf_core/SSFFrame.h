#ifndef _SSF_FRAME_H_
#define _SSF_FRAME_H_
#include "SSFLAPData.h"
#include "SSFRect.h" 
#include "SSFFeatures.h" 





class SSFFrame : public SSFLAPData  {
friend class LAP;
SSFMutex mutex;
CondMutexManager<string> dataCond;			// conditional mutex
unordered_map<string, SMIndexType> mask;			// link to masks for several input modules
unordered_map<string, SMIndexType> samples;		// link to samples for several input modules
unordered_map<string, SMIndexType> sldwindows;	// link to sliding windows
SMIndexType feedID;			// ID of the feed to which this frame belongs
SMIndexType image;			// link to the image
string modImage;		// module that created the image
SSFFeatures *feats;		// features for this frame
SMIndexType filename;		// filename if that is the case
string modFilename;		// module that created the filename



protected:
	// retrieve frameID
	SMIndexType GetFrameID();

	// get image
	SMIndexType GetImage(string &modID);

	// set image
	void SetImage(SMIndexType idx);

	// get features 
	SSFFeatures *GetFeatures();

	// set features
	void SetFeatures(SSFFeatures *feats);

	// get filename
	SMIndexType GetFilename(string &modID);

	// set filename
	void SetFilename(SMIndexType idx);

	// get mask
	SMIndexType GetMask(string module);

	// set mask
	void SetMask(SMIndexType idx);

	// get sliding windows
	SMIndexType GetSlidingWindows(string module);

	// set sliding windows
	void SetSlidingWindows(SMIndexType idx);

	// return ID of samples inside this image
	SMIndexType GetSamples(string module, bool lock = true);

	// set samples ID
	void SetSamples(SMIndexType idx);

	// save this frame to a storage
	void Save(SSFStorage &storage) { ; }

	// load frame from a storage
	void Load(const FileNode &node) { ; }

public:
	~SSFFrame();
	SSFFrame(SMIndexType feedID);
};







#endif