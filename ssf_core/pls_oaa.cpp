#include "headers.h"
#include "pls.h"
#include "ClassificationMethod.h"
#include "pls_oaa.h"



PLS_OAA::PLS_OAA(DataStartClassification dataStart) : ClassificationMethod(dataStart) {

	dimensions = 3;

	/* Add parameters */
	inputParams.Add("dimensions", &dimensions, "Number of the dimensions of the PLS space", true);
}



PLS_OAA::~PLS_OAA() {
map<string, PLS *>::iterator it;
PLS *pls;

	for (it = plsmodels.begin(); it != plsmodels.end(); it++) {
		pls = it->second;
		delete pls;
	}
}



// learn the classifier
void PLS_OAA::Learn() { 
set<string>::iterator it;
SSFMatrix<FeatType> Ylab;
PLS *pls;
int i;

	if (dataX.rows == 0)
		ReportError("Set data first");

	if (dimensions <= 0)
		ReportWarning("Inconsistent number of dimensions in the low dimensional space");

	// initialize label matrix
	Ylab.create(nsamples, 1);

	/* execute one-against-all training */
	for (it = labels.begin(); it != labels.end(); it++) {

		/* ignore if class label is -1 (negative extra samples) */
		if (*it == EXTRA_CLASS)
			continue;

		/* reset label vector (all set to negatives) */
		Ylab = -1.0f;

		/* set positive samples according to the sample IDs */
		for (i = 0; i < (int) dataY.size(); i++) {
			if (dataY[i] == *it)
				Ylab[i][0] = 1.0f;
		}
		
		/* learn model */
		pls = new PLS();
		pls->runpls(dataX, Ylab, dimensions); 

		/* store model */
		plsmodels.insert(pair<string, PLS *>(*it, pls));
	}
}





void PLS_OAA::Classify(const SSFMatrix<FeatType> &X, SSFMatrix<FeatType> &responses)  {
map<string, PLS *>::iterator it;
SSFMatrix<FeatType> maux;
int idx = 0;

	responses.create(X.rows, (int) plsmodels.size());

	for (it = plsmodels.begin(); it != plsmodels.end(); it++) {
		it->second->ProjectionBstar(X, maux);
		maux.copyTo(responses.col(idx++));
	}
}



void PLS_OAA::Setup(AllParameters *params) {

	/* Set input parameters */
	inputParams.SetParameters(this->GetID(), params);

	// retrieve values
	//modParams.GetValue("dimensions", dimensions);
}



void PLS_OAA::Save(SSFStorage &storage) {
int idx = 0;
map<string,PLS *>::iterator it;

	if (storage.isOpened() == false)
		ReportError("Invalid file storage!");

	storage << "PLS_OAA" << "{";
	Save_(storage);  // save parameters that are in the ClassificationMethod 
	storage << "nmodels" << (int) plsmodels.size();

	// print PLS model for each class
	for (it = plsmodels.begin(); it != plsmodels.end(); it++) {
		// new level
		storage << "model" + IntToString(idx++) << "{";

		// class ID
		storage << "label" << it->first;

		// save PLS model
		it->second->Save(storage);

		// return the level
		storage << "}";
	}
	storage << "}";
}



void PLS_OAA::Load(const FileNode &node, SSFStorage &storage) {
int nmodels, i;
string label;
vector<int> tmpsamples;
FileNode n, n2, n3;
PLS *pls;


	n = node["PLS_OAA"];
	n["nmodels"] >> nmodels;

	for (i = 0; i < nmodels; i++) {
		n2 = n["model" + IntToString(i)];

		// read the label
		n2["label"] >> label;
		labels.insert(label);

		// read the model
		pls = new PLS();

		// model label
		pls->Load(n2);
		plsmodels.insert(pair<string, PLS *>(label, pls));
	}

	Load_(n);   // load parameters that are in the ClassificationMethod 
}


PLS_OAA *PLS_OAA::Instantiate(DataStartClassification dataStart) {

	return new PLS_OAA(dataStart); 
}