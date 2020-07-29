#ifndef CLASSIFIER_PLS_OAA
#define CLASSIFIER_PLS_OAA
#include "headers.h"
#include "ClassificationDefs.h"
#include "ClassificationMethod.h"

class PLS;


class PLS_OAA : public ClassificationMethod {
int dimensions;								/* number of dimensions */
map<string, PLS *> plsmodels;				/* PLS models for each class */

public:
	PLS_OAA(DataStartClassification dataStart);
	~PLS_OAA();

	// Execute setup for the method
	void Setup(AllParameters *params);

	// Create a new instance of this method
	PLS_OAA *Instantiate(DataStartClassification dataStart);
	
	// Initialize the classifier
	void Initialize() { ; }

	// Classify samples and set responses.
	void Classify(const SSFMatrix<FeatType> &X, SSFMatrix<FeatType> &responses);

	// learn the classifier
	void Learn();

	// save classifier 
	void Save(SSFStorage &storage);

	// load classifier
	void Load(const FileNode &node, SSFStorage &storage);

	// return ID of the classification method 
	string GetName() { return CLASSIFIER_ID_PLS; }
};


#endif
