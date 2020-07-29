#include "headers.h"
#include "SSFStatistics.h"




vector<size_t> GenerateRandomPermutation(size_t n, size_t k) {
vector<size_t> myvector;
vector<size_t> newvector;
vector<size_t>::iterator it;
int i;

	// set some values
	for (i = 0; i < n; i++) {	
		myvector.push_back(i);
	}
  
	// using built-in random generator:
	random_shuffle ( myvector.begin(), myvector.end() );

	for (i = 0; i < k; i++)
		newvector.push_back(myvector.at(i));
	
	return newvector;
}



void InitializeRandomNumberGenerator() {
unsigned seedi = (unsigned) std::chrono::system_clock::now().time_since_epoch().count();

	glb_RandonGenerator.seed(seedi);
}	



size_t GenerateSIZETRandomNumber() {
std::uniform_int_distribution<size_t> distribution(0,SIZE_MAX);

	return distribution(glb_RandonGenerator);
}