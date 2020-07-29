#include "headers.h"
#include "structures.h"
#include "SSFAsync.h" 
#include "SSFTracklet.h" 


/********************
 * class SSFTracklet *
 ********************/
SSFTracklet::SSFTracklet() {

	mutex.Lock();
	this->SetDataType(SSF_DATA_TRACKLET);
	this->feats = NULL;
	mutex.Unlock();
}


SSFTracklet::~SSFTracklet() {

	mutex.Lock();
	if (this->feats != NULL)
		delete this->feats;
	mutex.Unlock();
}



void SSFTracklet::SetFeatures(SSFFeatures *feats) { 

	mutex.Lock();
	if (this->feats != NULL)
		delete this->feats;
	this->feats = feats;
	mutex.Unlock();
}






SSFFeatures *SSFTracklet::RetrieveFeatures() {
SSFFeatures *feat;

	mutex.Lock();
	feat = this->feats;
	mutex.Unlock();

	return feat;
}


SMIndexType SSFTracklet::GetTrackletID() {
SMIndexType id;

	mutex.Lock();
	id = this->GetSMID();
	mutex.Unlock();

	return id;
}




void SSFTracklet::SetSamples(vector<SMIndexType> &samplesID) {
size_t i;

	mutex.Lock();
	for (i = 0; i < samplesID.size(); i++)
		this->AddDataItem(samplesID[i]);
	this->EndWriting();
	mutex.Unlock();
}



vector<SMIndexType> SSFTracklet::GetSamplesID() { 
vector<SMIndexType> smp;
SMIndexType id;
SMIndexType position = 0;

	mutex.Lock();
	while (1) {
		id = this->GetDataEntry(position++);
		if (id == SM_INVALID_ELEMENT)
			break;
		smp.push_back(id);
	}
	mutex.Unlock();

	return smp;
}




SMIndexType SSFTracklet::GetSampleEntry(SMIndexType position) {

	return GetDataEntry(position);
}