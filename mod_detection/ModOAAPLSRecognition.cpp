#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "ExtractionControl.h"
#include "pls_oaa.h"
#include "ModOAAPLSRecognition.h"


// TODO: remove me
//extern LAP sharedMem;

ModOAAPLSRecognition instance("");

ModOAAPLSRecognition::ModOAAPLSRecognition(string modID) :  SSFUserModule(modID) {

	pls_oaa = NULL;
	nfactors = height = width = -1;
	maxrank = 1;
	executionType = "testing";
	testInputMod = "";

	/* module information */
	info.Description("PLS one-agains-all for recognition.");

	/* Add parameters */
	inputParams.Add("modelFile", &modelFile, "File storing the model to save or to load", true);
	inputParams.Add("executionType", &executionType, "Execute type (learning or testing)", true);
	inputParams.Add("nfactors", &nfactors, "Number of PLS factors (only for learning)", false);
	inputParams.Add("width", &width, "Resize all samples to the same width (default: sample width)", false);
	inputParams.Add("height", &height, "Resize all samples to the same height (default: sample height)", false);
	inputParams.Add("featFile", &featFile, "File containing the feature setup ", false);
	inputParams.Add("featSetup", &featSetup, "Feature setup that will be considered (must be defined in file featFile)", false);
	inputParams.Add("maxrank", &maxrank, "Number of candidates written in the output tracklet", false);

	/* set input and output data */
	modParams.Require(modID, SSF_DATA_OBJECT, &trainInputMod, "");	// tracklets for training samples
	modParams.Require(modID, SSF_DATA_OBJECTCANDIDATES, &testInputMod, ""); // object candidates for test images

	modParams.Provide(modID, SSF_DATA_OBJECTCANDIDATES);
	modParams.Provide(modID, SSF_DATA_OBJECT);

	/* register the module */
	this->Register(this, modID);
}

ModOAAPLSRecognition::~ModOAAPLSRecognition() {

	if (pls_oaa != NULL)
		delete pls_oaa;
}



void ModOAAPLSRecognition::Setup() {

	// check if the input modules have been set correctly
	if (executionType != "learning" && executionType != "testing")
		ReportError("executionType must be either 'learning' or 'testing'");

	else if (executionType == "learning" && trainInputMod == "")
		ReportError("trainInputMod and trainIDInputMod must be set for learning");

	else if (executionType == "testing" && testInputMod == "")
		ReportError("testInputMod must be set for testing");

	if (executionType == "testing" && modelFile == "") 
		ReportError("modelFile has to be set when execution type is 'testing'");


	// check if the parameters are set correctly
	// learning
	if (executionType == "learning") {
		if (featFile == "")
			ReportError("Parameter 'featFile' has to be set");
		if (featSetup == "")
			ReportError("Parameter 'featSetup' has to be set");
		if (nfactors <= 0)
			ReportError("Parameter 'nfactors' has to be set");
	}

	// testing
	else {
		if (featFile == "")
			ReportError("Parameter 'featFile' has to be set");
		if (featSetup == "")
			ReportError("Parameter 'featSetup' has to be set");
	}
}





SSFMethod *ModOAAPLSRecognition::Instantiate(string modID) {

	return new ModOAAPLSRecognition(modID); 
}



void ModOAAPLSRecognition::Learning() {
int i, j;
SMIndexType position;
SSFTracklet *tracklet;
SSFObject *object;
SSFSample *sample;
SSFFeatures features;
vector<SMIndexType> sampleIDs;
unordered_map<SMIndexType, string> tracklet2ObjectID;
unordered_map<SMIndexType, string>::iterator tracklet2ObjectIDIterator;
vector<SMIndexType> trackletIDs;
vector<SMIndexType>::iterator trackletIDsIterator;

	// get labels;
	position = 0;
	while( (object = glb_sharedMem.RetrieveObject(trainInputMod, position++)) != NULL ) {

		trackletIDs = object->GetTrackletIDs();

		for( i = 0; i < trackletIDs.size(); ++i ) {

			// get tracklet
			tracklet = glb_sharedMem.RetrieveTracklet(trackletIDs[i]);

			sampleIDs = tracklet->GetSamplesID();

			// get feature of samples and add to pls_oaa
			for( j = 0; j < sampleIDs.size(); ++j ) {

				// get sample
				sample = glb_sharedMem.GetSample( sampleIDs[j] );

				// check default width/height
				if( width == -1 )
					width = sample->GetWindow().w;
				if( height == -1 )
					height = sample->GetWindow().h;

				// calc features
				glb_sharedMem.ExtractSampleFeatures( sampleIDs[j], featSetupIdx, width, height );

				// add as positive feature
				pls_oaa->AddSamples( sample->RetrieveFeatures()->RetrieveFeatures(true), object->GetObjID() );
			}
		}
	}
	// train PLS oaa
	pls_oaa->Learn();

	// save model
	this->Save();
}




void ModOAAPLSRecognition::Testing() {
int i, j, w, h;
SMIndexType position = 0;
SMIndexType trackletID;
SSFObjectCandidates *objCandidates;
SSFTracklet *tracklet;
SSFSample *sample;
vector<SMIndexType> sampleIDs;
SSFFeatures features;
vector<SSFObjectCandidate> candidates;
SSFObjectCandidate candidate;
SSFMatrix<FeatType> responses;
vector<string> labelVector;
unordered_set<string> labels;
FeatType maxValue;
int maxIdx;
FeatType value;
vector<int> indices;
vector<int> searchIndices;
const vector<SSFObjectCandidate> *objFilter;
vector<SMIndexType> tracklets;

	Load();

	labelVector = pls_oaa->RetrieveClassIDs();
	for_each(
		labelVector.begin(),
		labelVector.end(),
		[&]( string &a ){
			labels.insert(a);
		});

	while( (objCandidates = glb_sharedMem.GetObjectCandidates(testInputMod, position++)) != NULL ) {

		// get tracklet
		trackletID = objCandidates->GetTrackletID();
		tracklet = glb_sharedMem.RetrieveTracklet(trackletID);
		tracklets.clear();
		tracklets.push_back(trackletID);

		// set candidates
		indices.clear();
		objFilter = objCandidates->GetObjectCandidates();
		if( !objFilter->empty() ) {
			for( i = 0; i < objFilter->size(); ++i ) {
				if( labels.find(
						glb_sharedMem.RetrieveObject(
						(*objFilter)[i].objCandidate
						)->GetObjID()
					) != labels.end()
				){
					indices.push_back( i );
				}
			}
		}

		sampleIDs = tracklet->GetSamplesID();

		// test samples and add to candidates
		candidates.clear();
		for( i = 0; i < sampleIDs.size(); ++i ) {

			// get sample
			sample = glb_sharedMem.GetSample( sampleIDs[i] );

			// check default width/height
			w = ( width == -1 )  ? sample->GetWindow().w : width;
			h = ( height == -1 ) ? sample->GetWindow().h : height;

			// calc features
			glb_sharedMem.ExtractSampleFeatures( sampleIDs[i], featSetupIdx, w, h );

			// classify
			pls_oaa->Classify(sample->RetrieveFeatures()->RetrieveFeatures(true), responses);

			// write candidates
			// take indices from responses
			searchIndices.clear();
			if( indices.empty() )
				for( j = 0; j < responses.cols; ++j ) searchIndices.push_back(j);
			else
				searchIndices = indices;
			// search for max. candidates in searchIndices
			while( candidates.size() < maxrank && !searchIndices.empty() ) {
				// search for max. value
				maxValue = -numeric_limits<FeatType>::max();
				for( j = 0; j < searchIndices.size(); ++j ) {
					value = responses.at<FeatType>(0, searchIndices[j]);
					if( value > maxValue ) {
						maxValue = value;
						maxIdx = j;
					}
				}
				// set maxValue
				candidate.objCandidate = glb_sharedMem.CreateObject(tracklets, labelVector[ searchIndices[ maxIdx ] ]);
				candidate.confidence = responses.at<FeatType>(0, searchIndices[ maxIdx ]);
				searchIndices.erase( searchIndices.begin() + maxIdx );
				candidates.push_back(candidate);
			}
		}
		// create candidates
		objCandidates = new SSFObjectCandidates(trackletID, candidates);

		// write to shared mem.
		glb_sharedMem.SetObjectCandidates(objCandidates);
	}
}




void ModOAAPLSRecognition::Save() {
SSFStorage storage;

	storage.open(modelFile, SSF_STORAGE_WRITE);
	if (storage.isOpened() == false)
		ReportError("Invalid file storage (%s)!", modelFile.c_str());

	storage << "OAAPLSRecognition" << "{";

	// save pls model
	pls_oaa->Save(storage);

	storage << "}";
	storage.release();
}



void ModOAAPLSRecognition::Load() {
SSFStorage storage;
FileNode n, node;

	storage.open(modelFile, SSF_STORAGE_READ);
	if (storage.isOpened() == false)
		ReportError("Invalid file storage (%s)!", modelFile.c_str());

	node = storage.root();

	n = node["OAAPLSRecognition"];

	// load pls model
	pls_oaa->Load(n, storage);

	storage.release();
}




void ModOAAPLSRecognition::Execute() {
DataStartClassification startClassification;
FeatureIndex featIdx;
size_t featSetupID;
	
	// setup features
	featIdx = glb_featExtControl.Setup(featFile);
	featSetupID = featIdx.GetID(featSetup);
	featSetupIdx = glb_featExtControl.RetrieveSetupIdx(featSetupID);

	// create PLS classification
	startClassification.ID = "PLS_OAA";
	pls_oaa = new PLS_OAA(startClassification);

	// execute learning
	if (executionType == "learning")
		Learning();
 
	// execute testing
	else if (executionType == "testing")
		Testing();

	// invalid execution type
	else 
		ReportError("Invalid executionType '%s'", executionType.c_str());
}
