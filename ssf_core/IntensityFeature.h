#ifndef INTENSITYFEATURE_H
#define INTENSITYFEATURE_H
#include "headers.h"
#include "ExtractionMethod.h"


#define INTENSITY_FEATURES 1	// number of features per pixel
#define INTENSITYID "INTENSITY"


class IntensityFeature : public ExtractionMethod {
Mat img;

public:
	IntensityFeature();
	~IntensityFeature();

	// Execute setup for the method
	void Setup(string ID, AllParameters *params);

	// set image
	void SetImage(Mat &img);

	// extract features without selection
	void ExtractFeatures(int x0, int y0, int x1, int y1, SSFMatrix<FeatType> &m);

	// get number of features for this method
	int GetNFeatures(SSFRect &rect);

	// get feature unique identifier
	string GetID() { return INTENSITYID; }

	// create a new instance of this method
	virtual IntensityFeature* New();
};

#endif