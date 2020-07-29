#ifndef _SSF_METHOD_CONTROL_H_
#define _SSF_METHOD_CONTROL_H_
#include "headers.h"



class SSFMethodType {
unordered_map<string, SSFMethod *> methodList;			// list of available methods

	SSFMethod *RetrieveMethod(string methodName);

public:
	// register a method
	void RegisterModule(string methodName, SSFMethod *methodPtr);

	// return chosen detector to the user
	SSFMethod *InstanciateMethod(string methodName, string instanceID);
};


class SSFMethodControl {
SSFMutex mutex;
SSFMutex mutex_finish;											// mutex to finish methods (don't allow multiple requests at once)
unordered_map<string, SSFMethodType *> methodTypes;				// list of methods by type
unordered_map<string, SSFMethod *> instantiatedMethods;			// list of all instantiated methods
unordered_map<string, SSFMethodThread *> runningMethods;		// list of all running methods (only those that support threads)
vector<SSFStructure *> instantiatedStructure;					// list of instantiated structures

	// find the next available instance ID with prefix methodType:methodName:%.5d
	string GenerateInstanceID(string methodName, string methodType);

public:
	// register a new method
	void RegisterMethod(string methodName, string methodType, SSFMethod *methodPtr);

	// instantiate a method
	SSFMethod *InstanciateMethod(string methodName, string methodType, string instanceID);

	// launch a thread for a method
	void LaunchThread(SSFMethodThread *method);

	// function to kill all running threads
	void KillRunningThreads();

	// send request to end the execution of all threads (wait until they finish)
	void RequestEndThreadMethods();

	// wait until user module to finish execution
	void WaitUserModuletoFinish(string instanceID);

	// save parameters of instantiated methods
	void SaveParameters(SSFStorage &storage);

	// set an instantiated SSFStructure
	void SetInstantiatedStructure(SSFStructure *structure);

	// print user modules available
	void ListUserModulesAvailable();
};


#endif