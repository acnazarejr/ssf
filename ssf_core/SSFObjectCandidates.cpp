#include "headers.h"
#include "structures.h"
#include "SSFData.h"
#include "SSFObjectCandidates.h" 




/******************
 * class SSFObjectCandidates *
 ******************/
SSFObjectCandidates::SSFObjectCandidates() {

	mutex.Lock();
	this->SetDataType(SSF_DATA_OBJECTCANDIDATES);
	this->trackletID = SM_INVALID_ELEMENT;
	mutex.Unlock();
}

SSFObjectCandidates::SSFObjectCandidates( SMIndexType trackletID, vector<SSFObjectCandidate> &objectCandidates ) {
	mutex.Lock();
	this->SetDataType(SSF_DATA_OBJECTCANDIDATES);
	this->trackletID = trackletID;
	this->objCandidates.insert( this->objCandidates.end(), objectCandidates.begin(), objectCandidates.end() );
	sort( this->objCandidates.begin(), this->objCandidates.end() );
	mutex.Unlock();
}

SSFObjectCandidates::SSFObjectCandidates( SMIndexType trackletID ) {

	mutex.Lock();
	this->SetDataType(SSF_DATA_OBJECTCANDIDATES);
	this->trackletID = trackletID;
	mutex.Unlock();
}


SSFObjectCandidates::~SSFObjectCandidates() {

}

SMIndexType SSFObjectCandidates::GetTrackletID() {
	return trackletID;
}

const vector<SSFObjectCandidate> *SSFObjectCandidates::GetObjectCandidates() {
	return &objCandidates;
}
