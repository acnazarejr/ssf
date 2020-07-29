#ifndef CLASSIFICATION_METHOD
#define CLASSIFICATION_METHOD
#include "headers.h"
#include "parameters.h"

class AllParameters;

// class to perform classification of feature vectors.
class ClassificationMethod {
string ID;						// ID of the classification method

protected:
InputParameters inputParams;
int nsamples;					/* total number of samples */
SSFMatrix<FeatType> dataX;		/* data */
vector<string> dataY;			/* labels */
set<string> labels;				/* class labels */

	void Save_(SSFStorage &storage);

	void Load_(const FileNode &node);

public:
	ClassificationMethod(DataStartClassification dataStart);
	~ClassificationMethod();

	// load a classifier from a file
	void Load(string filename);

	// save a classifier to a file
	void Save(string filename);

	// retrieve class IDs (same order as the responses)
	virtual vector<string> RetrieveClassIDs();

	// Add training samples to the classifier.  it requires one classID per sample
	void AddSamples(const SSFMatrix<FeatType> &X, const vector<string> &Y);

	// Add training samples to the classifier. The id is set to all samples.
	void AddSamples(const SSFMatrix<FeatType> &X, string id);

	// Retrieve the ID for this module
	string GetID() { return this->ID; }


	/* 
	 * Functions to be implemented by the specific methods
	 */
	// Execute setup for the method
	virtual void Setup(AllParameters *params) {
		ReportError("Setup() has not been implemented for classifier '%s'!", GetID().c_str()); 
	}

	// Create a new instance of this method
	virtual ClassificationMethod *Instantiate(DataStartClassification dataStart) { 
		ReportError("Instantiate() has not been implemented for classifier '%s'!", GetID().c_str()); 
		return NULL;
	}
	
	// Initialize the classifier
	virtual void Initialize() { 
		ReportError("Initialize() has not been implemented for classifier '%s'!", GetID().c_str()); 
	}
		
	// Classify samples and set responses.
	virtual void Classify(const SSFMatrix<FeatType> &X, SSFMatrix<FeatType> &responses) { 
		ReportError("AddTrainingSamples() has not been implemented for classifier '%s'!", GetID().c_str());
	}
	
	// learn the classifier
	virtual void Learn() { 
		ReportError("Learn() has not been implemented for classifier '%s'!", GetID().c_str());
	}

	// save classifier 
	virtual void Save(SSFStorage &storage) { 
		ReportError("Save() has not been implemented for classifier '%s'!", GetID().c_str());
	}
	
	// load classifier
	virtual void Load(const FileNode &node, SSFStorage &storage) { 
		ReportError("Load() has not been implemented for classifier '%s'!", GetID().c_str());
	}
	
	// return ID of the classification method 
	virtual string GetName() { 
		ReportError("GetName() has not been implemented for classifier '%s'!", GetID().c_str());
		return ""; 
	}
};


#endif