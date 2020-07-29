#include "headers.h"
#include "SSFAsync.h"


SSFAsync::SSFAsync() {

	writeAllowed = true;
	itemDataType = "";
}


void SSFAsync::EndWriting() {

	mutex.Lock();
	this->writeAllowed = false;

	// set that the current position + 1 was set - to indicate that write finished 
	dataCond.UnlockCondition((SMIndexType) SMIDs.size());
	mutex.Unlock();
}

/*
bool SSFAsync::CanWrite() {
bool v;
	
	mutex.Lock();
	v = this->writeAllowed;
	mutex.Unlock();

	return v;
}
*/


void SSFAsync::AddDataItem(SMIndexType smID) {

	// check if it is in the SM
	//if (data->GetSMID() != SM_INVALID_ELEMENT)
	//	ReportError("The data item must be added in the shared memory first");

	mutex.Lock();
	// check if it still possible to write
	if (this->writeAllowed == false) {
		mutex.Unlock();
		ReportError("It is no longer allowed to add more items (entry ID: %d)", smID);
	}

	// check if data type is the same as the ones in the vector
	//if (SMIDs.size() != 0 && data->GetDataType() != itemDataType) {
	//	mutex.Unlock();
	//	ReportError("Data type must be the same for all items (expected to be '%s', but a '%s' was passed)", 
	//		itemDataType.c_str(), data->GetDataType().c_str());
	//}

	// set the first data type
	//if (SMIDs.size() == 0)
	//	itemDataType = data->GetDataType();

	// set data entry ID
	SMIDs.push_back(smID);

	// set that the current position of the vector has been set
	dataCond.UnlockCondition((SMIndexType) SMIDs.size() - 1);
	mutex.Unlock();
}



SMIndexType SSFAsync::GetDataEntry(SMIndexType position) {
SMIndexType id = SM_INVALID_ELEMENT;
SMIndexType idx; 

	// check consistency
	if (position < 0)
		ReportError("Invalid requested position: %d", position);

	mutex.Lock();
	// if the entry in the requested position is ready, return it
	if (position < (SMIndexType) SMIDs.size()) {
		id = SMIDs[position];
	}
	else {
		// check if the position is invalid (write was set to finish)
		if (writeAllowed == false && position >= (SMIndexType) SMIDs.size()) {
			mutex.Unlock();
			return SM_INVALID_ELEMENT;
		}

		// wait until the data is ready
		mutex.Unlock();
		dataCond.CheckCondition(position);
		mutex.Lock();

		// remove conditions smaller than the current one (that is not need anymore)
		idx = position;
		while (dataCond.EraseCondition(idx--) == true);

		// check if the position is invalid (write was set to finish)
		if (writeAllowed == false && position >= (SMIndexType) SMIDs.size())
			id = SM_INVALID_ELEMENT;
		else
			id = SMIDs[position];
	}
	mutex.Unlock();

	return id;
}