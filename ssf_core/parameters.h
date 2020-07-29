#ifndef _PARAMS_H_
#define _PARAMS_H_
#include "SSFData.h"
#include "SSFRect.h"
#include "SMDataKey.h"
#include "FeatBlockInfo.h"

class AllParameters;
class SSFParameters;





// auxiliar function to transform str1,str2,str3,...,strN to a list of strings
vector<string> GetNameList(string str);


class Parser {

protected:

	// when there is a backslash, add an extra
	void FillBackSlash(string &str);

	// remove white spaces from the string
	void RemoveWhiteSpace(string &str);

	// remove quotes from the string
	void RemoveQuotes(string &str);

	// check correct number of parameters
	void CheckNumberOfParams(int linenumber, string cmd, int current, int correct, string filename = "");

	// get a token
	string GetToken(vector<string> &tokens, int idx) { return tokens[idx]; }

	// add command
	virtual void AddCommand(int linenumber, vector<string> tokens, string filename = "") { ; }

public:
	// get the tokens from lines
	vector<string> ExtractLineTokens(string strline, int linenumber);

	// set file with input parameters
	void ParseFile(string paramFile);

	// entries are in a string vector
	void ParseFileVector(vector<string> &lines); 
};
















/******
 * class FeatureParams
 ******/
/* 
 * params <feature, setup, ID>
 * params <feature, block, keyword, w, h, stride_x, stride_y>
 */
class FeatureParams : protected Parser {
set<string> setups;									// setup for the feature extraction method
map<string, vector<FeatBlockInfo> > Blocks;			// blocks for different feature extraction methods

public:
	void SetParameter(int linenumber, vector<string> tokens, string filename);

	// retrieve blocks for a given block configuration
	vector<FeatBlockInfo> GetBlocks(string blockID);

	// Return the methods that will be considered for feature extraction
	vector<string> GetSetups();

	/* Check if a feature extraction method is available */
	bool CheckAvailableSetup(string setupID);
};





/****
 * class Parameters
 ****/
class Parameters { 

protected:
	map<string, multimap<string, string> > params;		// keep all commands type params <key, name, value>

public:
	void SetParameters(string paramFile, string setupID) { ; } // ??? TODO }

	// retrieve parameters for a given key
	multimap<string, string> GetParams(string paramKey);

	// retrieve a value for a given key (returnError: returns an error if it is not found)
	string GetParams(string paramKey, string name, bool returnError = false);

	// set specific parameter
	//void SetParam(string paramKey, string name, string value);

	// retrieve only chains of parameters that depend on the key
	//Parameters RetrieveParameters(string key);
};




/****
 * class ValueArg
 ****/
class ValueArg {
string name;
string desc;
bool req;
string type;
void *ptr;
int nvalues;

public:
	ValueArg(string name, string desc, bool req, string *ptr);

	ValueArg(string name, string desc, bool req, int *ptr);

	ValueArg(string name, string desc, bool req, bool *ptr);

	ValueArg(string name, string desc, bool req, float *ptr);

	ValueArg(string name, string desc, bool req, vector<string> *ptr);
	
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
 * class InputParameters
 ****/
class InputParameters {
friend class SSFMethod;
friend class SSFStructure;
friend class SSFExecutionControl;

protected:
unordered_map<string, ValueArg *> args;

	// save all parameters
	void SaveParameters(SSFStorage &storage);

	// set input parameters to the arguments
	void SetParameters(const FileNode &node, SSFStorage &storage, string ID);

public:
	void Add(string name, string *variable, string desc, bool req);

	void Add(string name, int *variable, string desc, bool req);

	void Add(string name, float *variable, string desc, bool req);

	void Add(string name, bool *variable, string desc, bool req);

	void Add(string name, vector<string> *variable, string desc, bool req);

	void SetParameters(string ID, AllParameters *params); 

	void SetParameters(string ID, SSFParameters *params); 

	void PrintHelp(string modName);

	// retrieve a string with all keys and their values
	string ReturnAllParameters();


};






/****
 * Class to retrieve parameters in the configuration file
 ****/
class AllParameters : public Parser, public Parameters {
FeatureParams featParams;
map<string, string> typeModules;		// map having the modules and its type (User, Internal, PipeSeq, PipeMod)

public:
	// add command
	void AddCommand(int linenumber, vector<string> tokens, string filename);
	 
	// Retrieve parameters for feature extraction
	FeatureParams RetrieveFeatParams() { return featParams; }

	// Retrieve module type according to its original ID
	string RetrieveModuleType(string id);
};






#endif
