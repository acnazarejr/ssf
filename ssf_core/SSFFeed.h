#ifndef _SSF_FEED_H_
#define _SSF_FEED_H_
#include "SSFLAPData.h"


// class to store feed information
class SSFFeed : public SSFLAPData  {
SSFMutex mutex;
vector<SMIndexType> framesID;
unordered_multimap<string,string> modulesProcessing;		// modules that process this feed

protected:

	// save this feed to a storage
	void Save(SSFStorage &storage);

	// load feed from a storage
	void Load(const FileNode &node);

public:
	SSFFeed();

	// set a new frame to the last position of the feed 
	void SetFrame(SMIndexType frameID);

	// retrieve the number of frames in the feed
	size_t GetNFrames();

	// set module that process this feed
	void SetProcessingMod(string modID, string dataType);
};





#endif