#include "headers.h"
#include "structures.h"
#include "SSFFeatures.h"


/****
 * class SSFFeatures
 ****/
SSFFeatures::SSFFeatures() {
	
	lastRow = -1;
	dataM.create(0, 0);
}


SSFFeatures::~SSFFeatures() {

	dataM.release();
}


void SSFFeatures::AddFeatures(SSFMatrix<FeatType> &m) {

	// check if the number of columns is the same, if not, create a new matrix with new # of m.cols
	if ((lastRow == -1) && (m.cols != dataM.cols)) { 
		m.copyTo(dataM);
		lastRow = m.rows - 1;
	}

	// inconsistent number of cols
	else if ((lastRow >= 0) && (m.cols != dataM.cols)) { 
		ReportError("The number of columns must be the same");
	}

	// set features to dataM when there is memory allocated
	else if (dataM.rows - lastRow - 1 > m.rows) {
		m.copyTo(dataM.rowRange(lastRow + 1, lastRow + m.rows + 1));
		lastRow += m.rows;
	}

	// set features to dataM when there is no enough memory
	else {
		SSFMatrix<FeatType> tmp;
		dataM.resize(lastRow + m.rows + lastRow + 2);
		tmp = dataM.rowRange(lastRow + 1, lastRow + m.rows + 1);
		m.copyTo(tmp);
		lastRow += m.rows;
	}
}




void SSFFeatures::Reset() { 

	lastRow = -1; 
}


SSFMatrix<FeatType> SSFFeatures::RetrieveFeatures(bool singleRow) {

	if (singleRow == true)
		return dataM.rowRange(0, lastRow + 1).reshape(0, 1);

	return dataM.rowRange(0, lastRow + 1);
}



void SSFFeatures::Save(SSFStorage &storage) { 

	if (storage.isOpened() == false)
		ReportError("Invalid file storage!");

	storage << "SSFFeatures" << dataM;
}



void SSFFeatures::Load(const FileNode &node) { 

	node["SSFFeatures"] >> dataM;
}



SSFFeatures *SSFFeatures::Copy() {
SSFFeatures *feat;

	feat = new SSFFeatures();
	feat->AddFeatures(RetrieveFeatures(false));

	return feat;
}


SMIndexType SSFFeatures::GetNumberBytes() {

	return (SMIndexType) (dataM.total() * dataM.elemSize());
}



/* 
 * SSFMultipleFeatures
 */
SSFMultipleFeatures::SSFMultipleFeatures() {
SSFFeatures *feat;

	methodsExtracted = 0;
	feat = new SSFFeatures();
	feats.push_back(feat);
	featMap.insert(pair<size_t, size_t>(0,0));
}


SSFMultipleFeatures::~SSFMultipleFeatures() {
SSFFeatures *feat;
size_t i;

	for (i = 0; i < feats.size(); i++) {
		feat = feats[i];
		delete feat;
	}
	feats.clear();
}


SSFFeatures *SSFMultipleFeatures::RetrieveSingleFeatureExtraction(size_t featID) {
SSFFeatures *feat;
size_t idx;

	idx = this->RetrieveFeatureIndex(featID);

	mutex.Lock();
	feat = feats[idx];
	mutex.Unlock();

	return feat;
}


void SSFMultipleFeatures::AddFeatures(SSFMatrix<FeatType> &m, size_t position) {

	mutex.Lock();
	if (position >= feats.size()) {
		ReportError("Invalid feature index '%d'", position);
		mutex.Unlock();
	}
	feats[position]->AddFeatures(m);

	mutex.Unlock();
}


void SSFMultipleFeatures::Reset(size_t position) {

	mutex.Lock();
	if (position >= feats.size()) {
		ReportError("Invalid feature index '%d'", position);
		mutex.Unlock();
	}
	feats[position]->Reset();

	mutex.Unlock();
}


void SSFMultipleFeatures::ResetAll() {
size_t i;

	mutex.Lock();
	methodsExtracted = 0;
	for (i = 0; i < feats.size(); i++)
		feats[i]->Reset();
	mutex.Unlock();
}

SSFMatrix<FeatType> SSFMultipleFeatures::RetrieveFeatures(bool singleRow, size_t position) {
SSFMatrix<FeatType> m;

	mutex.Lock();
	if (position >= feats.size()) {
		ReportError("Invalid feature index '%d'", position);
		mutex.Unlock();
	}

	m = feats[position]->RetrieveFeatures(singleRow);
	mutex.Unlock();

	return m;
}

void SSFMultipleFeatures::RetrieveAllFeatures(SSFMatrix<FeatType> &m) {
SSFMatrix<FeatType> aux;
SMIndexType nvars;
size_t i;
int idx = 0;

	nvars = this->GetNumberBytes() / (SMIndexType) m.elemSize();
	m.create(1, (int) nvars);

	mutex.Lock();
	for (i = 0; i < feats.size(); i++) {
		aux = feats[i]->RetrieveFeatures(true);
		aux.copyTo(m.colRange(idx, idx + aux.cols - 1));
		idx += aux.cols;
	}
	mutex.Unlock();
}



void SSFMultipleFeatures::Save(SSFStorage &storage) {

}


void SSFMultipleFeatures::Load(const FileNode &node) {

}



size_t SSFMultipleFeatures::RetrieveFeatureIndex(size_t featID) {
unordered_map<size_t, size_t>::iterator it;
SSFFeatures *feat;
size_t index;

	mutex.Lock();
	it = featMap.find(featID);

	// still not available
	if (it == featMap.end()) {
		// locate the position for the new feature
		index = feats.size();

		// allocate new feature
		feat = new SSFFeatures();
		feats.push_back(feat);

		// insert entry to the map
		featMap.insert(pair<size_t, size_t>(featID, index));
	}

	// already available
	else {
		index = it->second;
	}
	mutex.Unlock();

	return index;
}


SSFMultipleFeatures *SSFMultipleFeatures::Copy() {
SSFMultipleFeatures *newfeats;
SSFFeatures *feat;
size_t i;

	mutex.Lock();
	// allocate memory
	newfeats = new SSFMultipleFeatures();
	
	// copy feature map
	newfeats->featMap = this->featMap;

	// copy every available feature
	for (i = 0; i < feats.size(); i++) {
		feat = feats[i]->Copy();
		newfeats->feats.push_back(feat);
	}
	mutex.Unlock();

	return newfeats;
}



SMIndexType SSFMultipleFeatures::GetNumberBytes() {
SMIndexType nbytes = 0;
size_t i;

	mutex.Lock();
	for (i = 0; i < feats.size(); i++) {
		nbytes += feats[i]->GetNumberBytes();
	}
	mutex.Unlock();

	return nbytes;
}


int SSFMultipleFeatures::NumberExtractionMethods() {
int n;

	mutex.Lock();
	n = this->methodsExtracted;
	mutex.Unlock();

	return n;
}


void SSFMultipleFeatures::SetExecutedMethod() {

	mutex.Lock();
	this->methodsExtracted++;
	mutex.Unlock();
}