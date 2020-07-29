#include "headers.h"
#include "SSFCore.h"
#include "SSFParameters.h"
#include "SSFExecutionSequence.h"


/****
 * Class SSFExecutionSequence
 ****/
SSFExecutionSequence::SSFExecutionSequence(ParamsExecution execParams, ParamsPipeline pipeParams, size_t entryID) {

	// execution information
	this->execParams = execParams;

	// pipeline information
	this->pipeParams = pipeParams;

	// set entry ID
	this->entryID = entryID;
}



SSFUserModule *SSFExecutionSequence::RetrieveMethod(string setupID) {
unordered_map<string, SSFUserModule *>::iterator it;

	it = methods.find(setupID);

	if (it == methods.end())
		ReportError("Method with setupID '%s' is not instantiated!", setupID.c_str());

	return it->second;
}


void SSFExecutionSequence::InstanciateModules(SSFParameters *parameters, SSFMethodControl *ssfMethodControl) {
string setupID, modType, modName, modID;
SSFMethod *method;
vector<string> modSetupIDs;
string pipeID;
size_t i;

	// instantiate modules in the pipeline
	modSetupIDs = pipeParams.RetrieveModSetupIDs();
	for (i = 0; i < modSetupIDs.size(); i++) {
		setupID = modSetupIDs[i];

		// retrieve module information
		modType = parameters->GetValue(setupID, "type");
		modName = parameters->GetValue(setupID, "method");

		// create module ID
		modID =  execParams.execID + "_" + setupID;

		// instantiate module
		method = ssfMethodControl->InstanciateMethod(modName, modType, modID);

		// keep instantiated modules for this pipeline
		methods.insert(pair<string, SSFUserModule *>(setupID, (SSFUserModule *) method));

		// add translation to the map
		mapSetupIDtoModID.insert(pair<string, string>(setupID, modID));
	}

	// create mapping from output to instanceID
	this->CreateOutputMapping();
}


void SSFExecutionSequence::CreateOutputMapping() {
unordered_map<string, ParamsExecOutput>::iterator it;
map<string, string>::iterator it2;
ParamsExecOutput output;

	// add output to the map
	for (it = execParams.outputs.begin(); it != execParams.outputs.end(); it++) {
		output = it->second;

		// search the module actual instanceID
		it2 = mapSetupIDtoModID.find(output.moduleID);

		// invalid moduleID
		if (it2 == mapSetupIDtoModID.end())
			ReportError("Module '%s' was not found, verify setup '%s'", output.moduleID.c_str(), execParams.execID.c_str());

		mapOutputtoModID.insert(pair<string,string>(execParams.execID + "_" + it->first, it2->second));
	}
}



void SSFExecutionSequence::SetModulesDependencies() {
unordered_map<string, SSFUserModule *>::iterator it;
unordered_map<string, ParamsExecInput>::iterator it2;
vector<ParamsModuleInput> inputs;
string attribute, dataType;
ParamsModuleInput input;
SSFUserModule *method;
vector<SMDataKey> reqData;
SMDataKey *req;
size_t i, j;

	// for each module in the sequence, set its input
	for (it = methods.begin(); it != methods.end(); it++) {
		method = it->second;

		// retrieve inputs for this method
		inputs = pipeParams.RetrieveInputs(it->first);

		// retrieve required data type for this module
		reqData = method->modParams.GetRequiredDataType();

		// try to set each of the input data type for this module
		for (i = 0; i < reqData.size(); i++) {
			req = &(reqData[i]);
			attribute = req->GetAttribute();
			dataType = req->GetDataType();

			// search for an input with the correct attribute and dataType
			for (j = 0; j < inputs.size(); j++) {
				input = inputs[j];

				// check if this is the correct
				if (input.attribute == attribute && input.dataType == dataType) {

					// check if this input is a module within the pipeline or a execution sequence input
					it2 = execParams.inputs.find(input.moduleID);
			
					// it is an execution sequence input
					if (it2 != execParams.inputs.end()) {
						if (it2->second.instanceID != "")
							// from starting module
							req->SetModFromVariableValue(it2->second.instanceID);
						else
							// from an output from another pipeline
							req->SetModFromVariableValue(it2->second.execID + "_" + it2->second.outputID);
					}
					else
						// from an output of a module in the same pipeline
						if (input.inattribute == "")
							req->SetModFromVariableValue(execParams.execID + "_" + input.moduleID);
						else
							req->SetModFromVariableValue(execParams.execID + "_" + input.moduleID + "*:*" + input.inattribute);
				}
			}
		}
	}
}




void SSFExecutionSequence::SetupModules(SSFParameters *parameters) {
unordered_map<string, SSFUserModule *>::iterator it;
SSFUserModule *method;

	// setup for each method in the pipeline
	for (it = methods.begin(); it != methods.end(); it++) {
		method = it->second;
		method->Setup(parameters, it->first);
	}
}



vector<SSFUserModule *> SSFExecutionSequence::RetrieveModules() {
unordered_map<string, SSFUserModule *>::iterator it;
vector<SSFUserModule *> modules;
	
	// setup for each method in the pipeline
	for (it = methods.begin(); it != methods.end(); it++) {
		modules.push_back(it->second);
	}

	return modules;
}