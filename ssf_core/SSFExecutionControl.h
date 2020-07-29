#ifndef _SSF_EXECUTION_CONTROL_H_
#define _SSF_EXECUTION_CONTROL_H_


class SSFMethodThread;
class SSFMethodControl;
class FESRequest;
class SSFParametersNew;
class ParamsExecution;
class ParamsPipeline;
class SSFExecutionSequence;




class SSFExecutionControl : public SSFStructure {
friend class SSFStructure;
friend class FESControl;
friend class FESMethodManager;
SSFMutex mutex;
SSFMutex mutexMapThreads;
unordered_map<string, SSFInternalModule *> internalModules;	// map by name
unordered_map<string, SSFUserModule *> userModules;			// map by ID
SSFQueue<SSFUserModule *> runningUserModules;				// queue with user modules to wait to finish

// class to control available methods
SSFMethodControl *ssfMethodControl;

// thread control
map<void *, string> mapThreads;								// map from the thread ID to the specific module

// execution sequence control
map<string, SSFExecutionSequence *> execSeqControl;			// execution sequences

// instanceID maps
map<string, string> instanceIDsMap;

// parameters
SSFParameters *parameters;									// parameters from the input

// local parameters
vector<string> internalModulesIDs;							// IDs of the internal modules
vector<string> userModulesIDs;								// IDs of the user modules that will run at the beginning
vector<string> execSeqs;									// IDs of execution sequences
string SMID;												// identifier of the shared memory setup

	// if ErrorMsg == true, abort when the module is not found
	SSFMethodThread *RetrieveInternalModule(string methodName, bool ErrorMsg = false);

	// launch internal modules
	void LaunchInternalModules();

	// instantiate user modules that will start outside of pipelines
	void InstantiateReadyUserModules();

	// execute pipelines
	void SetPipelines();

	// create one execution sequence
	void CreateExecutionSequence(SSFParameters *params, ParamsExecution execParams, ParamsPipeline pipeParams);

	// correct instanceIDs for the user modules
	void CorrectInstanceIDsUserModules();

	// setup shared memory of the user modules with their provided data
	void UserModulesSetupSharedMemory();

	// launch all user modules
	void LaunchUserModules();

	// set an instantiated SSFStructure
	void SetInstantiatedStructure(SSFStructure *structure);

	// local setup
	void Setup();

	// Return the name of the module
	string GetName() { return SSF_STRUCTURE_EXECCONTROL; }

	// retrieve the type of the method
	string GetType() { return SSF_METHOD_TYPE_STRUCTURE; }

	// version
	string GetVersion() { return "0.0.1"; }

public:
	SSFExecutionControl();
	~SSFExecutionControl();

	// initialized execution control
	void Initialize(string paramFile, string controlID);
	void Initialize(SSFParameters *params, string controlID);

	/* SSFMethod */
	// register a new SSFMethod method - can be either a thread or not 
	void SSFMethodControl_RegisterMethod(string methodName, string methodType, SSFMethod *methodPtr);

	// instantiate a method
	SSFMethod *SSFMethodControl_InstanciateMethod(string methodName, string methodType, string instanceID);

	// launch a method if it is allowed
	void SSFMethodControl_LaunchMethod(SSFMethodThread *method);

	// link thread id and the module ID
	void SSFMethodControl_SetThreadID(string instanceID);

	// retrieve ID of method been executed in the current thread (if setError == true, abort execution)
	string GetInstanceIDCurrentThread(bool setError = true);


	/* FES */
	// initialize a new feature extraction method - return its signature
	size_t FES_InitializeExtractionMethod(string setupID, string paramFile);

	// set feature extraction request
	void FES_SetRequest(FESRequest *request);


	/* Misc */
	// kill all running threads immediatelly
	void KillRunningThreads();

	// send request to end the execution of all threads (wait until they finish)
	void WaitModulesToEnd();

	// save all methods parameters
	void SaveMethodsParameters(string filename);

	// print all modules available
	void ListUserModulesAvailable();
};




#endif