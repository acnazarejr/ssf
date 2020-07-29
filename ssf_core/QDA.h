#ifndef _CLASSIFIER_QDA_
#define _CLASSIFIER_QDA_
#include "headers.h"
#include "ClassificationDefs.h"
#include "ClassificationMethod.h"



class QDA : public ClassificationMethod {
FeatType logPriori;     // logarithm of priori probability
FeatType logDetSigma1; 
FeatType logDetSigma2;
SSFMatrix<FeatType> Gmeans, Gmean1, Gmean2, invR1, invR2;


public:
	QDA(DataStartClassification dataStart);

	// Execute setup for the method
	void Setup(AllParameters *params) { ; }

	// Create a new instance of this method
	QDA *Instantiate(DataStartClassification dataStart);
	
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
	string GetName() { return CLASSIFIER_ID_QDA; }
};


#endif
