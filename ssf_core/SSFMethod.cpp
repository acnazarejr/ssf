#include "headers.h"
#include "SSFCore.h"
#include "SSFMethod.h"




/****
 * Class SSFMethod
 ****/
SSFMethod::SSFMethod(string instanceID) {

	this->instanceID = instanceID;
}


SSFMethod::~SSFMethod() {

}


void SSFMethod::Register(SSFMethod *modPtr, string instanceID) {

	if (instanceID == "")
		glb_SSFExecCtrl.SSFMethodControl_RegisterMethod(modPtr->GetName(), modPtr->GetType(), modPtr);
}