#include "headers.h"
#include "structures.h"
#include "SSFCamera.h"


SSFCamera::SSFCamera() {

	threadlock.Lock();
	this->SetDataType(SSF_DATA_CAMCOMM);
	threadlock.Unlock();
}



string SSFCamera::GetCommand() {
string str;

	threadlock.Lock();
	str = this->data;
	threadlock.Unlock();

	return str;
}

void SSFCamera::SetCommand(string str) {

	threadlock.Lock();
	this->data = str;
	threadlock.Unlock();
}
