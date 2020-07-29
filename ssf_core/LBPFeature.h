#ifndef LBPFEATURE_H
#define LBPFEATURE_H
#include "misc.h"
#include "LBP.h"


#define LBP_FEATURES 256	// number of features per pixel
#define LBPID "LBP"


class LBPFeature : public ExtractionMethod {
Mat img;

public:
	LBPFeature();
	~LBPFeature();

	// Execute setup for the method
	void Setup(string ID, AllParameters *params);

	// set image
	void SetImage(Mat &img);

	// extract features without selection
	void ExtractFeatures(int x0, int y0, int x1, int y1, SSFMatrix<FeatType> &m);

	// get number of features for this method
	int GetNFeatures(SSFRect &rect);

	// get feature unique identifier
	string GetID() { return LBPID; }

	// create a new instance of this method
	virtual LBPFeature* New();
};




#endif
