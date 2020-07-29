#include "headers.h"
#include "SSFCore.h"
#include "FES.h"
#include "FESRequest.h"


////
// class FESRequest
////
FESRequest::FESRequest() {

	noMoreInputData = false;
	data = NULL;
	NumbertoProcess = 0;
}


FESRegion *FESRequest::GetRegionToProcess() {

	return inQueue.pop();
}


void FESRequest::SetRegionToProcess(FESRegion *region) {

	mutex.Lock();
	inQueue.push(region);
	NumbertoProcess++;
	mutex.Unlock();
}


FESRegion *FESRequest::GetProcessedRegion() {

	return outQueue.pop();
}



void FESRequest::SetProcessedRegion(FESRegion *region) {

	mutex.Lock();
	// check whether this region has all features extracted
	if (region->NumberExtractionMethods() != featSignatures.size()) {
		mutex.Unlock();
		return;
	}
	else {
		NumbertoProcess--;
	}

	// all extraction methods have been executed, set it to the output queue
	outQueue.push(region);

	// all data have been processed
	if (noMoreInputData == true && NumbertoProcess == 0)
		outQueue.finishWriting();
	mutex.Unlock();
}


void FESRequest::FinishWriting() {

	mutex.Lock();
	inQueue.finishWriting();
	noMoreInputData = true;
	mutex.Unlock();
}


FESData *FESRequest::GetData() {
FESData *data;

	mutex.Lock();
	data = this->data;
	mutex.Unlock();

	return data;
}


vector<size_t> FESRequest::GetFeatSignatures() {
vector<size_t> ids;

	mutex.Lock();
	ids = this->featSignatures;
	mutex.Unlock();

	return ids;
}


void FESRequest::AddData(FESData *data, size_t featSignature) {

	mutex.Lock();
	if (this->data != NULL)
		ReportError("Data already set!");
	this->data = data;
	this->featSignatures.clear();
	this->featSignatures.push_back(featSignature);
	mutex.Unlock();
}


void FESRequest::AddData(FESData *data, vector<size_t> &featISignatures) {

	mutex.Lock();
	if (this->data != NULL)
		ReportError("Data already set!");
	this->data = data;
	this->featSignatures = featISignatures;
	mutex.Unlock();
}








////
// class FESRequestRegions
////
FESBlocks *FESRequestRegions::GetBlocks() {
FESBlocks *blk;

	mutexLocal.Lock();
	blk = this->blocks;
	mutexLocal.Unlock();

	return blk;
}


void FESRequestRegions::AddRegion(FESRegion *region) {

	this->SetRegionToProcess(region);
}


void FESRequestRegions::EndRequests() {

	this->FinishWriting();
}


void FESRequestRegions::SetBlocks(FESBlocks *blocks) {

	mutexLocal.Lock();
	if (this->blocks != NULL)
		ReportError("Blocks already set!");

	this->blocks = blocks;
	mutexLocal.Unlock();
}


FESRegion *FESRequestRegions::RetrieveExtractedFeatures() {

	return this->GetProcessedRegion();
}



////
// class FESRequestInterestPoints
////
void FESRequestInterestPoints::SetInterestPoints(InterestPoints *intPoints) {

	mutexLocal.Lock();
	if (this->intPoints != NULL)
		ReportError("Interest points already set!");

	this->intPoints = intPoints;
	mutexLocal.Unlock();
}



InterestPoints *FESRequestInterestPoints::GetInterestPoints() {
InterestPoints *ip;

	mutexLocal.Lock();
	ip = this->intPoints;
	mutexLocal.Unlock();

	return ip;
}

