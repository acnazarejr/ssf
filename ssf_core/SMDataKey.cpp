#include "headers.h"
#include "structures.h"
#include "SMDataKey.h"


/********
 * class SMDataKey
 *******/
SMDataKey::SMDataKey(string module, string dataType, string attribute) {

	this->module = module;
	this->dataType = dataType;
	this->attribute = attribute;
	this->modFromVariable = NULL;
	this->modFromVectorVariable = NULL;

	if (SSFStructures::Instance().CheckDataType(dataType) == false)
		ReportError("Invalid type '%s'", dataType.c_str());
}


string SMDataKey::ComputeKey() {

	return this->module + this->dataType + this->attribute;
}


void SMDataKey::SetModFromVariable(string *variable) {

	this->modFromVariable = variable;
}


void SMDataKey::SetModFromVariable(vector<string> *variable) {

	this->modFromVectorVariable = variable;
}


void SMDataKey::SetModFromVariableValue(string value) {

	if (modFromVariable == NULL && modFromVectorVariable == NULL)
		ReportError("Impossible to set variable since it will provide data");

	if (modFromVariable != NULL && modFromVariable->size() > 0)
		ReportError("Input for module '%s' has already been set to '%s' (use a vector in modParams.Require() to have multiple inputs of the same type)", module.c_str(), modFromVariable->c_str());

	if (modFromVariable != NULL)
		*modFromVariable = value;

	else if (modFromVectorVariable != NULL)
		(*modFromVectorVariable).push_back(value);

	else 
		ReportError("Inconsistent situation");
}


void SMDataKey::SwapModFromVariableValue(string oldValue, string newValue) {

	if (modFromVariable != NULL)
		*modFromVariable = newValue;
	else
		ReportError("Multiple input values is not implemented!");
}

void SMDataKey::SetModFromVariableValue(vector<string> value) {

	if (modFromVectorVariable == NULL)
		ReportError("Impossible to set variable since it will provide data");

	*modFromVectorVariable = value;

}


string SMDataKey::GetModFromVariableValue() {

	if (modFromVariable == NULL)
		ReportError("Impossible read set variable since it will not receive any data");

	return *modFromVariable;
}


vector<string> SMDataKey::GetModFromVariableVectorValue() {

	if (modFromVectorVariable == NULL)
		ReportError("Impossible read set variable since it will not receive any data");

	return *modFromVectorVariable;
}



bool SMDataKey::IsTypeVector() {

	if (modFromVectorVariable == NULL && modFromVariable == NULL)
		ReportError("Type has not been set");

	if (modFromVectorVariable != NULL)
		return true;

	return false;
}