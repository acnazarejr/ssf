#include "headers.h"
#include "SSFCore.h"
#include "SSFInternalModule.h"








/******
 * class SSFInternalModule 
 ******/
SSFInternalModule::SSFInternalModule(string instanceID) : SSFMethodThread(instanceID) {

}


void SSFInternalModule::Launch() {

	this->Execute();
}


