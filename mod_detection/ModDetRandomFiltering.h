#ifndef _DET_RANDOM_FILTERING_H_
#define _DET_RANDOM_FILTERING_H_


class ModDetRandomFiltering : public SSFUserModule {
FeatureExtControl featExt;
string executionType;
string posInputMod;
//string negInputMod;
string testInputMod;
string featFile;
string featSetup;
string modelFile;
float threshold;
int nfactors;
PLS *pls;

// sample generation
string groundTruthMod;
int detWindowWidth_;
int detWindowHeight_;
int strideX_; // stride for each direction when generating samples
int strideY_;

	// execute learning
	void Learning(SSFFeatures &ssfFeats, SSFMatrix<FeatType> &dataY);

	// execute testing
	Mat Testing(string filename, SSFFeatures &ssfFeats, SSFMatrix<FeatType> &deltas);

	// save learned model
	void Save();

	// load stored model
	void Load();

    // feature extraction
    void FeatureExtraction(vector<SSFImage*> ssfSamples, Mat gtStrides, SSFFeatures &ssfFeats, SSFMatrix<FeatType> &dataY);

    // computes ground truth for subsampled windows
    Mat ComputeGroundTruth(DetWin gtWin, float threshold, int strideX, int strideY);

    // computes intersection(rectA,rectB) / union(rectA,rectB).
	float JaccardCoefficient(DetWin &gtWin, DetWin &detwin);

    // generates training samples for PLS regression
	void GenerateTrainingSamples(SSFImage *img, vector<DetWin> *detwins, vector<SSFImage*> &ssfSamples, Mat &deltas);

    // checks whether a sample is close to other one
    bool AreSamplesClose(DetWin &a, DetWin &b, float maxDistance);

    // computes mean squared error of the regression 
    float RootMeanSquaredError(Mat Yhat, Mat Y);

public:
	ModDetRandomFiltering(string modID);
	~ModDetRandomFiltering();

	// return ID of the input format 
	string GetName() { return "ModDetRandomFiltering"; }

	// Return the type of the module  
	string GetClass() { return SSF_USERMOD_CLASS_Mod_Detection; }

	// retrieve structure version
	string GetVersion() { return "0.0.1"; }

	// function to generate a new instatiation of the detector
	SSFMethod *Instantiate(string modID);

	// function to check if the parameters are set correctly
	void Setup();

	// execute
	void Execute();
};

#endif