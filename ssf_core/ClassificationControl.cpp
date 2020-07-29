#include "headers.h"
#include "ClassificationMethod.h"
#include "ClassificationControl.h"
#include "pls_oaa.h"



ClassificationControl::ClassificationControl() {
PLS_OAA *plsoaa;
DataStartClassification dataStart;

	dataStart.ID = "notInstantiated";

	// add classifiers that can be considered
	plsoaa = new PLS_OAA(dataStart);
	classifierList.insert(pair<string,ClassificationMethod *>(plsoaa->GetID(), plsoaa));
}




ClassificationControl::~ClassificationControl() {
map<string,ClassificationMethod *>::iterator it;


	// release memory for every classifier
	for (it = classifierList.begin() ; it != classifierList.end(); it++)
		delete (*it).second;

	classifierList.clear();
}




// return chosen classifier to the user
ClassificationMethod *ClassificationControl::InstantiateClassifier(string name, DataStartClassification dataStart) {
map<string,ClassificationMethod *>::iterator it;

	it = classifierList.find(name);
	if (it != classifierList.end()) 
		return it->second->Instantiate(dataStart);
	
	ReportError("Classifier '%s' not found", name.c_str());

	return NULL;
}