#include "headers.h"
#include "structures.h"
#include "SSFString.h"


SSFString::SSFString() {

	threadlock.Lock();
	this->SetDataType(SSF_DATA_STRING);
	threadlock.Unlock();
}



string SSFString::GetString() {
string str;

	threadlock.Lock();
	str = this->data;
	threadlock.Unlock();

	return str;
}

void SSFString::SetString(string str) {

	threadlock.Lock();
	this->data = str;
	threadlock.Unlock();
}


void SSFString::Save(SSFStorage &storage) {

	if (storage.isOpened() == false)
		ReportError("Invalid file storage!");

	storage << "SSFString" << "{";
	storage << "data" << data;
	storage << "}";
}