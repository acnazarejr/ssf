#include "headers.h"
#include "structures.h"
#include "SSFObject.h" 


SSFObject::SSFObject() {

	mutex.Lock();
	this->SetDataType(SSF_DATA_OBJECT);
	this->objID = "";
	mutex.Unlock();
}



void SSFObject::AddTrackletID(SMIndexType trackID) {

	mutex.Lock();
	tracksID.push_back(trackID);
	mutex.Unlock();
}


vector<SMIndexType> SSFObject::GetTrackletIDs() {
vector<SMIndexType> ids;

	mutex.Lock();
	ids = tracksID;
	mutex.Unlock();

	return ids;
}


void SSFObject::SetObjID(string objID) {

	mutex.Lock();
	this->objID = objID;
	mutex.Unlock();
}



string SSFObject::GetObjID() {
string id;

	mutex.Lock();
	id = this->objID;
	mutex.Unlock();

	return id;
}



SMIndexType SSFObject::GetIndex() {
SMIndexType idx;

	mutex.Lock();
	idx = this->GetSMID();		// retrieve the unique identifier
	mutex.Unlock();

	return idx;
}


void SSFObject::Save(SSFStorage &storage) {

	storage << "SSFObject" << "{";
	storage << "objID"  << objID;
	//storage << "tracksID" << tracksID;
	storage << "}";
}

void SSFObject::Load(const FileNode &node) {

}