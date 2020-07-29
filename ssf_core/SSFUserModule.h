#ifndef _SSF_USER_MODULE_H_
#define _SSF_USER_MODULE_H_



/****
 * class SSFModParameters
 ****/
class SSFModParameters  {
map<string, SMDataKey> required;
map<string, SMDataKey> provided;

public:
	// Set a data type that the module requires as input
	void Require(string modID, string dataType, string *modFromVariable, string attribute = "");

	// Set a data type that the module requires as input (a vector of modules as input)
	void Require(string modID, string dataType, vector<string> *modFromVariable, string attribute = "");

	// set a data type that the module provides as output
	void Provide(string modID, string dataType, string attribute = "");

	// retrieve the required data type for this module
	vector<SMDataKey> GetRequiredDataType();

	// retrieve the provided data type for this module
	vector<SMDataKey> GetProvidedDataType();
};



/* 
 * Class for User Modules
 */
class SSFUserModule : public SSFMethodThread { 
friend class SSFExecutionSequence;
friend class SSFExecutionControl;
SSFMutex mutex;
unordered_map<SMIndexType, int> entriesID;		// keep entries that haven't be released
bool launched;									// true if this module has been launched


protected:
SSFModParameters modParams;

	// do nothing for user modules
	void RequestFinish() { ; } 

	// launch module
	void Launch();

	// setup shared memory
	void SetupSharedMemory();

public:
	SSFUserModule(string instanceID);
	
	// retrieve the type of the method
	string GetType() { return SSF_METHOD_TYPE_USERMOD; }

	// check if this user module has been launched
	bool IsLaunched();

	// retrieve the class of this user module
	virtual string GetClass() = 0;
};




#endif