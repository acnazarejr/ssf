#ifndef _SSF_PARAMETERS_H_
#define _SSF_PARAMETERS_H_



typedef struct {
	string execID;
	string outputID;
	string instanceID;
} ParamsExecInput;


typedef struct {
	string moduleID;
	string dataType;
} ParamsExecOutput;


typedef struct {
	string moduleID;
	string dataType;
	string attribute;
	string inattribute;
} ParamsModuleInput;



class ParamsPipeline {
public:
unordered_map<string, vector<ParamsModuleInput> > modules;

	vector<string> RetrieveModSetupIDs();

	vector<ParamsModuleInput> RetrieveInputs(string setupID);
};


class ParamsExecution {
public:
unordered_map<string, ParamsExecInput> inputs;		// inputs for the pipeline, indexed by the input ID
unordered_map<string, ParamsExecOutput> outputs;	// inputs for the pipeline, indexed by the input ID
string pipeID;		// ID of the pipeline that will be launched
string pipeAttr;	// attribute of the pipeline for this execution
string execID;		// execution ID

};




class SSFParameterNode {

public:
unordered_map<string, SSFParameterNode *> mapEntries;
vector<SSFParameterNode *> vecEntries;
string name;
string value;

	// ending node has a variable and a value
	bool IsEndingNode();

	// check if it is a map - list of named variables and values
	bool IsMap();

	// check if it is a sequence - list of unamend values
	bool IsSequence();

	// check if node is empty 
	bool IsEmpty();
};



class SSFParameters {
unordered_map<string, SSFParameterNode *> roots;

	void SetNode(SSFParameterNode *rootParamNode, FileNode &node);

	// retrieve a node by some criterium
	SSFParameterNode *RetrieveNode(string ID, string variable);
	SSFParameterNode *RetrieveNode(string ID);
	SSFParameterNode *RetrieveNode(SSFParameterNode *node, string variable);

	string GetValue(SSFParameterNode *node, string variable);

public:
	void SetFromFile(string filename);

	string GetValue(string ID, string variable);

	// retrieve data from entry "parameters" inside entry ID
	map<string, vector<string> > GetParameters(string ID);

	// retrieve a pipeline by its ID
	ParamsPipeline RetrievePipeline(string ID);

	// retrieve execution instance by its ID
	ParamsExecution RetrieveExecutionSequence(string ID);
};




/****
 * class SSFValueArg
 ****/
class SSFValueArg {
string name;
string desc;
bool req;
string type;
void *ptr;
int nvalues;

public:
	SSFValueArg(string name, string desc, bool req, string *ptr);

	SSFValueArg(string name, string desc, bool req, int *ptr);

	SSFValueArg(string name, string desc, bool req, bool *ptr);

	SSFValueArg(string name, string desc, bool req, float *ptr);

	SSFValueArg(string name, string desc, bool req, vector<string> *ptr);
	
	void SetValue(string value);

	void SetVecValue(vector<string> value);

	bool Required() { return req; }

	string GetDesc() { return desc; }
	
	string GetValue();

	vector<string> GetVecValue();

	int GetNValues() { return nvalues; }

	string GetType() { return type; }

	
};





/****
 * class SSFInputParameters
 ****/
class SSFInputParameters {
friend class SSFMethod;
friend class SSFStructure;
friend class SSFExecutionControl;

protected:
unordered_map<string, SSFValueArg *> args;						// parameters for this structure
unordered_map<SSFStructure *, string> subStructures;		// parameters for eventual structures inside this structure

	// save all parameters
	void SaveParameters(SSFStorage &storage);

	// set input parameters to the arguments
	//void SetParameters(const FileNode &node, SSFStorage &storage, string ID);

	// set parameters for all SSFStructure
	void SetParametersStructures(SSFParameters *params);

public:
	void Add(string name, string *variable, string desc, bool req);

	void Add(string name, int *variable, string desc, bool req);

	void Add(string name, float *variable, string desc, bool req);

	void Add(string name, bool *variable, string desc, bool req);

	void Add(string name, vector<string> *variable, string desc, bool req);

	// structure for which the parameters will be set, name is the variable that will contain the setupID 
	// with parameters for this structure
	void SetStructure(SSFStructure *structure, string name, string *variable, string desc, bool req);

	void SetParameters(string ID, SSFParameters *params); 

	void PrintHelp(string modName);

	// retrieve a string with all keys and their values
	string ReturnAllParameters();
};


#endif