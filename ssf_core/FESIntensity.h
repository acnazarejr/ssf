#ifndef _FES_INTENSITY_H_
#define _FES_INTENSITY_H_


class FESIntensity : public FeatureExtractionCPU2D {
Mat img;
bool useColor;		// use color information instead of grayscale (3x more features)

	// get number of features for this method
	int GetNFeatures(int w, int h);

	// return current version of the method
	string GetVersion() { return "0.0.1"; }

public:
	FESIntensity(string instanceID);
	~FESIntensity();

	// Return the name of the module
	string GetName() { return SSF_METHOD_FES_FEATURE_INTENSITY; }

	// retrieve the type of the method
	string GetType() { return SSF_METHOD_TYPE_FEATURE; }

	// create a new instance of this method
	FESIntensity* Instantiate(string instanceID);

	// local setup
	void Setup();

	// set image
	void SetImage(Mat &img);

	// extract features without selection
	void ExtractFeatures(int x0, int y0, int x1, int y1, SSFMatrix<FeatType> &m);
};


#endif