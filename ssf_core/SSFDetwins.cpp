#include "headers.h"
#include "SSFDetwins.h"


SSFDetwins::SSFDetwins() {

	mutex.Lock();
	this->detwins = new vector<DetWin>();
	mutex.Unlock();
}


SSFDetwins::SSFDetwins(vector<DetWin> *detwins) {

	mutex.Lock();
	this->detwins = detwins;
	mutex.Unlock();
}


SSFDetwins::~SSFDetwins() {

	mutex.Lock();
	if (this->detwins != NULL)
		delete this->detwins;
	this->detwins = NULL;
	mutex.Unlock();
}

void SSFDetwins::SetDetwins(vector<DetWin> *detwins) {

	mutex.Lock();
	if (this->detwins != NULL)
		delete this->detwins;
	this->detwins = detwins;
	mutex.Unlock();
}


vector<DetWin> *SSFDetwins::GetDetwins() {
vector<DetWin> *ptr;

	mutex.Lock();
	ptr = detwins;
	mutex.Unlock();

	return ptr;
}


size_t SSFDetwins::GetNumberofDetWins() { 
size_t n;

	mutex.Lock();
	n = detwins->size();
	mutex.Unlock();

	return n;
}



void SSFDetwins::SelectDetwin(size_t idx) {

	mutex.Lock();
	if (detwins->size() <= idx)
		ReportError("Invalid window index: %d (of %d)", idx,  detwins->size());
	selectedDetwin.insert(idx);
	mutex.Unlock();
}



void SSFDetwins::SetResponse(size_t idx, float response) {

	mutex.Lock();
	if (detwins->size() <= idx)
		ReportError("Invalid window index: %d (of %d)", idx,  detwins->size());
	detwins->at(idx).response = response;
	mutex.Unlock();
}



void SSFDetwins::SelectDetwinRandomly(size_t n) {
vector<size_t> randsel;
size_t i;

	mutex.Lock();
	// clear the current selection
	selectedDetwin.clear();

		// if number of windows is smaller than the one that should be selected, select all
	if (n >= detwins->size()) {
		for (i = 0; i < detwins->size(); i++) {
			selectedDetwin.insert(i);
		}
	}
	else { // select random windows	
		randsel = GenerateRandomPermutation(detwins->size(), n);
		for (i = 0; i < randsel.size(); i++) {
			selectedDetwin.insert(randsel[i]);
		}
	}
	mutex.Unlock();

}



vector<DetWin> *SSFDetwins::RetrieveSelected() {
unordered_set<size_t>::iterator it;
vector<DetWin> *vec;

	vec = new vector<DetWin>();
	mutex.Lock();
	for (it = selectedDetwin.begin(); it != selectedDetwin.end(); it++) {
		vec->push_back(detwins->at(*it));
	}
	mutex.Unlock();

	return vec;
}



void SSFDetwins::AddDetWindow(DetWin detwin) {

	mutex.Lock();
	detwins->push_back(detwin);
	mutex.Unlock();
}