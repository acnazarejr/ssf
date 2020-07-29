#ifndef _SSF_EXECUTION_SEQUENCE_H_
#define _SSF_EXECUTION_SEQUENCE_H_



class SSFExecutionSequence { 
ParamsExecution execParams;							// execution sequence parameters
ParamsPipeline pipeParams;							// pipeline parameters
unordered_map<string, SSFUserModule *> methods;		// list with setupID -> instantiated used methods
size_t entryID;										// unique ID for this entry
map<string, string> mapSetupIDtoModID;				// map setupID in pipe to the instantiated module ID
map<string, string> mapOutputtoModID;				// map from outputID to the instantiated module ID

	// retrieve instantiated SSFMethod by setup ID
	SSFUserModule *RetrieveMethod(string setupID);

	// create output mapping
	void CreateOutputMapping();

public:
	SSFExecutionSequence(ParamsExecution execParams, ParamsPipeline pipeParams, size_t entryID); 

	// instantiate modules
	void InstanciateModules(SSFParameters *parameters, SSFMethodControl *ssfMethodControl);

	// set input dependences
	void SetModulesDependencies();

	// setup all modules
	void SetupModules(SSFParameters *parameters);

	// retrieve output to instanceID mapping
	map<string, string> RetrieveOutputToInstanceIDMapping() { return mapOutputtoModID; }

	// retrieve instantiated modules
	vector<SSFUserModule *> RetrieveModules();
};


#endif