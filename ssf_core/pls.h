#ifndef PLS_H
#define PLS_H
#include "headers.h"


class PLS {
SSFMatrix<FeatType> Xmean, Xstd, b, T, P, W, Wstar, Bstar, Ymean, Ystd, zdataV;
int nfactors;
	
public:
	// compute PLS model
	void runpls(SSFMatrix<FeatType> &X, SSFMatrix<FeatType> &Y, int nfactors);

	// return projection considering n factors
	void Projection(const SSFMatrix<FeatType> &X, SSFMatrix<FeatType> &projX, int nfactors);

	// retrieve the number of factors
	int GetNFactors();

	// projection Bstar considering a number of factors (must be smaller than the maximum)
	void ProjectionBstar(const SSFMatrix<FeatType> &X, SSFMatrix<FeatType> &ret);

	// save PLS model
	void Save(string filename);
	void Save(SSFStorage &storage);

	// load PLS model
	void Load(string filename);
	void Load(const FileNode &node);
};

#endif