#ifndef CLASSIFICATION_CONTROL
#define CLASSIFICATION_CONTROL
#include "headers.h"

class ClassificationMethod;


class ClassificationControl {
map<string, ClassificationMethod *> classifierList;	// list of available classifiers

public:
	ClassificationControl();
	~ClassificationControl();

	// return chosen classifier to the user
	ClassificationMethod *InstantiateClassifier(string name, DataStartClassification dataStart);
};


#endif