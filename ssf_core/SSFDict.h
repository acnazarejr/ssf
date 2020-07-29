#ifndef _SSFDICT_H_
#define _SSFDICT_H_
#include "headers.h"
#include "SSFLAPData.h"
#include "SSFRect.h" 

class SSFDict : public SSFLAPData {

protected:

SSFMatrix<FeatType> Dict;	// Dictionary
int nCWs;					// Number of Codewords

	// Return de distance between two SSFMatrix
	double SSFDict::Distance(SSFMatrix<FeatType> dict, SSFMatrix<FeatType> feat);

	// Return de Mahalanobis distance between two SSFMatrix
	double SSFDict::DistanceMahalanobis(SSFMatrix<FeatType> dict, SSFMatrix<FeatType> feat, SSFMatrix<FeatType> invcovar);

public:
	
	// construtor
	SSFDict();

	// destrutor
	~SSFDict();

	// function to return the dictionary
	void BuildDictionary(SSFMatrix<FeatType> BigMatrix, int nCWs);
	
	SSFDict *Clone();

	// Index of the lowest distance

	int ComputeDistance(SSFMatrix<FeatType> feat);

	int ComputeDistance(SSFMatrix<FeatType> feat, double &Dist);

	// save Dict
	void Save(string filename);
	void Save(SSFStorage &storage);

	// load Dict
	void Load(string filename);
	void Load(const FileNode &node);
};

#endif