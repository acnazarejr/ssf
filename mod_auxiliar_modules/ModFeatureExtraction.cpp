#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "ExtractionControl.h"
#include "ModFeatureExtraction.h"

ModFeatureExtraction instance("");


/******
 * class FeatureExtractionMod
 ******/
ModFeatureExtraction::ModFeatureExtraction(string modID) :  SSFUserModule(modID) {

	/* set input and output data */
	modParams.Require(modID, SSF_DATA_IMAGE, &inModFrames);			// input frames
	modParams.Require(modID, SSF_DATA_TRACKLET, &inModTracklets);	// input tracklets

	/* module information */
	info.Description("Module to extract feature from samples or images and store them in a file.");

	/* Add parameters */
	inputType = SSF_DATA_SAMPLE;
	inputParams.Add("inputType", &inputType, "Data type to extract feature (FRAME, SAMPLE, TRACKLET)", true);
	inputParams.Add("featFile", &featFile, "File containing the feature setup for detection", true);
	inputParams.Add("featSetup", &featSetup, "Feature setup that will be considered (must be defined in file featFile)", true);

	/* register the module */
	this->Register(this, modID);
}


SSFMethod *ModFeatureExtraction::Instantiate(string modID) {

	return new ModFeatureExtraction(modID); 
}


void ModFeatureExtraction::Setup() {

	if (inputType != SSF_DATA_SAMPLE && inputType != SSF_DATA_FRAME && inputType != inModTracklets)
		ReportError("Must choose input type among FRAME, SAMPL and TRACKLET (parameter inputType)");

	if (inputType == SSF_DATA_SAMPLE && inModFrames == "")
		ReportError("Input module not set for data type '%s' has not been set (variable: inModSamples)!", SSF_DATA_SAMPLE);

	if (inputType == SSF_DATA_FRAME && inModFrames == "")
		ReportError("Input module not set for data type '%s' has not been set (variable: inModFrames)!", SSF_DATA_FRAME);

	if (inputType == SSF_DATA_TRACKLET && inModTracklets == "")
		ReportError("Input module not set for data type '%s' has not been set (variable: inModTracklets)!", SSF_DATA_TRACKLET);

	if (featFile == "")
		ReportError("Must set feature file (parameter featFile)");

	if (featSetup == "")
		ReportError("Feature setup to be used must be set (parameter featSetup)");
}



void ModFeatureExtraction::ExtractFeatFrames() {
SMIndexType position = 0;
SMIndexType frameID;

	while ((frameID = glb_sharedMem.RetrieveFrameIDbyPosition(inModFrames, SSF_DATA_IMAGE, position++)) != SM_INVALID_ELEMENT) {
		glb_sharedMem.ExtractFrameFeatures(frameID, featSetupIdx);
	}
}




void ModFeatureExtraction::ExtractFeatSamples() {
SMIndexType position = 0;
SMIndexType frameID;
size_t i;
SSFSamples *ssfSamples;
vector<SMIndexType> smpIds;

	while ((frameID = glb_sharedMem.RetrieveFrameIDbyPosition(inModFrames, SSF_DATA_SAMPLES, position++)) != SM_INVALID_ELEMENT) {
		ssfSamples = glb_sharedMem.GetFrameSamples(inModFrames, frameID);

		// retrive all samples for the current frame
		smpIds = ssfSamples->GetSamples();
		
		// extract feature for each sample in the current frame
		for (i = 0; i < smpIds.size(); i++) {
			glb_sharedMem.ExtractSampleFeatures(smpIds[i], featSetupIdx);
		}

		glb_sharedMem.UnlinkDataItem(ssfSamples);
	}
}




void ModFeatureExtraction::ExtractFeatTracklets() {
SMIndexType position = 0;
SSFTracklet *ssfTracklet;

	while ((ssfTracklet = glb_sharedMem.RetrieveTracklet(inModTracklets, position++)) != NULL) {
		ReportWarning("LAP::ExtractTrackletFeatures() still not implemented, no feature extraction then!");

		glb_sharedMem.UnlinkDataItem(ssfTracklet);
	}
}




void ModFeatureExtraction::Execute() {
FeatureIndex featIdx;
size_t featSetupID;
	
	// setup features
	featIdx = glb_featExtControl.Setup(featFile);
	featSetupID = featIdx.GetID(featSetup);
	featSetupIdx = glb_featExtControl.RetrieveSetupIdx(featSetupID);

	if (inputType == SSF_DATA_FRAME) 
		ExtractFeatFrames();

	else if (inputType == SSF_DATA_SAMPLE) 
		ExtractFeatSamples();

	else if (inputType == SSF_DATA_TRACKLET) 
		ExtractFeatTracklets();

	else 
		ReportError("Invalid data type '%s'", inputType.c_str());
}