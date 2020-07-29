#include "SSFDict.h"

SSFDict::SSFDict() {

	this->SetDataType(SSF_DATA_DICT);
}

SSFDict::~SSFDict() {
	Dict.release();
}

void SSFDict::BuildDictionary(SSFMatrix<FeatType> BigMatrix, int ncws) {

	nCWs = ncws;
	vector<size_t> index;	
	vector<size_t>::iterator it;

	SSFMatrix<FeatType> Aux(nCWs, BigMatrix.cols);

	// Generate a random permutation of BigMatrix.
	index = GenerateRandomPermutation((size_t)BigMatrix.rows, (size_t)nCWs);
	
	// Creates the dictionary
	for(int i=0; i<nCWs; i++)
	{
		BigMatrix.row((int)index[i]).copyTo(Aux.row(i));
	}
	Dict = Aux;
}

SSFDict *SSFDict::Clone() {
	
	SSFDict* dict = new SSFDict();

	dict->Dict = Dict;
	dict->nCWs = nCWs;

	return dict;
}

double SSFDict::Distance(SSFMatrix<FeatType> dict, SSFMatrix<FeatType> feat) {
	
	// Calculate distance
	double dist = 0;

	for(int i=0; i<dict.cols; i++) {

		dist = dist +  pow(dict.at<FeatType>(0,i) - feat.at<FeatType>(0,i), 2);
	}
	return (dist/(dict.cols));
}

double SSFDict::DistanceMahalanobis(SSFMatrix<FeatType> dict, SSFMatrix<FeatType> feat, SSFMatrix<FeatType> invcovar) {
	
	// Calculate distance
	double dist = 0;

	//cout<<"feat"<<feat<<endl;
	//cout<<"and"<<endl;
	//cout<<"dict"<<dict<<endl;
	//cout<<"is: "<<endl;
	
	dist = Mahalanobis(dict, feat, invcovar);
	//cout<<"dist"<<dist<<endl;
	return dist;
}


int SSFDict::ComputeDistance(SSFMatrix<FeatType> feat) {

	double distaux;
	double dist = 99999999999999999999.0;
	int idx;
	int nfeats = Dict.cols;
	SSFMatrix<FeatType> aux, mean, covar, invcovar;

	if(feat.cols != Dict.cols) {
		ReportError("Size doesn't match!\n");
	}

	//// Computes the inverse covariance matrix
	//aux.push_back(feat);
	//aux.push_back(Dict);

	//calcCovarMatrix(aux, covar, mean, CV_COVAR_NORMAL+CV_COVAR_ROWS, -1);
	//covar = covar/(aux.rows-1);
	////cout<<"covar matrix:\n"<<covar<<endl<<endl<<endl;

	//invert(covar, invcovar, DECOMP_SVD);
	////cout<<"inverse covar matrix:\n"<<invcovar<<endl<<endl;

	for(int i=0; i<Dict.rows; i++) {
		distaux = Distance(Dict.row(i), feat);
		//distaux = DistanceMahalanobis(Dict.row(i), feat, invcovar);

		if(distaux <= dist) {
			idx = i;
			dist = distaux;
		}
	}
	return idx;
}

int SSFDict::ComputeDistance(SSFMatrix<FeatType> feat, double &Dist) {

	double distaux;
	long double dist = 99999999999999999999.0;
	int idx;
	int nfeats = Dict.cols;
	SSFMatrix<FeatType> aux, mean, covar, invcovar;

	if(feat.cols != Dict.cols) {
		ReportError("Size doesn't match!\n");
	}

	//// Computes the inverse covariance matrix
	//aux.push_back(feat);
	//aux.push_back(Dict);

	//calcCovarMatrix(aux, covar, mean, CV_COVAR_NORMAL+CV_COVAR_ROWS, -1);
	//covar = covar/(aux.rows-1);
	////cout<<"covar matrix:\n"<<covar<<endl<<endl<<endl;

	//invert(covar, invcovar, DECOMP_SVD);
	////cout<<"inverse covar matrix:\n"<<invcovar<<endl<<endl;

	for(int i=0; i<Dict.rows; i++) {
		distaux = Distance(Dict.row(i), feat);
		//distaux = DistanceMahalanobis(Dict.row(i), feat, invcovar);

		if(distaux <= dist) {
			idx = i;
			dist = distaux;
		}
	}
	Dist = dist;
	return idx;
}

void SSFDict::Load(const FileNode &node){
	
	FileNode n;

	n = node["Dict"];
	n["nCWs"] >> nCWs;
	n["dict"] >> Dict;
}

void SSFDict::Save(SSFStorage &storage) {

if (storage.isOpened() == false)
		ReportError("Invalid file storage!");
	
	storage << "Dict" << "{";
	storage << "nCWs" << nCWs;
	storage << "dict" << Dict;
	storage << "}";
}

void SSFDict::Load(string filename){
	
	SSFStorage storage;

	storage.open(filename, SSF_STORAGE_READ);
	this->Load(storage.root());
	storage.release();
}

void SSFDict::Save(string filename) {
SSFStorage storage;

	storage.open(filename, SSF_STORAGE_WRITE);
	this->Save(storage);
	storage.release();
}
