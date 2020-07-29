#include "headers.h"
#include "structures.h"
#include "SSFData.h"
#include "SSFAsync.h" 
#include "SSFSamples.h" 




/******************
 * class SSFSample *
 ******************/
SSFSamples::SSFSamples() {

	mutex.Lock();
	this->SetDataType(SSF_DATA_SAMPLES);
	mutex.Unlock();
}


SSFSamples::~SSFSamples() {

}




void SSFSamples::SetSamples(vector<SMIndexType> &samplesID) {
size_t i;

	mutex.Lock();
	for (i = 0; i < samplesID.size(); i++)
		this->AddDataItem(samplesID[i]);
	this->EndWriting();
	mutex.Unlock();
}



vector<SMIndexType> SSFSamples::GetSamples() {
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


SMIndexType SSFSamples::GetSampleEntry(SMIndexType position) {

	return GetDataEntry(position);
}