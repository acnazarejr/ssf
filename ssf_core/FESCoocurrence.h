#ifndef _FES_COOCURRENCE_H_
#define _FES_COOCURRENCE_H_



class FESCoocurrence :	public FeatureExtractionCPU2D {
co_occurrence_general *cooc;
int nbins;
int distance; 
string channel;
SSFMatrix<int> data;

	// get number of features for this method
	int GetNFeatures();

	// return current version of the method
	string GetVersion() { return "0.0.1"; }

public:
	FESCoocurrence(string instanceID);
	~FESCoocurrence();

	// Return the name of the module
	string GetName() { return SSF_METHOD_FES_FEATURE_COOC; }

	// retrieve the type of the method
	string GetType() { return SSF_METHOD_TYPE_FEATURE; }

	// create a new instance of this method
	FESCoocurrence* Instantiate(string instanceID);

	// local setup
	void Setup();

	// set image
	void SetImage(Mat &img);

	// extract features without selection
	void ExtractFeatures(int x0, int y0, int x1, int y1, SSFMatrix<FeatType> &m);
};

#endif