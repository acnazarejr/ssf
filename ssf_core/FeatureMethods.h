#ifndef _FEATURE_METHODS_H_
#define _FEATURE_METHODS_H_
#include "headers.h"

class ExtractionMethod;


/*
 * Class to control and select a feature extraction methods using their names
 */
class FeatureMethods {
unordered_map<string, ExtractionMethod *> methods;		// list of available feature extraction methods

public:
	FeatureMethods();

	// instantiate a feature extraction method
	ExtractionMethod *InstanciateMethod(string featName);
};

#endif