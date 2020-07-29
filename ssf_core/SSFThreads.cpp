#include "headers.h"
#include "SSFCore.h"
#include "SSFThreads.h"


/*************************
 * class SSFMutex 
 *************************/
SSFMutex::SSFMutex() {

	if (pthread_mutex_init(&lock, NULL) != 0)
		ReportError("Mutex init failed");
}


SSFMutex::~SSFMutex() {

	pthread_mutex_destroy(&lock);
}



void SSFMutex::Lock() { 
int ret;

	ret = pthread_mutex_lock(&lock);
}


void SSFMutex::Unlock() { 

	pthread_mutex_unlock(&lock);
}


/*************************
 * class SSFRWMutex 
 *************************/
SSFRWMutex::SSFRWMutex() {

	if (pthread_rwlock_init(&lock, NULL) != 0)
		ReportError("Read/Write mutex init failed");
}


SSFRWMutex::~SSFRWMutex() {

	pthread_rwlock_destroy(&lock);
}


void SSFRWMutex::LockRead() {
int ret;

	ret = pthread_rwlock_rdlock(&lock);
}

void SSFRWMutex::LockWrite() {
int ret;

	ret = pthread_rwlock_wrlock(&lock);
}


void SSFRWMutex::Unlock() {
	pthread_rwlock_unlock(&lock);
}






/*************************
 * class SSFMutexCond 
 *************************/
SSFMutexCond::SSFMutexCond(bool initialCondition) {

	if (pthread_cond_init(&cond, NULL) != 0)
		ReportError("Conditional mutex init failed");

	mutex.Lock();

	conditionMet = initialCondition;

	mutex.Unlock();

}


SSFMutexCond::~SSFMutexCond() {

	pthread_cond_destroy(&cond);
}



void SSFMutexCond::SetConditionTrue() {
int rc;

	mutex.Lock();

	conditionMet = true;
	rc = pthread_cond_broadcast(&cond);
	//ReportStatus("pthread_cond_broadcast: %d", rc);

	mutex.Unlock();
}



void SSFMutexCond::SetConditionFalse() {
int rc;

	mutex.Lock();

	conditionMet = false;
	rc = pthread_cond_broadcast(&cond);
	//ReportStatus("pthread_cond_broadcast: %d", rc);

	mutex.Unlock();
}



void SSFMutexCond::WaitforConditiontobeTrue() {
int rc;

	mutex.Lock();

	while (conditionMet == false) {
		//ReportStatus("Thread blocked");
		rc = pthread_cond_wait(&cond, &(mutex.lock));
		//ReportStatus("pthread_cond_wait: %d", rc);
	}

	mutex.Unlock();
}



void *SSFThreadGetSelf() {
void *ptr;
	
	ptr = pthread_self().p;
	return ptr;
}