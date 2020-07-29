#include "headers.h"
#include "structures.h"
#include "SSFSample.h" 





/******************
 * class SSFSample *
 ******************/
SSFSample::SSFSample() {

	mutex.Lock();
	this->SetDataType(SSF_DATA_SAMPLE);
	this->feats = NULL;
    this->response = -1;
	mutex.Unlock();
}


SSFSample::~SSFSample() {

	mutex.Lock();
	if (feats != NULL)
		delete feats;
	mutex.Unlock();
}



void SSFSample::SetFeatures(SSFFeatures *feats) {

	mutex.Lock();
	if (this->feats != NULL)
		delete this->feats;
	this->feats = feats;
	mutex.Unlock();
}


void SSFSample::SetPatch(SSFImage *patch, SSFRect window) {

	ReportError("Function SSFSample::SetPatch() has been removed in version 0.0.3. Use function SSFSample::SetPatch(SSFRect window) instead!");
}


void SSFSample::SetPatch(SSFRect window) {

	mutex.Lock();
	this->window = window;
	mutex.Unlock();
}


SSFRect SSFSample::GetWindow() { 
SSFRect ret;

	mutex.Lock();
	ret = window;
	mutex.Unlock();

	return ret;
}

void SSFSample::SetResponse(float r) {

    mutex.Lock();
    this->response = r;
    mutex.Unlock();
}

float SSFSample::GetResponse() {
float r;
    
    mutex.Lock();
    r = this->response;
    mutex.Unlock();

	return r;
}


SSFFeatures *SSFSample::RetrieveFeatures() {
SSFFeatures *feat;

	mutex.Lock();
	feat = this->feats;
	mutex.Unlock();

	return feat;
}



SMIndexType SSFSample::GetSampleID() {
SMIndexType id;

	mutex.Lock();
	id = this->GetSMID();
	mutex.Unlock();

	return id;
}