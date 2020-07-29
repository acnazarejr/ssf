#ifndef _SM_DATAKEY_H_
#define _SM_DATAKEY_H_
#include "structures.h" 



// class used to set the communication among modules using a stream of data
class SMDataKey {
string module;
string dataType;
string attribute;
string *modFromVariable;
vector<string> *modFromVectorVariable;

public:
	SMDataKey(string module, string dataType, string attribute);

	void SetModFromVariable(string *variable);

	void SetModFromVariable(vector<string> *variable);

	void SetModFromVariableValue(string value);

	void SwapModFromVariableValue(string oldValue, string newValue);

	void SetModFromVariableValue(vector<string> value);

	string GetModFromVariableValue();

	vector<string> GetModFromVariableVectorValue();

	// return true if the variables are in a string
	bool IsTypeVector();

	void SetModule(string module) { this->module = module; }

	void SetDataType(string dataType) { this->dataType = dataType; }

	void SetAttribute(string attribute) { this->attribute = attribute; }

	string GetModule() { return this->module; }

	string GetDataType() { return this->dataType; }

	string GetAttribute() { return this->attribute; }

	string ComputeKey();
};




#endif