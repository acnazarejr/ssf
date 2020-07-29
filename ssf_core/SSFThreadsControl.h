#ifndef _SSF_THREADS_CONTROL_H_
#define _SSF_THREADS_CONTROL_H_


class SSFMethodThread;


class SSFThreadExecuteNew {
pthread_t threadID;

public:
	// initialize a thread that will call a module passed as parameter
	pthread_t Initialize(SSFMethodThread *method);

	// function to wait the module to finish
	void WaitToFinish();

	// send signal to kill this thread
	void KillThread();
};






#endif