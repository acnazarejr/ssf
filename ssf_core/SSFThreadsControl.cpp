#include "headers.h"
#include "SSFCore.h"
#include "SSFThreads.h"
#include "SSFThreadsControl.h"



/***********************
 * SSFThreadExecuteNew
 ***********************/

// function to start a new thread
void *fstartxNew(void *x){
SSFMethodThread *method;


  method = (SSFMethodThread *) x;
  glb_SSFExecCtrl.SSFMethodControl_SetThreadID(method->GetInstanceID());
  method->Launch();

  // exit thread
  pthread_exit(NULL);  
  return NULL;
}

pthread_t SSFThreadExecuteNew::Initialize(SSFMethodThread *method) {

	pthread_create(&threadID, NULL, &fstartxNew, (void *) method);

	return threadID;
}

void SSFThreadExecuteNew::WaitToFinish() {
	pthread_join(threadID, NULL);
}

void SSFThreadExecuteNew::KillThread() {
int ret;

	ret = pthread_kill(threadID, 0);
	if (ret != 0)
		ReportError("Error when killing the thread!");
	this->WaitToFinish();
}