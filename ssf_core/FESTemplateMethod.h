#ifndef _EXTRACTION_FEATURE_METHOD_H_
#define _EXTRACTION_FEATURE_METHOD_H_


class SSFMethodFeatureExtraction : public SSFMethodNonThread {

public:
	SSFMethodFeatureExtraction(string instanceID);

	virtual int GetDataType() = 0;

	virtual int GetProcessingType() = 0;
};


// GPU methods
class FeatureExtractionGPU : public SSFMethodFeatureExtraction {

public:
	FeatureExtractionGPU(string instanceID);

	int GetProcessingType() { return FES_PROCESSING_GPU; }

	string GetType() { return SSF_METHOD_TYPE_FEATURE; }
};




// CPU methods
class FeatureExtractionCPU : public SSFMethodFeatureExtraction {


public:
	FeatureExtractionCPU(string instanceID);

	int GetProcessingType() { return FES_PROCESSING_CPU; }

	string GetType() { return SSF_METHOD_TYPE_FEATURE; }
};




class FeatureExtractionCPU2D : public FeatureExtractionCPU {

public:
	FeatureExtractionCPU2D(string instanceID);

	int GetDataType() { return FES_DATATYPE_2D; }

	// set image before performing feature extraction
	virtual void SetImage(Mat &img) = 0; 

	// Perform feature extraction for methods that implement single frame extraction (set the first row of the matrix).
	virtual void ExtractFeatures(int x0, int y0, int x1, int y1, SSFMatrix<FeatType> &m) = 0;
};




class FeatureExtractionTemporal : public FeatureExtractionCPU {

public:
	FeatureExtractionTemporal(string instanceID);

	int GetDataType() { return FES_DATATYPE_TEMPORAL; }

	// set image before performing feature extraction
	virtual void SetImage(vector<Mat> &imgs) = 0;
};



#endif