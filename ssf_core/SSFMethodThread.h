#ifndef _SSF_METHOD_THREAD_H_
#define _SSF_METHOD_THREAD_H_



/* 
 * Class for methods that can be launched as a new thread, such as Module
 */
class SSFMethodThread : public SSFMethod {
friend class SSFMethodControl;

protected:
SSFThreadExecuteNew *ssfThreadExecute;			// thread created for this method

	// initialize thread
	void InitializeThread();

	// function to kill the method immediately
	void KillExecution();

	// function to request the module to end (finish the execution first)
	virtual void RequestFinish() = 0;

public:
	SSFMethodThread(string instanceID);

	// check if this method can be launched to a thread
	virtual bool EnabletoLaunch() { return true; }

	// function to launch this method
	virtual void Launch() = 0;

	// method execution (infinite loop)
	virtual void Execute() = 0;
};



#endif