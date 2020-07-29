#include "headers.h"
//#include "parameters.h"
#include "SSFCore.h"
#include "SSFParameters.h"

/**********
 * class SSFParameterNode
 ***********/
 bool SSFParameterNode::IsEndingNode() {

	 if (mapEntries.size() == 0 && vecEntries.size() == 0)
		 return true;

	 return false;
}


 bool SSFParameterNode::IsMap() {

	 if (mapEntries.size() != 0)
		 return true;

	 return false;
 }


 bool SSFParameterNode::IsSequence() {

	 if (vecEntries.size() != 0) 
		 return true;

	 return false;
 }


 bool SSFParameterNode::IsEmpty() {

	 if (vecEntries.size() == 0 && mapEntries.size() == 0 && value.size() == 0)
		 return true;

	 return false;
 }
 


 /**********
 * class ParamsPipeline
 ***********/
vector<string> ParamsPipeline::RetrieveModSetupIDs() {
unordered_map<string, vector<ParamsModuleInput> >::iterator it;
vector<string> ids;

	for (it = modules.begin(); it != modules.end(); it++) 
		ids.push_back(it->first);

	return ids;
}

vector<ParamsModuleInput> ParamsPipeline::RetrieveInputs(string setupID) {
unordered_map<string, vector<ParamsModuleInput> >::iterator it;

	it = modules.find(setupID);

	if (it == modules.end())
		ReportError("SetupID '%s' was not found", setupID.c_str());

	return it->second;
}




/**********
 * class SSFParameters
 **********/
void SSFParameters::SetNode(SSFParameterNode *rootParamNode, FileNode &node) {
SSFParameterNode *paramNode;
FileNodeIterator it;
	
	if (node.isString() == true) {
		printf("end: %s= %s\n", node.name().c_str(), ((string) node).c_str());
		rootParamNode->name = node.name();
		rootParamNode->value = (string) node;
	}
	else if (node.isMap() == true) {
		printf("map: %s\n", node.name().c_str());
		rootParamNode->name = node.name();
		for (it = node.begin(); it != node.end(); it++) {
			paramNode = new SSFParameterNode();
			rootParamNode->mapEntries.insert(pair<string, SSFParameterNode *>((*it).name(), paramNode));
			this->SetNode(paramNode, *it);
		}
	}
	else if (node.isSeq() == true) {
		printf("seq: %s\n", node.name().c_str());
		rootParamNode->name = node.name();
		for (it = node.begin(); it != node.end(); it++) {
			paramNode = new SSFParameterNode();
			rootParamNode->vecEntries.push_back(paramNode);
			this->SetNode(paramNode, *it);
		}
	}
}



SSFParameterNode *SSFParameters::RetrieveNode(string ID, string variable) {
unordered_map<string, SSFParameterNode *>::iterator it;
SSFParameterNode *node;

	it = roots.find(ID);

	if (it == roots.end()) 
		ReportError("Entry ID '%s' does not exist in the parameters", ID.c_str());

	node = it->second;

	// check if variable exist
	it = node->mapEntries.find(variable);
	if (it == node->mapEntries.end()) 
		ReportError("Variable '%s' was not found inside entry ID '%s'", variable.c_str(), ID.c_str());

	return it->second;
}



SSFParameterNode *SSFParameters::RetrieveNode(string ID) {
unordered_map<string, SSFParameterNode *>::iterator it;

	it = roots.find(ID);

	if (it == roots.end()) 
		ReportError("Entry ID '%s' does not exist in the parameters", ID.c_str());

	return it->second;
}



SSFParameterNode *SSFParameters::RetrieveNode(SSFParameterNode *node, string variable) {
unordered_map<string, SSFParameterNode *>::iterator it;

	// check if variable exist
	it = node->mapEntries.find(variable);
	if (it == node->mapEntries.end()) 
		ReportError("Variable '%s' was not found inside node '%s'", variable.c_str(), node->name.c_str());

	return it->second;
}


string SSFParameters::GetValue(SSFParameterNode *node, string variable) {
unordered_map<string, SSFParameterNode *>::iterator it;
SSFParameterNode *node2;

	if (node->IsMap() == false)
		ReportError("Invalid node type, must be a map");
	
	// search for the variable
	it = node->mapEntries.find(variable);
	if (it == node->mapEntries.end())
		ReportError("Variable '%s' was not found in the node", variable.c_str());

	// retrieve the node with the value
	node2 = it->second;

	if (node2->IsEndingNode() == false)
		ReportError("This node must be a leaf!");

	return node2->value;
}



void SSFParameters::SetFromFile(string filename) {
SSFParameterNode *rootParamNode;
SSFStorage storage;
FileNodeIterator it;
FileNode node;


	storage.open(filename, SSF_STORAGE_READ);

	if (storage.isOpened() == false)
		ReportError("Could not open file '%s' with parameters!", filename.c_str());

	node = storage.root();

	for (it = node.begin(); it != node.end(); it++) {
		printf("%s\n", (*it).name().c_str());
		rootParamNode = new SSFParameterNode();
		this->SetNode(rootParamNode, *it);
		roots.insert((pair<string, SSFParameterNode *>((*it).name(), rootParamNode)));
	}

	storage.release();
}



string SSFParameters::GetValue(string ID, string variable) {
SSFParameterNode *node;

	node = RetrieveNode(ID, variable);

	// check if this is an ending node
	if (node->IsEndingNode() == false)
		ReportError("Variable '%s' in entry ID '%s' is not a ending node!", variable.c_str(), ID.c_str());

	return node->value;
}



map<string, vector<string> > SSFParameters::GetParameters(string ID) {
map<string, vector<string> > retData;
vector<string> values;
unordered_map<string, SSFParameterNode *>::iterator it;
SSFParameterNode *node, *currNode, *seqNode;
size_t i;

	node = RetrieveNode(ID, "parameters");

	// no parameters are available
	if (node->IsEmpty() == true)
		return retData;

	if (node->IsMap() == false)
		ReportError("Variable 'parameters' in entry ID '%s' does not contain set of parameters!", ID.c_str());

	// go through every pair <variable, value>
	for (it = node->mapEntries.begin(); it != node->mapEntries.end(); it++) {
		currNode = it->second;
		values.clear();

		// if the node is empty
		if (currNode->IsEmpty() == true) {
			;
		}
		
		// if it is an ending node (variable = "value")
		else if (currNode->IsEndingNode() == true) {
			values.push_back(currNode->value);
		}

		// if it is a sequence node (variable = "v1", "v2", ..., "vn")
		else if (currNode->IsSequence() == true) {
			for (i = 0; i < currNode->vecEntries.size(); i++) {
				seqNode = currNode->vecEntries[i];
				values.push_back(seqNode->value);
			}
		}

		else {
			ReportError("Invalid node, cannot be a map at this point!");
		}

		// add data to the returning structure
		retData.insert(pair<string, vector<string> >(currNode->name, values));
	}

	return retData;
}



ParamsPipeline SSFParameters::RetrievePipeline(string ID) {
unordered_map<string, SSFParameterNode *>::iterator it;
SSFParameterNode *node, *node2, *node3, *node4;
vector<ParamsModuleInput> inputs;
ParamsModuleInput input;
ParamsPipeline pipe;
size_t i;

	// retrieve list of modules
	node = RetrieveNode(ID, "modules");

	// check if node is a map
	if (node->IsMap() == false)
		ReportError("Invalid node type for 'modules', must be a map!");

	// retrieve each module in the pipeline
	for (it = node->mapEntries.begin(); it != node->mapEntries.end(); it++) {
		node2 = it->second;

		// check if is a correct type of node
		if (node->IsMap() == false)
			ReportError("Invalid node type for '%s', must be a sequence", node2->name.c_str());

		node3 = RetrieveNode(node2, "inputs");

		// check if is a correct type of node
		if (node3->IsSequence() == false)
			ReportError("Invalid node type for '%s', must be a sequence", node3->name.c_str());

		// retrieve module inputs
		inputs.clear();
		for (i = 0; i < node3->vecEntries.size(); i++) {
			node4 = node3->vecEntries[i];

			// retrieve variables
			input.attribute = GetValue(node4, "attribute");
			input.inattribute = GetValue(node4, "inattribute");
			input.dataType = GetValue(node4, "dataType");
			input.moduleID = GetValue(node4, "moduleID");

			inputs.push_back(input);
		}

		pipe.modules.insert(pair<string, vector<ParamsModuleInput> >(node2->name, inputs));
	}

	return pipe;
}



ParamsExecution SSFParameters::RetrieveExecutionSequence(string ID) {
unordered_map<string, SSFParameterNode *>::iterator it;
SSFParameterNode *node, *node2;
ParamsExecInput input;
ParamsExecOutput output;
ParamsExecution exec;

	// retrieve node with the execution sequence
	node = RetrieveNode(ID);

	// execution sequence ID
	exec.execID = ID;

	// retrieve pipeline ID
	exec.pipeID = GetValue(node, "pipeID");

	// retrieve pipeline attribute
	exec.pipeAttr = GetValue(node, "pipeAttr");

	// retrieve list of inputs
	node = RetrieveNode(ID, "inputs");	

	// check if node is a map
	//if (node->IsMap() == false)
	//	ReportError("Invalid node type for 'inputs', must be a map!");

	// retrieve each input
	for (it = node->mapEntries.begin(); it != node->mapEntries.end(); it++) {
		node2 = it->second;

		// retrieve variables
		input.execID = GetValue(node2, "execID");
		input.outputID = GetValue(node2, "outputID");
		input.instanceID = GetValue(node2, "instanceID");

		// add input to the execution
		exec.inputs.insert(pair<string, ParamsExecInput>(node2->name, input));
	}


	// retrieve list of outputs
	node = RetrieveNode(ID, "outputs");	

	// check if node is a map
	//if (node->IsMap() == false)
	//	ReportError("Invalid node type for 'outputs', must be a map!");

	// retrieve each output
	for (it = node->mapEntries.begin(); it != node->mapEntries.end(); it++) {
		node2 = it->second;

		// retrieve variables
		output.moduleID = GetValue(node2, "moduleID");
		output.dataType = GetValue(node2, "dataType");

		// add input to the execution
		exec.outputs.insert(pair<string, ParamsExecOutput>(node2->name, output));
	}

	return exec;
}










/********
 * class SSFValueArg
 ********/
SSFValueArg::SSFValueArg(string name, string desc, bool req, string *ptr) {

	this->name = name;
	this->desc = desc;
	this->req = req;
	this->ptr = (void *) ptr;
	this->type = "string";
	this->nvalues = 0;
}

SSFValueArg::SSFValueArg(string name, string desc, bool req, int *ptr) {

	this->name = name;
	this->desc = desc;
	this->req = req;
	this->ptr = (void *) ptr;
	this->type = "int";
	this->nvalues = 0;
}

SSFValueArg::SSFValueArg(string name, string desc, bool req, bool *ptr) {

	this->name = name;
	this->desc = desc;
	this->req = req;
	this->ptr = (void *) ptr;
	this->type = "bool";
	this->nvalues = 0;
}

SSFValueArg::SSFValueArg(string name, string desc, bool req, float *ptr) {

	this->name = name;
	this->desc = desc;
	this->req = req;
	this->ptr = (void *) ptr;
	this->type = "float";
	this->nvalues = 0;
}

SSFValueArg::SSFValueArg(string name, string desc, bool req, vector<string> *ptr) {

	this->name = name;
	this->desc = desc;
	this->req = req;
	this->ptr = (void *) ptr;
	this->type = "vectstring";
	this->nvalues = 0;
}


void SSFValueArg::SetValue(string value) {

	if (this->nvalues > 0 && type != "vectstring")
		ReportError("Value for '%s' is already set", this->name.c_str());

	if (type == "string")
		*((string *) ptr) = value;

	else if (type == "float")
		*((float *) ptr) = StringToFloat(value);

	else if (type == "bool")
		*((bool *) ptr) = StringToBool(value);

	else if (type == "int")
		*((int *) ptr) = StringToInt(value);

	else if (type == "vectstring")
		(*((vector<string> *) ptr)).push_back(value);

	else 
		ReportError("Invalid parameter type '%s'", type.c_str());

	this->nvalues++;
}


void SSFValueArg::SetVecValue(vector<string> value) {

	if (type != "vectstring")
		ReportError("Value for '%s' is already set", this->name.c_str());

	else if (type == "vectstring")
		(*((vector<string> *) ptr)) = value;

	this->nvalues = (int) value.size();
}


string SSFValueArg::GetValue() {

	if (this->nvalues > 1)
		ReportError("Invalid type - must be a vector");

	if (type == "string")
		return *((string *) ptr);

	else if (type == "float")
		return FloatToString(*((float *) ptr));

	else if (type == "bool")
		return BoolToString(*((bool *) ptr));

	else if (type == "int")
		return IntToString(*((int *) ptr));

	else 
		ReportError("Invalid parameter type '%s'", type.c_str());

	return "";
}


vector<string> SSFValueArg::GetVecValue() {

	if (type != "vectstring")
		ReportError("Invalid type - must be a single element");

	return (*((vector<string> *) ptr));
}



/********
 * class SSFInputParameters
 ********/
void SSFInputParameters::Add(string name, string *variable, string desc, bool req) {
SSFValueArg *arg;
	
	arg =  new SSFValueArg(name, desc, req, variable);

	args.insert(pair<string, SSFValueArg *>(name, arg));

}

void SSFInputParameters::Add(string name, int *variable, string desc, bool req) {
SSFValueArg *arg;
	
	arg =  new SSFValueArg(name, desc, req, variable);

	args.insert(pair<string, SSFValueArg *>(name, arg));

}

void SSFInputParameters::Add(string name, float *variable, string desc, bool req) {
SSFValueArg *arg;
	
	arg =  new SSFValueArg(name, desc, req, variable);

	args.insert(pair<string, SSFValueArg *>(name, arg));
}

void SSFInputParameters::Add(string name, bool *variable, string desc, bool req) {
SSFValueArg *arg;
	
	arg =  new SSFValueArg(name, desc, req, variable);

	args.insert(pair<string, SSFValueArg *>(name, arg));
}


void SSFInputParameters::Add(string name, vector<string> *variable, string desc, bool req) {
SSFValueArg *arg;
	
	arg =  new SSFValueArg(name, desc, req, variable);

	args.insert(pair<string, SSFValueArg *>(name, arg));
}


void SSFInputParameters::SetStructure(SSFStructure *structure, string name, string *variable, string desc, bool req) {
unordered_map<SSFStructure *, string>::iterator it;
SSFValueArg *arg;

	it = subStructures.find(structure);
	if (it != subStructures.end())
		ReportError("Structure already added in the parameters");

	// creat new arg
	arg =  new SSFValueArg(name, desc, req, variable);
	args.insert(pair<string, SSFValueArg *>(name, arg));

	// insert to the SSFStructure map
	subStructures.insert(pair<SSFStructure *, string>(structure, name));
}


void SSFInputParameters::SetParameters(string ID, SSFParameters *params) {
map<string, vector<string> > retParams;
map<string, vector<string> >::iterator it;
unordered_map<string, SSFValueArg *>::iterator it2;
string value;
size_t i;

	retParams = params->GetParameters(ID);

	// retrieve parameter values
	for (it = retParams.begin(); it != retParams.end(); ++it) {
		// retrieve parameter name
		it2 = args.find(it->first);
		if (it2 == args.end())
			ReportError("Parameter '%s' does not exist in setup '%s'", it->first.c_str(), ID.c_str());
	
		// set values
		for (i = 0; i < it->second.size(); i++)
			it2->second->SetValue(it->second[i]);
	}

	// check if all required parameters are set
	for (it2 = args.begin(); it2 != args.end(); ++it2) {
		if (it2->second->Required() == true && it2->second->GetNValues() == 0)
			ReportError("Required parameter '%s' was not set in setup '%s'!", it2->first.c_str(), ID.c_str());
	}

	// perform setup in all sub-structures
	SetParametersStructures(params);
}

void SSFInputParameters::PrintHelp(string modName) {
unordered_map<string, SSFValueArg *>::iterator it;

	printf("\n\n  Parameter\n      %s (%s)\n\n  Options\n", modName.c_str(), "desc");
	
	//for (it = args.begin(); it != args.end(); ++it) {
//		printf("      %s : %s%s[%s]\n", it->first.c_str(), it->second->GetDesc().c_str(), it->second->Required() ? " (required) " : " ", it->second->GetValue().c_str());
	//}
	printf("\n");
}



string SSFInputParameters::ReturnAllParameters() {
unordered_map<string, SSFValueArg *>::iterator it;
set<string>::iterator itSet;
set<string> keys;
SSFValueArg *arg;
vector<string> vecStr;
string keyStr;
string returnStr = "";
size_t i;

	// sort keys
	for (it = args.begin(); it != args.end(); it++) {
		keys.insert(it->first);
	}

	// for all keys, retrieve their values
	for (itSet = keys.begin(); itSet != keys.end(); itSet++) {
		it = args.find(*itSet);
		if (it == args.end())
			ReportError("That is impossible!");
		arg = it->second;

		// parameter name
		returnStr += "::" + *itSet;

		// if it is a vector, print all 
		if (arg->GetType() == "vectstring") {
			vecStr = arg->GetVecValue();
			for (i = 0; i < vecStr.size(); i++) 
				returnStr += ":" + vecStr[i];
		}
		// single value
		else {
			returnStr += ":" + arg->GetValue();
		}
	}

	return returnStr;
}




void SSFInputParameters::SaveParameters(SSFStorage &storage) {
unordered_map<string, SSFValueArg *>::iterator it;
set<string>::iterator itSet;
set<string> keys;
SSFValueArg *arg;
vector<string> vecStr;
string keyStr;
string returnStr = "";
size_t i;

	if (storage.isOpened() == false)
		ReportError("Invalid file storage!");

	//cvWriteComment( storage.fs, "List of parameters for the current SSFMethod", 0);
	storage << "parameters" << "{";
	

	// sort keys
	for (it = args.begin(); it != args.end(); it++) {
		keys.insert(it->first);
	}

	// for all keys, retrieve their values
	for (itSet = keys.begin(); itSet != keys.end(); itSet++) {
		it = args.find(*itSet);
		if (it == args.end())
			ReportError("That is impossible!");
		arg = it->second;

		// if it is a vector, print all
		if (arg->GetType() == "vectstring") {
			vecStr = arg->GetVecValue();
			storage << *itSet << "[";
			//cvWriteComment( storage.fs, arg->GetDesc().c_str(), 1);
			for (i = 0; i < vecStr.size(); i++) 
				storage << vecStr[i];
			storage << "]";
		}
		// single value
		else {
			storage << *itSet << arg->GetValue();
			//cvWriteComment( storage.fs, arg->GetDesc().c_str(), 1);
		}
	}

	storage << "}";
}


#if 0
void SSFInputParameters::SetParameters(const FileNode &node, SSFStorage &storage, string ID) {
unordered_map<string, SSFValueArg *>::iterator it2;
unordered_map<string, SSFValueArg *>::iterator it;
FileNodeIterator itInput, itVector;
string name, value;
FileNode n, nvector;
vector<string> vectorInput;

	n = node["parameters"];

	for (itInput = n.begin(); itInput != n.end(); itInput++) {
		name = (*itInput).name();

		// check if this is a valid parameter
		it = args.find(name);
		if (it == args.end())
			ReportError("Invalid parameter '%s' in setup '%s'!", name.c_str(), ID.c_str());

		// sequence of values
		if ((*itInput).type() == FileNode::SEQ) {
			nvector = n[name];
			vectorInput.clear();
			for (itVector = nvector.begin(); itVector != nvector.end(); itVector++) {
				value = (string)(*itVector);
				vectorInput.push_back(value);
			}
			it->second->SetVecValue(vectorInput);
		}

		// single values
		else if ((*itInput).type() == FileNode::STRING) {
			value = (string)(*itInput);
			if (value == "")
				continue;
			it->second->SetValue(value);
		}

		else if ((*itInput).type() == FileNode::INT) {
			value = aToString((int)(*itInput));
			it->second->SetValue(value);
		}

		else if ((*itInput).type() == FileNode::FLOAT) {
			value = FloatToString((float)(*itInput));
			it->second->SetValue(value);
		}

		else {
			ReportError("Parameter '%s' with invalid value type (%d) in setup '%s'!", name.c_str(), (*itInput).type(), ID.c_str());
		}
	}


	// check if all required parameters are set
	for (it2 = args.begin(); it2 != args.end(); ++it2) {
		if (it2->second->Required() == true && it2->second->GetNValues() == 0)
			ReportError("Required parameter '%s' was not set in setup '%s'!", it2->first.c_str(), ID.c_str());
	}
}
#endif


void SSFInputParameters::SetParametersStructures(SSFParameters *params) {
unordered_map<SSFStructure *, string>::iterator itStruct;
unordered_map<string, SSFValueArg *>::iterator itNames;
SSFValueArg *valueArg;
SSFStructure *structure;
string setupID, variable;

	for (itStruct = subStructures.begin(); itStruct != subStructures.end(); itStruct++) {

		// set info
		structure = itStruct->first;
		variable = itStruct->second;

		ReportStatus("Performing setup for setupID in variable'%s'", variable.c_str());

		// search for arguments of variable 
		itNames = args.find(variable);

		if (itNames == args.end())
			ReportError("Variable '%s' was not found in the parameters", variable.c_str());

		// retrieve setupID from the variable 
		valueArg = itNames->second;
		setupID = valueArg->GetValue();

		if (setupID == "")
			ReportError("Variable '%s' has not been set!", variable.c_str());

		// perform actual setup in the current structure
		structure->Setup(params, setupID);
	}
}