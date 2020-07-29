#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "ExtractionControl.h"
#include "pls.h"
#include "QDA.h"
#include "ModPLSDetector.h"


ModPLSDetector instance("");

/******
 * class ModPLSDetector
 ******/
ModPLSDetector::ModPLSDetector(string modID) :  SSFUserModule(modID) {

	pls = NULL;
	qda = NULL;
	nfactors = -1;
	threshold = -10000.0f;

	executionType = "testing";

	/* Add parameters */
	inputParams.Add("modelFile", &modelFile, "File storing the model to save or to load", true);
	inputParams.Add("executionType", &executionType, "Execute type (learning or testing)", true);
	inputParams.Add("featFile", &featFile, "File containing the feature setup for detection (only for learning)", false);
	inputParams.Add("featSetup", &featSetup, "Feature setup that will be considered (must be defined in file featFile", false);
	inputParams.Add("nfactors", &nfactors, "Number of PLS factors (only for learning)", false);
	inputParams.Add("threshold", &threshold, "Threshold for this detector (only for testing)", false);

	/* module information */
	info.Description("Implementation of the PLS Detector.");

	/* set input and output data */
	modParams.Require(modID, SSF_DATA_SLDWINDOWS, &posInputMod, "positive");	// sliding windows for positive samples
	modParams.Require(modID, SSF_DATA_SLDWINDOWS, &negInputMod, "negative");	// sliding windows for negative samples
	modParams.Require(modID, SSF_DATA_SLDWINDOWS, &testInputMod, "test");		// sliding windows for test images
	//modParams.Require(modID, SSF_DATA_USER, &trainModelMod, "ModelPLSDetector");// PLS model generated on the fly
	modParams.Provide(modID, SSF_DATA_SLDWINDOWS);

	/* register the module */
	this->Register(this, modID);
}

ModPLSDetector::~ModPLSDetector() {

	if (pls != NULL)
		delete pls;

	if (qda != NULL)
		delete qda;
}



void ModPLSDetector::Setup() {

	// check if the input modules have been set correctly
	if (executionType != "learning" && executionType != "testing")
		ReportError("executionType must be either 'learning' or 'testing'");

	else if (executionType == "learning" && (posInputMod == "" || negInputMod == ""))
		ReportError("posInputMod and negInputMod must be set for learning");

	else if (executionType == "testing" && testInputMod == "")
		ReportError("testInputMod must be set for testing");

	if (executionType == "testing" && modelFile == "" && trainModelMod == "") 
		ReportError("trainModelMod has to be set to read PLS model build on-the-fly (when modelFile has not been set)");


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





SSFMethod *ModPLSDetector::Instantiate(string modID) {

	return new ModPLSDetector(modID); 
}




SSFFeatures *ModPLSDetector::FeatureExtraction(string inputMod, vector<int> featIdxs) {
SSFSlidingWindow *sldWins;
SSFDetwins *ssfDetwins;
vector<DetWin> *detwins;
FeatureIndex featIdx;
SSFFeatures *ssfFeats;
SSFFeatures ssfFeatSample;
size_t position, i;
size_t frameID;
size_t scaleID;
SSFRect rect;

	ssfFeats = new 	SSFFeatures();

	// extract features for the positive samples	
	position = 0;
	while ((frameID = glb_sharedMem.RetrieveFrameIDbyPosition(inputMod, SSF_DATA_SLDWINDOWS, position++)) != SM_INVALID_ELEMENT) {

		// retrieve sliding windows for frameID
		sldWins = glb_sharedMem.GetSlidingWindows(inputMod, frameID);

		// load images for the input sliding windows
		sldWins->LoadImages(glb_sharedMem.GetFrameImage(frameID));

		// run for each scale
		for (scaleID = 0; scaleID < sldWins->GetNEntries(); scaleID++) {

			// set image for this scale
			featExt.SetImage(featIdxs, sldWins->GetSSFImage(scaleID));

			// retrieve sliding windows of this scale
			ssfDetwins = sldWins->GetSSFDetwins(scaleID);

			// extract features for each detection window
			detwins = ssfDetwins->GetDetwins();

			for (i = 0; i < detwins->size(); i++) {
				ssfFeatSample.Reset();
				rect = detwins->at(i).GetSSFRect();
				featExt.ExtractFeatures(featIdxs, rect, ssfFeatSample);  
				ssfFeats->AddFeatures(ssfFeatSample.RetrieveFeatures(true));
			}
		}

		// release the images for the input sliding windws
		sldWins->ReleaseImages();

		ReportStatus("Extracting features from images provided by module '%s' [frame: %d]", inputMod.c_str(), position + 1);
	}

	return ssfFeats;
}




void ModPLSDetector::Learning() {
FeatureIndex featIdx;
SSFMatrix<FeatType> dataY;
SSFFeatures ssfFeats, *ssfFeatPos, *ssfFeatNeg;
SSFMatrix<FeatType> highPos, highNeg;
SSFFeatures ssfFeatSample;
SSFMatrix<FeatType> lowPos, lowNeg;
vector<int> featIdxs;
vector<size_t> allIds;
vector<FeatBlockDef *> blocks;
DataStartClassification startClassification;
size_t i;

	// initialize Y matrix (this matrix will have Nx1, when N is the number of feature samples (positive + negative ones)
	dataY.create(0, 1);

	// setup feature extraction
	featIdx = featExt.Setup(featFile);

	// obtain the feature index that will be used to perform the feature extraction
	allIds = featIdx.GetAllIDs();
	for (i = 0; i < allIds.size(); i++) {
		featIdxs.push_back(featExt.RetrieveSetupIdx(allIds[i]));
		blocks.push_back(featExt.RetrieveBlocks(featIdxs[i]));
	}

	// extract features for positive samples
	ssfFeatPos = this->FeatureExtraction(posInputMod, featIdxs);
	highPos = ssfFeatPos->RetrieveFeatures(false);

	for (i = 0; i < highPos.rows; i++)
		dataY.push_back(1.0f);

	// extract features for negative samples
	ssfFeatNeg = this->FeatureExtraction(negInputMod, featIdxs);
	highNeg = ssfFeatNeg->RetrieveFeatures(false);

	for (i = 0; i < highNeg.rows; i++)
		dataY.push_back(-1.0f);

	// concatenate negative and positive features
	ssfFeats.AddFeatures(highPos);
	ssfFeats.AddFeatures(highNeg);

	// execute PLS
	pls = new PLS();
	pls->runpls(ssfFeats.RetrieveFeatures(false), dataY, nfactors);

	// perform dimension reduction
	pls->Projection(highPos, lowPos, nfactors);
	pls->Projection(highNeg, lowNeg, nfactors);

	// clean data
	delete ssfFeatNeg;
	delete ssfFeatPos;

	// execute QDA
	startClassification.ID = "";
	qda = new QDA(startClassification);
	qda->Initialize();
	qda->AddSamples(lowPos, "1");
	qda->AddSamples(lowNeg, "-1");
	qda->Learn();

	// save model
	this->Save();
}




void ModPLSDetector::Testing() {
SSFSlidingWindow *sldWins, *outputSldWins;
SSFDetwins *ssfDetwins, *outputDetwins;
vector<DetWin> *detwins;
FeatureIndex featIdx;
SSFRect rect;
SSFFeatures ssfFeatSample;
DetWin detwin;
SSFMatrix<FeatType> lowD, responses, allResponses;
//int featSetupIdx;
size_t i;
size_t frameID;
size_t scaleID;
int responsePosID;		// index of the positive response
vector<size_t> allIds;
vector<int> featIdxs;
size_t position = 0;

	// setup feature extraction
	featIdx = featExt.Setup(featFile);

	// obtain the feature index that will be used to perform the feature extraction
	//featSetupIdx = featExt.RetrieveSetupIdx(featIdx[featSetup]);
	allIds = featIdx.GetAllIDs();
	for (i = 0; i < allIds.size(); i++)
		featIdxs.push_back(featExt.RetrieveSetupIdx(allIds[i]));

	// load saved detection model
	this->Load();

	// set index of the positive class
	if (qda->RetrieveClassIDs().at(0) == "-1")
		responsePosID = 1;
	else
		responsePosID = 0;



	// extract features for the positive samples	
	while ((frameID = glb_sharedMem.RetrieveFrameIDbyPosition(testInputMod, SSF_DATA_SLDWINDOWS, position++)) != SM_INVALID_ELEMENT) {

		// set sliding windows
		outputSldWins = new SSFSlidingWindow();

		// retrieve sliding windows for frameID
		sldWins = glb_sharedMem.GetSlidingWindows(testInputMod, frameID);

		// load images for the input sliding windows
		sldWins->LoadImages(glb_sharedMem.GetFrameImage(frameID));

		ReportStatus("[ID frame: %d] Detecting for %d scales and %d detection windows)", frameID, sldWins->GetNEntries(), sldWins->GetNumberofWindows());

		// run for each scale
		for (scaleID = 0; scaleID < sldWins->GetNEntries(); scaleID++) {

			// set image for this scale
			featExt.SetImage(featIdxs, sldWins->GetSSFImage(scaleID));

			// retrieve sliding windows of this scale
			ssfDetwins = sldWins->GetSSFDetwins(scaleID);

			// extract features for each detection window
			detwins = ssfDetwins->GetDetwins();

			// create output SSFDetwins
			outputDetwins = new SSFDetwins();

			for (i = 0; i < detwins->size(); i++) {
				ssfFeatSample.Reset();
				rect = detwins->at(i).GetSSFRect();
				
				featExt.ExtractFeatures(featIdxs, rect, ssfFeatSample);  

				// project to the low dimension
				pls->Projection(ssfFeatSample.RetrieveFeatures(true), lowD, pls->GetNFactors());

				// classify sample
				qda->Classify(lowD, responses);

				// select if the sample will be passed to the next stage
				if (responses(0, responsePosID) >= threshold) {
					detwin = detwins->at(i);
					detwin.response = responses(0, responsePosID);
					outputDetwins->AddDetWindow(detwin);
				}
			}

			// only create this guy if outputDetwins->Size() > 0, otherwise do not propagate to the "next stage"
			if (outputDetwins->GetNumberofDetWins() > 0) 
				//ReportStatus("WARNING: clone scaleInfo");
				outputSldWins->AddDetwins(outputDetwins, sldWins->GetScaleInfo(scaleID));
		}

		// release the images for the input sliding windws
		sldWins->ReleaseImages();

		// set output sliding windows
		glb_sharedMem.SetSlidingWindows(frameID, outputSldWins);
	}
}




void ModPLSDetector::Save() {
SSFStorage storage;

	storage.open(modelFile, SSF_STORAGE_WRITE);
	if (storage.isOpened() == false)
		ReportError("Invalid file storage (%s)!", modelFile.c_str());

	storage << "PLSDetector" << "{";

	// save pls model
	pls->Save(storage);

	// save qda classifier
	qda->Save(storage);

	// save feature setup

	storage << "}";
	storage.release();
}



void ModPLSDetector::Load() {
DataStartClassification startClassification;
SSFStorage storage;
FileNode n, node;

	storage.open(modelFile, SSF_STORAGE_READ);
	if (storage.isOpened() == false)
		ReportError("Invalid file storage (%s)!", modelFile.c_str());

	node = storage.root();

	n = node["PLSDetector"];

	// load pls model
	pls = new PLS();
	pls->Load(n);

	// load qda classifier
	startClassification.ID = "";
	qda = new QDA(startClassification);
	qda->Load(n, storage);

	// load feature setup

	storage.release();
}



void ModPLSDetector::Execute() {

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