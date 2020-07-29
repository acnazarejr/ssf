#include "headers.h"
#include "SSFCore.h"
#include "FES.h"
#include "ExtractionControl.h"
#include "SSFString.h" 
//#include "SSFFrame.h"
#include "shared_memory.h"
#include "LAP.h"



/******
 * class LAP
 ******/
LAP::LAP() : SharedMemory() {

}








SSFFrame *LAP::RetrieveLAPFrame(SMIndexType frameID) {

	return (SSFFrame *) this->GetData(frameID);
}





void LAP::SetFrameFeatures(SMIndexType frameID, SSFFeatures *feats) {
SSFFrame *frame;

	if (feats == NULL)
		ReportWarning("Empty features");

	// retrieve frame
	frame = RetrieveLAPFrame(frameID);

	// set features
	frame->SetFeatures(feats);

	// unlink reference to the frame
	UnlinkDataItem(frame);
}



SMIndexType LAP::CreateFeed(string dataType) {
SSFFeed *ssfFeed;
SMIndexType id;

	mutexFeeds.Lock();
	ssfFeed = new SSFFeed();
	ssfFeed->modID = glb_SSFExecCtrl.GetInstanceIDCurrentThread();
	id = SetData(ssfFeed, "");

	feedMap.insert(pair<string, SMIndexType>(ssfFeed->modID, id));
	mutexFeeds.Unlock();

	feedsCond.UnlockCondition(ssfFeed->modID);

	return id;
}





SMIndexType LAP::GetFeedIDbyName(string feedName) {

	// check if the feed has already been created
	feedsCond.CheckCondition(feedName);

	return feedMap[feedName];
}





bool LAP::IsProcessingFinished(string modID, string dataType) {

	return IsProcessingDone(modID, dataType);
}



SMIndexType LAP::RetrieveLastProcessedFrameID(string modFrom, string dataType, bool lock) {
SSFLAPData *data;
size_t idx;

	ReportWarning("Function LAP::RetrieveLastProcessedFrameID() is deprecated and will be removed in version %s. Use LAP::RetrieveFrameIDbyPosition() instead", SSF_NEXT_VERSION);

	data = (SSFLAPData *) GetData(modFrom, dataType);

	// end of the data
	if (data == NULL) {
		if (IsProcessingDone(modFrom, dataType) == false)
			ReportError("Invalid return condition (it should be EOS)");
		return SM_INVALID_ELEMENT;
	}

	idx = data->GetFrameID();

	UnlinkDataItem(data);

	return idx;
}



SMIndexType LAP::RetrieveFrameIDbyPosition(string modFrom, string dataType, SMIndexType position) {
SSFLAPData *data;
size_t idx;

	data = (SSFLAPData *) GetData(modFrom, dataType, position);

	// end of the data
	if (data == NULL) {
		if (IsProcessingDone(modFrom, dataType) == false)
			ReportError("Invalid return condition (it should be EOS)");
		return SM_INVALID_ELEMENT;
	}

	idx = data->GetFrameID();

	UnlinkDataItem(data);

	return idx;
}



SMIndexType LAP::CreateFrame(SMIndexType feedID) {
SSFFeed *ssfFeed;
SSFFrame *frame;
SMIndexType id;

	mutexFrames.Lock();
	frame = new SSFFrame(feedID);
	id = SetData(frame, "");
	mutexFrames.Unlock();

	// set frame to the feed
	ssfFeed = (SSFFeed *) GetData(feedID);
	ssfFeed->SetFrame(id);

	UnlinkDataItem(ssfFeed);

	return id;
}



void LAP::SetFrameImage(SMIndexType frameID, SSFImage *image) {
SSFFrame *frame;
size_t idx;

	// set frame ID
	image->SetFrameID(frameID);

	// set the actual data to the shared memory
	idx = SetData(image, "");

	// set the data index to the frame
	frame = RetrieveLAPFrame(frameID);
	frame->SetImage(idx);

	// unlink reference to the frame
	UnlinkDataItem(frame);
}



SSFImage *LAP::GetFrameImage(SMIndexType frameID) {
SSFFrame *frame;
size_t idx;
string modID;

	frame = RetrieveLAPFrame(frameID);

	idx = frame->GetImage(modID);

	// unlink reference to the frame
	UnlinkDataItem(frame);

	return (SSFImage *) GetData(idx);   //modID, SSF_DATA_IMAGE, idx);
}



SSFImage *LAP::GetFramePatch(SMIndexType frameID, SSFRect window) {
SSFImage *img;

	img = GetFrameImage(frameID);
	return img->RetrievePatch(window);
}



SSFFeatures *LAP::GetFrameFeatures(SMIndexType frameID) {
SSFFrame *frame;
SSFFeatures *ssfFeats;

	frame = RetrieveLAPFrame(frameID);

	ssfFeats = frame->GetFeatures();

	// unlink reference to the frame
	UnlinkDataItem(frame);

	return ssfFeats;
}


void LAP::ExtractFrameFeatures(SMIndexType frameID, int featSetupIdx) {
SSFRect window;
SSFImage *img;
SSFFeatures *feat;
//Mat imgData;

	// retrieve frame
	img = GetFrameImage(frameID);

	// retrieve features of that frame
	feat = GetFrameFeatures(frameID);

	if (feat == NULL)
		feat = new SSFFeatures();

	// setup the window equals to the image size
	window.SetRect(0, 0, img->GetNCols(), img->GetNRows()); 

	// extract features
	glb_featExtControl.SetImage(featSetupIdx, img);
	glb_featExtControl.ExtractFeatures(featSetupIdx, window, NULL, *feat);

	// store features to the frame
	SetFrameFeatures(frameID, feat);
}


string LAP::GetFrameFilename(SMIndexType frameID) {
SSFFrame *frame;
SSFString *str;
size_t idx;
string modID;
string strvalue;

	frame = RetrieveLAPFrame(frameID);
	idx = frame->GetFilename(modID);

	// unlink reference to the frame
	UnlinkDataItem(frame);

	str = (SSFString *) GetData(idx);  //modID, SSF_DATA_STRING, idx);
	strvalue = str->GetString();
	UnlinkDataItem(str);

	return strvalue;
}

	

void LAP::SetFrameFilename(SMIndexType frameID, string filename) {
SSFFrame *frame;
SSFString *str;
size_t idx;

	// set the actual data to the shared memory
	str = new SSFString();

	// set frame ID
	str->SetFrameID(frameID);

	str->SetString(filename);
	idx = SetData(str, "");

	// set the data index to the frame
	frame = RetrieveLAPFrame(frameID);
	frame->SetFilename(idx);

	// unlink reference to the frame
	UnlinkDataItem(frame);
}



SSFImage *LAP::GetFrameMask(string modFrom, SMIndexType frameID) {
SSFFrame *frame;
size_t idx;

	frame = RetrieveLAPFrame(frameID);

	// read from the actual running module
	idx = frame->GetMask(modFrom);

	// unlink reference to the frame
	UnlinkDataItem(frame);

	return (SSFImage *) GetData(idx); //modFrom, SSF_DATA_IMAGE, idx);

}


void LAP::SetFrameMask(SMIndexType frameID, SSFImage *mask) {
SSFFrame *frame;
size_t idx;

	// set frame ID
	mask->SetFrameID(frameID);

	// set the actual data to the shared memory
	idx = SetData(mask, "");

	// set the data index to the frame
	frame = RetrieveLAPFrame(frameID);
	frame->SetMask(idx);

	// unlink reference to the frame
	UnlinkDataItem(frame);
}





SSFSlidingWindow *LAP::GetSlidingWindows(string modFrom, SMIndexType frameID) {
SSFFrame *frame;
size_t idx;

	frame = RetrieveLAPFrame(frameID);

	idx = frame->GetSlidingWindows(modFrom);

	// unlink reference to the frame
	UnlinkDataItem(frame);

	return (SSFSlidingWindow *) GetData(idx); //modFrom, SSF_DATA_SLDWINDOWS, idx);
}



void LAP::SetSlidingWindows(SMIndexType frameID, SSFSlidingWindow *sldwins, string attribute) {
SSFFrame *frame;
size_t idx;

	// set frame ID
	sldwins->SetFrameID(frameID);

	// set the actual data to the shared memory
	idx = SetData(sldwins, attribute);

	// set the data index to the frame
	frame = RetrieveLAPFrame(frameID);
	frame->SetSlidingWindows(idx);

	// unlink reference to the frame
	UnlinkDataItem(frame);
}




SSFSample *LAP::GetSample(SMIndexType sampleID) {

	return (SSFSample *) this->GetData(sampleID);
}




SMIndexType LAP::AddFrameSample(SMIndexType frameID, SSFSample *sample) {
SSFSamples *ssfSamples;
SSFFrame *frame;
SMIndexType id, idx;	
string modFrom;

	// set frame ID in the sample
	sample->SetFrameID(frameID);

	/* add sample in the SM */
	id = SetData(sample, "");

	/* add sample in the SSFSamples and it in the SM */
	// retrieve frame
	frame = RetrieveLAPFrame(frameID);

	// retrieve samples
	modFrom = glb_SSFExecCtrl.GetInstanceIDCurrentThread();
	idx = frame->GetSamples(modFrom, false);

	// if invalid, create structure that will be stored
	if (idx == SM_INVALID_ELEMENT) {
		ssfSamples = new SSFSamples();
		ssfSamples->SetFrameID(frameID);
		idx = SetData(ssfSamples, "");
		frame->SetSamples(idx);
		ssfSamples->AddDataItem(id);
	}
	else {
		ssfSamples = (SSFSamples *) GetData(idx);
		ssfSamples->AddDataItem(id);
		UnlinkDataItem(ssfSamples);
	}

	// unlink reference to the frame
	UnlinkDataItem(frame);

	return id;
}


void LAP::EndFrameSamples(SMIndexType frameID) {
SSFSamples *ssfSamples;
SSFFrame *frame;
SMIndexType idx;	
string modFrom;

	// retrieve frame
	frame = RetrieveLAPFrame(frameID);

	// retrieve samples
	modFrom = glb_SSFExecCtrl.GetInstanceIDCurrentThread();
	idx = frame->GetSamples(modFrom, false);

	// if invalid, create structure and set end writing
	if (idx == SM_INVALID_ELEMENT) {
		ReportWarning("No samples added");
		ssfSamples = new SSFSamples();
		ssfSamples->SetFrameID(frameID);
		idx = SetData(ssfSamples, "");
		frame->SetSamples(idx);
		ssfSamples->EndWriting();
	}
	else {
		idx = frame->GetSamples(modFrom);
		ssfSamples = (SSFSamples *) GetData(idx);
		ssfSamples->EndWriting();
		UnlinkDataItem(ssfSamples);
	}

	// unlink reference to the frame
	UnlinkDataItem(frame);
}




SSFSamples *LAP::GetFrameSamples(string modFrom, SMIndexType frameID) {
SSFFrame *frame;
size_t idx;

	frame = RetrieveLAPFrame(frameID);

	idx = frame->GetSamples(modFrom);

	// unlink reference to the frame
	UnlinkDataItem(frame);

	return (SSFSamples *) GetData(idx); //modFrom, SSF_DATA_SAMPLES, idx);
}






void LAP::ExtractSampleFeatures(SMIndexType sampleID, int featSetupIdx, int height, int width, int interpolationMethod) {
SSFImage *patch, *ssfimg;
SSFFeatures *feat;
SSFRect window;
SSFSample *sample;

	// retrieve sample
	sample = GetSample(sampleID);

	mutexSamples.Lock();
	// retrieve image and extract patch
	ssfimg = this->GetFrameImage(sample->GetFrameID());
	window = sample->GetWindow();
	patch = (height == 0 && width == 0) ?
				  ssfimg->RetrievePatch(window)
				: ssfimg->RetrieveResizedPatch(window, height, width, interpolationMethod);

	// get features currently extracted for this sample
	feat = sample->RetrieveFeatures();
	if (feat != NULL)
		ReportWarning("Features already have been extracted for sample ID: %d", sample->GetSampleID());
	else
		feat = new SSFFeatures();

	// extract features
	window.SetRect(0, 0, patch->GetNCols(), patch->GetNRows());
	glb_featExtControl.SetImage(featSetupIdx, patch);
	glb_featExtControl.ExtractFeatures(featSetupIdx, window, NULL, *feat);

	// set the features to the tracklet
	sample->SetFeatures(feat);

	// release memory of SSF structures
	delete patch;

	// unlink used data
	UnlinkDataItem(sample);

	mutexSamples.Unlock();
}



SSFTracklet *LAP::RetrieveTracklet(SMIndexType trackletID) {

	return (SSFTracklet *) this->GetData(trackletID);
}


SMIndexType LAP::SetTracklet(SSFTracklet *tracklet, string attribute) {
SMIndexType id;

	mutexTracklet.Lock();
	id = SetData(tracklet, attribute);
	mutexTracklet.Unlock();

	return id;
}


SSFTracklet *LAP::RetrieveTracklet(string modFrom, SMIndexType position) {

	return (SSFTracklet *) GetData(modFrom, SSF_DATA_TRACKLET, position);
}





SMIndexType LAP::CreateObject(vector<SMIndexType> &trackletIDs, string objID, string attribute) {
SSFObject *obj;
SMIndexType id, i;

	mutexObject.Lock();
	obj = new SSFObject();

	// set object ID
	obj->SetObjID(objID);

	for (i = 0; i < (SMIndexType) trackletIDs.size(); i++) {
		obj->AddTrackletID(trackletIDs[i]);
	}
	
	// set object to the shared memory
	id = SetData(obj, attribute);

	// check it the id is empty
	if (objID == "")
		obj->SetObjID(aToString(id, "%15.15llu"));

	mutexObject.Unlock();

	return id;
}




SSFObject *LAP::RetrieveObject(string modFrom, SMIndexType position) {

	return (SSFObject *) GetData(modFrom, SSF_DATA_OBJECT, position);
}



SSFObject *LAP::RetrieveObject(SMIndexType id) {

	return (SSFObject *) this->GetData(id);
}



SMIndexType LAP::MergeObjects(SMIndexType IdxObj1, SMIndexType IdxObj2, string newID) {
SSFObject *obj1, *obj2, *newObj;
vector<SMIndexType> tracksID;
SMIndexType i, objIdx;

	// retrieve objects
	obj1 = RetrieveObject(IdxObj1);
	obj2 = RetrieveObject(IdxObj2);
	
	tracksID = obj1->GetTrackletIDs();
	objIdx = CreateObject(tracksID, newID);

	// retrieve object created
	newObj = RetrieveObject(objIdx);

	mutexObject.Lock();
	// add the remaining tracklet IDs
	tracksID = obj2->GetTrackletIDs();
	for (i = 0; i < (SMIndexType) tracksID.size(); i++)
		newObj->AddTrackletID(tracksID[i]);
	mutexObject.Unlock();

	// remove the links for the objects used */
	UnlinkDataItem(obj1);
	UnlinkDataItem(obj2);
	UnlinkDataItem(newObj);

	return objIdx;
}



vector<SSFFeatures *> LAP::ExtractObjSampleFeatures(SMIndexType objID, int featSetupIdx) {
vector<SMIndexType> db_tracklets, db_samples;
vector<SSFFeatures *> featureRet;
SSFTracklet *db_tracklet;
SSFFeatures *features; //, *feat_Object;
SSFSample *db_sample;
SSFObject *db_obj;
size_t i, j;

	// retrieve the required object
	db_obj = this->RetrieveObject(objID); 

	// Read the tracklets id from the memory
	db_tracklets = db_obj->GetTrackletIDs();

	// go for each tracklet inside the object
	for (i = 0; i < db_tracklets.size(); i++) {

		// allocate feature extraction memory for samples in each object
		//feat_Object = new SSFFeatures();

		// Read tracklets
		db_tracklet = this->RetrieveTracklet(db_tracklets[i]);

		// Read the samples id from the memory
		db_samples = db_tracklet->GetSamplesID();

		// go for each sample inside the tracklet
		for(j = 0; j < db_samples.size(); j++) {

			// Read sample
			db_sample = this->GetSample(db_samples[j]);

			ReportStatus("\t- %s\n", this->GetFrameFilename(db_sample->GetFrameID()).c_str());

			// Extract and cropping features from a given sample
			this->ExtractSampleFeatures(db_sample->GetSampleID(), featSetupIdx);

			// Retrieves the extracted features
			features = db_sample->RetrieveFeatures();

			if (features == NULL)
				ReportError("Problems in the feature extraction for samples");
			
			// set sample feature to the return structure
			featureRet.push_back(features);
		}

		// set all features of the current object to the returning structure
		//featureRet.push_back(feat_Object);
	}

	return featureRet;
}


/*
bool LAP::IsCameraControlEnabled(SMIndexType feedID) {
bool allowCommand;

	mutexCameras.Lock();

	if (feedID >= (SMIndexType) cameraControl.size()) {
		mutexCameras.Unlock();
		ReportError("Feed ID '%d' does not exist", feedID);
	}

	// check if camera allow control
	allowCommand = cameraControl[feedID];

	mutexCameras.Unlock();

	return allowCommand;
}


void LAP::EnableCameraControl(SMIndexType feedID) {

	mutexCameras.Lock();
	
	if (feedID >= (SMIndexType) cameraControl.size()) {
		mutexCameras.Unlock();
		ReportError("Feed ID '%d' does not exist", feedID);
	}

	// allow camera to be controled
	cameraControl[feedID] = true;

	mutexCameras.Unlock();
}



void LAP::SetCameraCommand(SMIndexType feedID, SSFCamera *command) {

	mutexCameras.Lock();

	if (feedID >= (SMIndexType) cameraControl.size()) {
		mutexCameras.Unlock();
		ReportError("Feed ID '%d' does not exist", feedID);
	}

	if (cameraControl[feedID] == false) {
		mutexCameras.Unlock();
		ReportError("Feed ID '%d' does not allow to be controlled", feedID);
	}

	// set command to have unknow source
	command->SetUnknown();
	command->SetAttribute("feed" + aToString(feedID));

	// set data to the shared memory
	SetData(command);

	mutexCameras.Unlock();
}


SSFCamera *LAP::RetrieveCameraCommand(SMIndexType feedID) {
SSFCamera *cmd;

	mutexCameras.Lock();
	
	if (feedID >= (SMIndexType) cameraControl.size()) {
		mutexCameras.Unlock();
		ReportError("Feed ID '%d' does not exist", feedID);
	}

	if (cameraControl[feedID] == false) {
		mutexCameras.Unlock();
		ReportError("Feed ID '%d' does not allow to be controlled", feedID);
	}

	cmd = (SSFCamera *) GetData("*", SSF_DATA_CAMCOMM, "feed" + aToString(feedID,"%d"));

	mutexCameras.Unlock();

	return cmd;
}
*/

/*
void LAP::SetUserData(SSFUserData *data) {

	SetData(data);
}



SSFUserData *LAP::GetUserData(string modFrom, string attribute, SMIndexType pos) {

	return (SSFUserData *) GetData(modFrom, SSF_DATA_USER, pos, attribute);
}
*/

void LAP::SetObjectCandidates(SSFObjectCandidates *objCandidates, string attribute) {

	SetData(objCandidates, attribute);
}

SSFObjectCandidates *LAP::GetObjectCandidates(string modFrom, SMIndexType pos){

	return (SSFObjectCandidates *) GetData(modFrom, SSF_DATA_OBJECTCANDIDATES, pos);
}



size_t LAP::FES_InitializeExtractionMethod(string setupID, string paramFile) {

	return glb_SSFExecCtrl.FES_InitializeExtractionMethod(setupID, paramFile);
}


void LAP::FES_SetRequest(FESRequest *request) {

	glb_SSFExecCtrl.FES_SetRequest(request);
}