#include "headers.h"
#include "SSFData.h"
#include "SSFGarbageCollector.h"

SSFData::SSFData() {

	dataType = "notSet";
	modID = "notSet";
	attribute = "";
	unknownSrc = false;
	syncTag = SM_INVALID_ELEMENT;
	timeStamp = GenerateTimeStamp();
	idSM = SM_INVALID_ELEMENT;
	glb_SSFGarbageCollector.AddEntry(this);
}

SSFData::~SSFData() {

	glb_SSFGarbageCollector.RemoveEntry(this);
}

void SSFData::SetDataType(string dataType) {

	this->dataType = dataType;
}


void SSFData::SetModuleID(string modID) {

	this->modID = modID;
}




void SSFData::SetAttribute(string attribute) {

	this->attribute = attribute;
}


void SSFData::SetUnknown() {

	this->unknownSrc = true;
}

void SSFData::SetTimeStamp(size_t timeStamp) {

	this->timeStamp = timeStamp;
}

void SSFData::SetSynchonizationTag(SMIndexType tagID) {

	this->syncTag = tagID;
}

SMIndexType SSFData::GetSynchonizationTag() {

	return this->syncTag;
}


void SSFData::SetSMID(SMIndexType idSM) {

	this->idSM = idSM;
	glb_SSFGarbageCollector.RemoveEntry(this);
}




string SSFData::GetDataType() {

	return this->dataType;
}


string SSFData::GetModID() {

	return this->modID;
}


string SSFData::GetAttribute() {

	return attribute;
}

bool SSFData::IsUnknownSource() {

	return unknownSrc;
}


size_t SSFData::GetTimeStamp() {

	return timeStamp;
}

SMIndexType SSFData::GetSMID(bool error) {

	if (idSM == SM_INVALID_ELEMENT && error == true)
		ReportError("Index has not been set");

	return idSM;
}
