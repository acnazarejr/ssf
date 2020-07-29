#include "headers.h"
#include "SSFParameters.h"
#include "parameters.h"




vector<string> GetNameList(string str) {
vector<string> strs;
string tmp;
int idx;

	idx = (int) str.find(',');
	while (idx != -1) {
		tmp = str.substr(0,idx);
		strs.push_back(tmp);
		str.erase(0, idx + 1);
		idx = (int) str.find(',');
	}
	strs.push_back(str);

	return strs;
} 



void Parser::FillBackSlash(string &str) {
int idx;
int previousidx;
	
	idx = (int) str.find('\\');
	while (idx != -1) {
		str.insert(idx+1, "\\");
		previousidx = idx + 2;
		idx = (int) str.find('\\', previousidx);
	}
}


void Parser::RemoveWhiteSpace(string &str) {
int idx;
	
	idx = (int) str.find(' ');
	while (idx != -1) {
		str.erase(idx, 1);
		idx = (int) str.find(' ');
	}
}


void Parser::RemoveQuotes(string &str) {
int idx;
	
	idx = (int) str.find('"');
	str.erase(0, idx + 1);

	idx = (int) str.find('"');
	if (idx == -1)
		ReportError("Quote was not closed in '%s'", str.c_str());

	str.erase(idx);
}


vector<string> Parser::ExtractLineTokens(string strline, int linenumber) {
vector<string> vtokens;	
string tmpstr, paramstr;
int idx1, idx2, idx, previdx; 
int idq1;

	// comment, ignore line
	if (strline[0] == '#' || strline.size() == 0 || strline[0] == '\n' || strline[0] == '\r')
		return vtokens;

	// command syntax: command <param1, param2,...>
	// find the command, 
	idx1 = (int) strline.find('<');
	idx2 = (int) strline.find('>');

	
	// syntax error
	if (idx1 == -1 || idx2 == -1) {
		ReportWarning("White line: %d\n", linenumber);
		return vtokens;
	}


	// command
	tmpstr = strline.substr(0, idx1 - 1);
	idq1 = (int) tmpstr.find('"');
	if (idq1 >= 0)
		RemoveQuotes(tmpstr);
	else
		RemoveWhiteSpace(tmpstr);
	
	vtokens.push_back(tmpstr);

	// read parameters
	paramstr = strline.substr(idx1 + 1, idx2 - idx1 - 1);

	// read parameters sequentially
	idx = (int) paramstr.find(',');
	previdx = 0;
	while (idx != -1) {
		tmpstr = paramstr.substr(previdx, idx - previdx);
		idq1 = (int) tmpstr.find('"');
		if (idq1 >= 0)
			RemoveQuotes(tmpstr);
		else
			RemoveWhiteSpace(tmpstr);
		vtokens.push_back(tmpstr);
		previdx = idx + 1;
		idx = (int) paramstr.find(',', previdx);
	}
	// last token
	if ((int) paramstr.size() != 0) {
		tmpstr = paramstr.substr(previdx);
		idq1 = (int) tmpstr.find('"');
		if (idq1 >= 0)
			RemoveQuotes(tmpstr);
		else
			RemoveWhiteSpace(tmpstr);
		vtokens.push_back(tmpstr);
	}

	return vtokens;
}



void Parser::CheckNumberOfParams(int linenumber, string cmd, int current, int correct, string filename) {

	if (current != correct) 
		ReportError("[%s:%d] Incorrect number of parameters for command '%s'", filename.c_str(), linenumber, cmd.c_str());
}






void Parser::ParseFile(string paramFile) {  //, char* lineparams[], int nparams) {
vector<string> tokens;
ifstream myfile;
int linenumber;
string line;

	if (paramFile.size() == 0)
		ReportError("Parameter file cannot be empty!");

	myfile.open (paramFile.c_str());

	if (!myfile.is_open())
		ReportError("Cannot open parameter file '%s'!", paramFile.c_str());

	linenumber = 1;
	while (myfile.good()) {
		getline(myfile, line);
		tokens = ExtractLineTokens(line, linenumber);
		AddCommand(linenumber, tokens, paramFile);
		linenumber++;		
	}

	myfile.close();
}




void Parser::ParseFileVector(vector<string> &lines) { 
vector<string> tokens;
int linenumber = 1;
int i;

	for (i = 0; i < (int) lines.size(); i++) {
		tokens = ExtractLineTokens(lines.at(i), -1);
		AddCommand(linenumber, tokens);
		linenumber++;
	}
}






//////////////////////////////////////////






/***
 * class AllParameters
 ***/
void AllParameters::AddCommand(int linenumber, vector<string> tokens, string filename) {
string cmd, tmpstr, firstparam;
int nparams;

	// no command
	if (tokens.size() == 0)
		return;

	// number of parameters
	nparams = (int) tokens.size() - 1;

	// get command
	cmd = GetToken(tokens, 0);


	// parameters for classification methods
	if (cmd.find("params") != -1) {
		// in this case at least 3 parameters are required
		if (nparams < 3)
			ReportError("Incorrect number of parameters in '%s:%d'", filename.c_str(), linenumber);

		firstparam = GetToken(tokens, 1);

		// according to the type of the first parameters, either add into the map of call further action
		// parameter for features
		if (firstparam == "feature") {
			tokens.erase(tokens.begin(), tokens.begin() + 2);
			featParams.SetParameter(linenumber, tokens, filename);
		}

		// remaining parameters - must have exactly 3 parameters <key, name, value>
		else {
			if (nparams != 3) {
				ReportError("Incorrect number of parameters in '%s:%d', must be 3 <key, name, value>", 
					filename.c_str(), linenumber);
			}

			map<string, multimap<string, string> >::iterator it;
			string arg2, arg3, arg4;

			// key of the parameter
			tmpstr = GetToken(tokens, 1);
			arg2 = GetToken(tokens, 2); // param
			arg3 = GetToken(tokens, 3); // value

			// search if the key already exist
			it = params.find(tmpstr);

			// if does not exist create a new
			if (it != params.end()) {
				(*it).second.insert(pair<string, string>(arg2, arg3));
			}
			else {
				multimap<string, string> paramMap;
				paramMap.insert(pair<string, string>(arg2, arg3));
				params.insert(pair<string, multimap<string, string> >(tmpstr, paramMap));
			}
		}
	}
}


string AllParameters::RetrieveModuleType(string id) {
map<string, string>::iterator it;

	it = typeModules.find(id);

	if (it == typeModules.end())
		ReportError("Module '%s' does not exist in the config file", id.c_str());

	return it->second;
}

#if 0
map<string, string> AllParameters::GetParams(string paramKey) { 
map<string, map<string, string> >::iterator it;
map<string, string> m;

	it = params.find(paramKey);

	if (it != params.end()) {
		return it->second;
	}
	
	return m;
};
#endif

#if 0
Parameters *AllParameters::RetrieveParameters() {
Parameters *params;

	params = new Parameters();

	params->params = this->params;

	return params;
}
#endif


/*
 * FeatureParams
 */
void FeatureParams::SetParameter(int linenumber, vector<string> tokens, string filename) {
Parser parser;
string cmd, tmpstr;
int nparams;
FeatBlockInfo blocksize;

	// number of parameters
	nparams = (int) tokens.size() - 1;

	// get command
	cmd = GetToken(tokens, 0);

	// parameters for classification methods
	if (cmd.find("setup") != -1) {
		// <..., setup, ID>
		CheckNumberOfParams(linenumber, cmd, nparams, 1, filename);
		setups.insert(GetToken(tokens, 1));
	}

	// block for a feature extraction method
	else if (cmd.find("block") != -1) {
		// block type <block, block ID, w, h, shiftw, shifth>
		if (nparams == 5) {
			tmpstr = GetToken(tokens, 1); // feauture method ID
			blocksize.block.x0 = 0;
			blocksize.block.y0 = 0;
			blocksize.block.w = atoi(GetToken(tokens, 2).c_str());
			blocksize.block.h = atoi(GetToken(tokens, 3).c_str());
			blocksize.strideX = atoi(GetToken(tokens, 4).c_str());
			blocksize.strideY = atoi(GetToken(tokens, 5).c_str());

			map<string, vector<FeatBlockInfo> >::iterator it;
			it = Blocks.find(tmpstr);
			if (it != Blocks.end()) {
				(*it).second.push_back(blocksize);
			}
			else {
				vector<FeatBlockInfo> blkList;
				blkList.push_back(blocksize);
				Blocks.insert(pair<string, vector<FeatBlockInfo> >(tmpstr, blkList)); 
			}
		}

		else { // incorrect number of parameters
			CheckNumberOfParams(linenumber, cmd, nparams, 10, filename);
		}
	}

	else {
		ReportError("Invalid command '%s' in '%s:%d'", cmd.c_str(), filename.c_str(), linenumber);
	}
}



vector<string> FeatureParams::GetSetups() {
vector<string> vids;
set<string>::iterator it;

	for (it = setups.begin(); it != setups.end(); it++) 
		vids.push_back(*it);

	return vids;
}


bool FeatureParams::CheckAvailableSetup(string setupID) {
set<string>::iterator it;

	it = setups.find(setupID);

	if (it == setups.end())
		return false;

	return true;
}


vector<FeatBlockInfo> FeatureParams::GetBlocks(string featName) { 
map<string, vector<FeatBlockInfo> >::iterator it;
vector<FeatBlockInfo> v;

	it = Blocks.find(featName);

	if (it != Blocks.end())
		return it->second;

	return v;
}








/****
 * class Parameters
 ****/
#if 0
void Parameters::AddCommand(int linenumber, vector<string> tokens, string filename) {
string cmd, tmpstr, firstparam;
int nparams;

	// no command
	if (tokens.size() == 0)
		return;

	// number of parameters
	nparams = (int) tokens.size() - 1;

	// get command
	cmd = GetToken(tokens, 0);

	// parameters for classification methods
	if (cmd.find("params") != -1) {
		// in this case at least 3 parameters are required
		if (nparams != 3) {
			ReportError("Incorrect number of parameters in '%s:%d', must be 3 <key, name, value>", 
				filename.c_str(), linenumber);
		}

		map<string, map<string, string> >::iterator it;

		// key of the parameter
		tmpstr = GetToken(tokens, 1);	
	
		// search if the key already exist
		it = params.find(tmpstr);

		// if does not exist create a new
		if (it != params.end()) {
			(*it).second.insert(pair<string, string>(GetToken(tokens, 2), GetToken(tokens, 3)));
		}
		else {
			map<string, string> paramMap;
			paramMap.insert(pair<string, string>(GetToken(tokens, 2), GetToken(tokens, 3)));
			params.insert(pair<string, map<string, string> >(tmpstr, paramMap));
		}
	}
}
#endif


multimap<string, string> Parameters::GetParams(string paramKey) { 
map<string, multimap<string, string> >::iterator it;
multimap<string, string> m;

	it = params.find(paramKey);

	if (it != params.end()) {
		return it->second;
	}
	
	return m;
};


string Parameters::GetParams(string paramKey, string name, bool returnError) {
map<string, string>::iterator it;
multimap<string, string> m;

	m =	GetParams(paramKey);
	it = m.find(name);

	if (it == m.end()) {
		if (returnError == true) {
			ReportError("Parameter '%s' was not found in key '%s'", name.c_str(), paramKey.c_str());
		}
		else {
			return "";
		}
	}

	return it->second;
}





/********
 * class ValueArg
 ********/
ValueArg::ValueArg(string name, string desc, bool req, string *ptr) {

	this->name = name;
	this->desc = desc;
	this->req = req;
	this->ptr = (void *) ptr;
	this->type = "string";
	this->nvalues = 0;
}

ValueArg::ValueArg(string name, string desc, bool req, int *ptr) {

	this->name = name;
	this->desc = desc;
	this->req = req;
	this->ptr = (void *) ptr;
	this->type = "int";
	this->nvalues = 0;
}

ValueArg::ValueArg(string name, string desc, bool req, bool *ptr) {

	this->name = name;
	this->desc = desc;
	this->req = req;
	this->ptr = (void *) ptr;
	this->type = "bool";
	this->nvalues = 0;
}

ValueArg::ValueArg(string name, string desc, bool req, float *ptr) {

	this->name = name;
	this->desc = desc;
	this->req = req;
	this->ptr = (void *) ptr;
	this->type = "float";
	this->nvalues = 0;
}

ValueArg::ValueArg(string name, string desc, bool req, vector<string> *ptr) {

	this->name = name;
	this->desc = desc;
	this->req = req;
	this->ptr = (void *) ptr;
	this->type = "vectstring";
	this->nvalues = 0;
}


void ValueArg::SetValue(string value) {

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


void ValueArg::SetVecValue(vector<string> value) {

	if (type != "vectstring")
		ReportError("Value for '%s' is already set", this->name.c_str());

	else if (type == "vectstring")
		(*((vector<string> *) ptr)) = value;

	this->nvalues = (int) value.size();
}


string ValueArg::GetValue() {

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


vector<string> ValueArg::GetVecValue() {

	if (type != "vectstring")
		ReportError("Invalid type - must be a single element");

	return (*((vector<string> *) ptr));
}



/********
 * class InputParameters
 ********/
void InputParameters::Add(string name, string *variable, string desc, bool req) {
ValueArg *arg;
	
	arg =  new ValueArg(name, desc, req, variable);

	args.insert(pair<string, ValueArg *>(name, arg));

}

void InputParameters::Add(string name, int *variable, string desc, bool req) {
ValueArg *arg;
	
	arg =  new ValueArg(name, desc, req, variable);

	args.insert(pair<string, ValueArg *>(name, arg));

}

void InputParameters::Add(string name, float *variable, string desc, bool req) {
ValueArg *arg;
	
	arg =  new ValueArg(name, desc, req, variable);

	args.insert(pair<string, ValueArg *>(name, arg));
}

void InputParameters::Add(string name, bool *variable, string desc, bool req) {
ValueArg *arg;
	
	arg =  new ValueArg(name, desc, req, variable);

	args.insert(pair<string, ValueArg *>(name, arg));
}


void InputParameters::Add(string name, vector<string> *variable, string desc, bool req) {
ValueArg *arg;
	
	arg =  new ValueArg(name, desc, req, variable);

	args.insert(pair<string, ValueArg *>(name, arg));
}

void InputParameters::SetParameters(string ID, AllParameters *params) {
multimap<string, string> retParams;
multimap<string, string>::iterator it;
unordered_map<string, ValueArg *>::iterator it2;
string value;

	retParams = params->GetParams(ID);

	// retrieve parameter values
	for (it = retParams.begin(); it != retParams.end(); ++it) {
		// retrieve parameter name
		it2 = args.find(it->first);
		if (it2 == args.end())
			ReportError("Parameter '%s' does not exist in setup '%s'", it->first.c_str(), ID.c_str());
	
		// set value
		it2->second->SetValue(it->second);
	}

	// check if all required parameters are set
	for (it2 = args.begin(); it2 != args.end(); ++it2) {
		if (it2->second->Required() == true && it2->second->GetNValues() == 0)
			ReportError("Required parameter '%s' was not set in setup '%s'!", it2->first.c_str(), ID.c_str());
	}
}


void InputParameters::SetParameters(string ID, SSFParameters *params) {
map<string, vector<string> > retParams;
map<string, vector<string> >::iterator it;
unordered_map<string, ValueArg *>::iterator it2;
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
}

void InputParameters::PrintHelp(string modName) {
unordered_map<string, ValueArg *>::iterator it;

	printf("\n\n  Parameter\n      %s (%s)\n\n  Options\n", modName.c_str(), "desc");
	
	//for (it = args.begin(); it != args.end(); ++it) {
//		printf("      %s : %s%s[%s]\n", it->first.c_str(), it->second->GetDesc().c_str(), it->second->Required() ? " (required) " : " ", it->second->GetValue().c_str());
	//}
	printf("\n");
}



string InputParameters::ReturnAllParameters() {
unordered_map<string, ValueArg *>::iterator it;
set<string>::iterator itSet;
set<string> keys;
ValueArg *arg;
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




void InputParameters::SaveParameters(SSFStorage &storage) {
unordered_map<string, ValueArg *>::iterator it;
set<string>::iterator itSet;
set<string> keys;
ValueArg *arg;
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



void InputParameters::SetParameters(const FileNode &node, SSFStorage &storage, string ID) {
unordered_map<string, ValueArg *>::iterator it2;
unordered_map<string, ValueArg *>::iterator it;
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