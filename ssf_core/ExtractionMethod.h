#ifndef _EXTRACTION_METHOD_H_
#define _EXTRACTION_METHOD_H_
#include "headers.h"
#include "parameters.h"


/*
 * This virtual class is provided to have a standard to implement feature extraction methods.
 * It should not be used directly by the user. The user should use FeatureInterface class instead.
 */
class ExtractionMethod {

protected:
InputParameters inputParams;

public:
	ExtractionMethod() { ; }

	// Execute setup for the method
	virtual void Setup(string ID, AllParameters *params) {
		ReportError("Function Setup() has not been implemented for feature extraction method '%s'!", GetID().c_str()); 
	}

	// set image before performing feature extraction
	virtual void SetImage(Mat &img) { 
		ReportError("Function SetImage() has not implemented feature extraction for single frame for feature '%s'!", GetID().c_str()); 
	}

	// set image before performing feature extraction
	virtual void SetImage(vector<Mat> &imgs) {
		ReportError("Function SetImage() has not implemented feature extraction for multiple frames for feature '%s'!", GetID().c_str()); 
	}

	
	// Perform feature extraction for methods that implement single frame extraction (set the first row of the matrix).
	virtual void ExtractFeatures(int x0, int y0, int x1, int y1, SSFMatrix<FeatType> &m) { 
		ReportError("Feature '%s' has not implemented feature extraction for single frame!", GetID().c_str()); 
	}

	// Return number of features in this method according to the block size
	virtual int GetNFeatures(SSFRect &rect) { return -1; }

	/*
	 * Return the feature identifier.
	 */
	virtual string GetID() { return ""; }
	

	/*
	 * Function to generate a new instance of the feature extraction method.
	 */
	virtual ExtractionMethod *New() {
		ReportError("Function New() has not been implemented for '%s'", this->GetID().c_str()); 
		return NULL;
	}
};

#endif