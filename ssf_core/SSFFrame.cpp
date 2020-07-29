#include "headers.h"
#include "SSFCore.h"
#include "structures.h"
#include "SSFFrame.h" 




/*****************
 * class SMFrame *
 *****************/

SSFFrame::SSFFrame(SMIndexType feedID) {

	mutex.Lock();
	this->feedID = feedID;
	this->SetDataType(SSF_DATA_FRAME);

	// start
	this->image = SM_INVALID_ELEMENT;
	this->modImage = "";
	this->feats = NULL;
	this->filename = SM_INVALID_ELEMENT;
	this->modFilename = "";
	mutex.Unlock();
}



SSFFrame::~SSFFrame() {

}

SMIndexType SSFFrame::GetFrameID() {
SMIndexType id;

	mutex.Lock();
	id = this->GetSMID();
	mutex.Unlock();

	return id;
}

SMIndexType SSFFrame::GetImage(string &modID) {
SMIndexType id;

	// check if image has been set
	if (image == SM_INVALID_ELEMENT)
		dataCond.CheckCondition("image");

	mutex.Lock();
	id = this->image;
	modID = this->modImage;
	mutex.Unlock();

	return id;
}


void SSFFrame::SetImage(SMIndexType idx) {

	mutex.Lock();
	this->image = idx;
	this->modImage = glb_SSFExecCtrl.GetInstanceIDCurrentThread();
	mutex.Unlock();

	// image has been set
	dataCond.UnlockCondition("image");
}


SSFFeatures *SSFFrame::GetFeatures() {
SSFFeatures *feat;

	mutex.Lock();
	feat = this->feats;
	mutex.Unlock();	

	return feat;
}


void SSFFrame::SetFeatures(SSFFeatures *feats) {

	mutex.Lock();
	this->feats = feats;
	mutex.Unlock();	
}


SMIndexType SSFFrame::GetFilename(string &modID) {
SMIndexType id;

	// check if filename has been set
	if (filename == SM_INVALID_ELEMENT) 
		dataCond.CheckCondition("filename");

	mutex.Lock();
	id = this->filename;
	modID = this->modFilename;
	mutex.Unlock();

	return id;
}


void SSFFrame::SetFilename(SMIndexType idx) {

	mutex.Lock();
	this->filename = idx;
	this->modFilename = glb_SSFExecCtrl.GetInstanceIDCurrentThread();
	mutex.Unlock();	

	// filename has been set
	dataCond.UnlockCondition("filename");
}



SMIndexType SSFFrame::GetMask(string module) {
unordered_map<string, SMIndexType>::iterator it;
SMIndexType id;

	mutex.Lock();
	// check if samples have been set
	it = mask.find(module);
	if (it == mask.end()) {
		mutex.Unlock();
		dataCond.CheckCondition("mask" + module);
		mutex.Lock();
	}

	id = mask[module];
	mutex.Unlock();	

	return id;
}


void SSFFrame::SetMask(SMIndexType idx) {
string module;

	module = glb_SSFExecCtrl.GetInstanceIDCurrentThread();

	mutex.Lock();
	this->mask.insert(pair<string, SMIndexType>(module, idx));
	mutex.Unlock();

	// mask for module has been set
	dataCond.UnlockCondition("mask" + module);
}




SMIndexType SSFFrame::GetSlidingWindows(string module) {
unordered_map<string, SMIndexType>::iterator it;
SMIndexType id;

	mutex.Lock();
	// check if the sliding windows have been set
	it = sldwindows.find(module);
	if (it == sldwindows.end()) {
		mutex.Unlock();
		dataCond.CheckCondition("sldwindows" + module);
		mutex.Lock();
	}

	id = sldwindows[module];
	mutex.Unlock();	

	return id;
}


void SSFFrame::SetSlidingWindows(SMIndexType idx) {
string module;

	module = glb_SSFExecCtrl.GetInstanceIDCurrentThread();
	mutex.Lock();
	sldwindows.insert(pair<string, SMIndexType>(module, idx));
	mutex.Unlock();

	// samples have been set by module
	dataCond.UnlockCondition("sldwindows" + module);

}


SMIndexType SSFFrame::GetSamples(string module, bool lock) {
unordered_map<string, SMIndexType>::iterator it;
SMIndexType id;

	mutex.Lock();
	// check if samples have been set
	it = samples.find(module);
	if (it == samples.end() && lock == true) {
		mutex.Unlock();
		dataCond.CheckCondition("samples" + module);
		mutex.Lock();
	}
	else if (it == samples.end() && lock == false) {
		id = SM_INVALID_ELEMENT;
		mutex.Unlock();
		return id;
	}

	id = samples[module];
	mutex.Unlock();	

	return id;
}



void SSFFrame::SetSamples(SMIndexType idx) {
string module;

	module = glb_SSFExecCtrl.GetInstanceIDCurrentThread();
	mutex.Lock();
	samples.insert(pair<string, SMIndexType>(module, idx));
	mutex.Unlock();

	// samples have been set by module
	dataCond.UnlockCondition("samples" + module);
}