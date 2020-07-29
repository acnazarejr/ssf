#include "headers.h"
#include "structures.h"
#include "SSFFeed.h" 



/****************
 * class SMFeed *
 ****************/
SSFFeed::SSFFeed() {

	this->SetDataType(SSF_DATA_FEED);
}





void SSFFeed::Save(SSFStorage &storage) {

}


void SSFFeed::Load(const FileNode &node) {



}

void SSFFeed::SetFrame(SMIndexType frameID) {

	mutex.Lock();
	framesID.push_back(frameID);
	mutex.Unlock();
}


size_t SSFFeed::GetNFrames() {
size_t idx;

	mutex.Lock();
	idx = framesID.size();
	mutex.Unlock();

	return idx;
}


void SSFFeed::SetProcessingMod(string modID, string dataType) {

	mutex.Lock();
	modulesProcessing.insert(pair<string, string>(modID, dataType));
	mutex.Unlock();
}