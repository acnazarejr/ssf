#include "headers.h"
#include "SSFCore.h"
#include "FESMemoryManagement.h"
#include "FES.h"
#include "FESRegion.h"

//////
// class FESRegion 
//////
FESRegion::FESRegion() {

	this->features = NULL;
	this->memManager = NULL;
}


FESRegion::~FESRegion() {

	if (this->features != NULL)
		this->memManager->ReleaseEntry(features);
}


const SSFMultipleFeatures *FESRegion::RetrieveFeatures() {

	return features;
}


void FESRegion::SetFeatures(SSFMultipleFeatures *feats, FESMemoryManagement *memManager) {

	if (this->features != NULL) 
		ReportError("FESRegion cannot be used more than once");

	this->memManager = memManager;
	this->features = feats;
}


SSFFeatures *FESRegion::RetrieveSingleExtraction(SMIndexType featID) {

	return features->RetrieveSingleFeatureExtraction(featID);
}


int FESRegion::NumberExtractionMethods() {

	return (int) features->NumberExtractionMethods();
}


void FESRegion::SetExtractedOneMethod() {
	
	features->SetExecutedMethod();
}