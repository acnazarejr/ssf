#ifndef HISTOGRAMFEATURE_H
#define HISTOGRAMFEATURE_H
#include "headers.h"
#include "ExtractionMethod.h"


#define HISTOGRAM_FEATURES 1	
#define HISTOGRAMID "HISTOGRAM"


class HistogramFeature : public ExtractionMethod {
Mat img;
int nBins;
string colorSpace;
string normalization;
int nChannel;

public:
	HistogramFeature();
	~HistogramFeature();

	// Execute setup for the method
	void Setup(string ID, AllParameters *params);

	// set image
	void SetImage(Mat &img);

	// extract features without selection
	void ExtractFeatures(int x0, int y0, int x1, int y1, SSFMatrix<FeatType> &m);

	// get number of features for this method
	int GetNFeatures(SSFRect &rect);

	// get feature unique identifier
	string GetID() { return HISTOGRAMID; }

	// create a new instance of this method
	virtual HistogramFeature* New();
};

#endif