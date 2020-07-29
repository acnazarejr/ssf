#include "headers.h"
#include "structures.h"
#include "SSFLAPData.h"

SSFLAPData::SSFLAPData() {

	frameID = SM_INVALID_ELEMENT;
}



void SSFLAPData::SetFrameID(SMIndexType frameID) {

	this->frameID = frameID;
}


SMIndexType SSFLAPData::GetFrameID() {

	return this->frameID;
}