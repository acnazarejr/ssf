#ifndef PREDOMINANCEFEATURE_H
#define PREDOMINANCEFEATURE_H
#include "headers.h"
#include "ExtractionMethod.h"


#define PREDOMINANCE_FEATURES 1	// number of features per pixel
#define PREDOMINANCEID "PREDOMINANCE"


class PredominanceFilter : public ExtractionMethod {
Mat img;
float threshold;

public:
	PredominanceFilter();
	~PredominanceFilter();

	// Execute setup for the method
	void Setup(string ID, AllParameters *params);

	// set image
	void SetImage(Mat &img);

	// extract features without selection
	void ExtractFeatures(int x0, int y0, int x1, int y1, SSFMatrix<FeatType> &m);

	// get number of features for this method
	int GetNFeatures(SSFRect &rect);

	// get feature unique identifier
	string GetID() { return PREDOMINANCEID; }

	// create a new instance of this method
	virtual PredominanceFilter* New();

	float Max(float v1, float v2);

	float Subt(float v1, float v2);
};

#endif