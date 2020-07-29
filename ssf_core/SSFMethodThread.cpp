#include "headers.h"
#include "SSFCore.h"
#include "SSFMethodThread.h"


/******
 * class SSFMethodThread 
 ******/
SSFMethodThread::SSFMethodThread(string instanceID) : SSFMethod(instanceID) {

	this->ssfThreadExecute = NULL;
}


void SSFMethodThread::InitializeThread() {

	if (this->ssfThreadExecute != NULL)
		ReportError("Thread is already initialized for method ID '%s'", this->GetInstanceID());

	// initialize thread for this method
	this->ssfThreadExecute = new SSFThreadExecuteNew();
	this->ssfThreadExecute->Initialize(this);
}


void SSFMethodThread::KillExecution() {

	ssfThreadExecute->KillThread();

	delete ssfThreadExecute;
	ssfThreadExecute = NULL;
}