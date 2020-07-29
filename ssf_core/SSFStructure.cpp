#include "headers.h"
#include "SMDataKey.h"
#include "SSFCore.h"
#include "SSFStructure.h"



/****
 * Class SSFStructure
 ****/

SSFStructure::SSFStructure() {

	this->setupID = "";

}


SSFStructure::~SSFStructure() {

}



void SSFStructure::SetSetupID(string ID) {

	this->setupID = ID;
}


size_t SSFStructure::RetrieveSignature() {
hash<string> hash_fn;
string inputStr;

	inputStr = this->GetType();
	inputStr += "::" + this->GetName();
	inputStr += "::" + this->GetVersion();
	inputStr += inputParams.ReturnAllParameters();

	return hash_fn(inputStr);
}



void SSFStructure::Setup(SSFParameters *params, string setupID) {

	inputParams.SetParameters(setupID, params);

	// store setupID to save parameters
	this->SetSetupID(setupID);

	// call local setup
	this->Setup();

	// add this instance in the method control (if it is a SSFStructure)
	if (this->IsSSFMethod() == false)
		glb_SSFExecCtrl.SetInstantiatedStructure(this);
}


void SSFStructure::SaveParameters(SSFStorage &storage) {
FileNode node;

	if (storage.isOpened() == false)
		ReportError("Invalid file storage!");

	// check if setup ID has been stored already
	node = storage.root()[this->setupID];

	// setup ID has already been stored
	if (node.empty() == false)
		return;

	//cvWriteComment(storage.fs, "Setup ID", 0); // just a description
	storage << this->setupID << "{";

	storage << "version" << this->GetVersion();

	storage << "signature" <<  aToString(this->RetrieveSignature()); 

	storage << "name" << this->GetName();

	storage << "type" << this->GetType();

	inputParams.SaveParameters(storage); 
	storage << "}";
}