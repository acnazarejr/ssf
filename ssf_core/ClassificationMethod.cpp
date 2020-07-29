#include "headers.h"
#include "ClassificationMethod.h"


ClassificationMethod::ClassificationMethod(DataStartClassification dataStart) {

	this->ID = dataStart.ID;

	nsamples = 0;
}

ClassificationMethod::~ClassificationMethod() {


}


void ClassificationMethod::Save(string filename) {
SSFStorage storage;

	storage.open(filename, SSF_STORAGE_WRITE);
	this->Save(storage);
	storage.release();
}


void ClassificationMethod::Load(string filename) {
SSFStorage storage;

	storage.open(filename, SSF_STORAGE_READ);
	this->Load(storage.root(), storage);
	storage.release();
}


void ClassificationMethod::AddSamples(const SSFMatrix<FeatType> &X, string id) {
int i;

	dataX.push_back(X);
	nsamples += X.rows;

	for (i = 0; i < X.rows; i++) 
		dataY.push_back(id);

	labels.insert(id);
}


// add sample features to the classifier
void ClassificationMethod::AddSamples(const SSFMatrix<FeatType> &X, const vector<string> &Y) { 
int i;

	if (X.rows != (int) Y.size())
		ReportError("Inconsistent matrix sizes (number of rows in X and Y must be the same)");

	dataX.push_back(X);

	for (i = 0; i < (int) Y.size(); i++)
		dataY.push_back(Y[i]);

	nsamples += X.rows;
}

vector<string> ClassificationMethod::RetrieveClassIDs() {
set<string>::iterator it;
vector<string> ids;

	for (it = labels.begin(); it != labels.end(); it++) {
		ids.push_back(*it);
	}

	return ids;
}



void ClassificationMethod::Save_(SSFStorage &storage) {
set<string>::iterator it;

	storage << "ClassificationMethod" << "{";
	storage << "labels" << "[:";
	for (it = labels.begin(); it != labels.end(); it++) 
		storage << *it;
	storage <<  "]";
	storage << "}";
}

void ClassificationMethod::Load_(const FileNode &node) {
FileNode n;
vector<string> tmp;
size_t i;

	n = node["ClassificationMethod"];
	n["labels"] >> tmp;

	for (i = 0; i < tmp.size(); i++) 
		labels.insert(tmp[i]);
}