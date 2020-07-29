#ifndef _EXTRACTION_CONTROL_H_
#define _EXTRACTION_CONTROL_H_


class AllParameters;
class ExtractionMethod;
class FeatureMethods;
class FeatureBlocks;
class FeatBlockDef;
class SSFFeatures;
class SSFImage;



class FeatureIndex {
friend class FeatureExtControl;
unordered_map<string, size_t> setupMap;

protected:
	void SetMap(string featSetupName, size_t ID);

public:
	// return the ID associated to featSetupName
	size_t GetID(string featSetupName);

	// retrieve all IDs
	vector<size_t> GetAllIDs();

	// return the ID associated to featSetupName
	size_t operator[](string featSetupName);
};




class FeatureExtractionSetup {
friend class FeatureExtControl;
ExtractionMethod *extMethod;	// method that actually extracts the features
vector<FeatType> *tmpData;		// temporary data to be used in feature extraction
SSFMatrix<FeatType> dataTmp;	// temporary data to be used in feature extraction
FeatBlockDef *defaultBlockDef;	// definition of the block setup for a given feature setup

protected:
	// the creation of a feature extraction receives a method alredy instantiated and set up
	FeatureExtractionSetup(ExtractionMethod *extMethod, FeatBlockDef *blockDef);

	~FeatureExtractionSetup();

	void SetImage(SSFImage *img);

	void SetImage(vector<SSFImage*> &imgs);

	// perform feature extraction for a window inside the img (if *ssfFeats == NULL, create a new feature)
	void ExtractFeatures(SSFRect &window, FeatureBlocks *blocks, SSFFeatures &ssfFeats);

	// perform feature extraction for a window inside the img using block definition (if *ssfFeats == NULL, create a new feature)
	void ExtractFeatures(SSFRect &window, SSFFeatures &ssfFeats);

	// retrieve block definition
	FeatBlockDef *RetrieveBlockDefs();

	// function to retrieve blocks 
	FeatureBlocks *RetriveFeatureBlocks(SSFRect &window);

	// perform feature extraction for multiple frames (if *ssfFeats == NULL, create a new feature)
	//void ExtractFeatures(vector <Mat> &imgs, SSFFeatures *ssfFeats);
};




class FeatureExtControl {
unordered_map<size_t, int> IDMap;				// map setup from its unique ID to its index in the vector
vector<FeatureExtractionSetup *> extractSetup;	// feature extraction setups
FeatureMethods *methods;						// available feature extraction methods	
vector<SSFFeatures *> localSSFFeat;				// temporary storage for the features

	// create feature extraction method
	FeatureExtractionSetup *InstantiateExtSetup(string featSetupID, AllParameters *params); // map<string, string> &parameters);
	
public:
	FeatureExtControl();

	// set image from which the feature extraction will be performed (must be done before extracting features)
	void SetImage(int featSetupIdx, SSFImage *img);

	void SetImage(vector<int> &featSetupIdx, SSFImage *img);

	// set image from which the feature extraction will be performed (must be done before extracting features)
	void SetImage(int featSetupIdx, vector<SSFImage*> &imgs);

	// extract features for a single image using setup featSetupID within the window in the image
	// if blocks != NULL, it will extract only for those blocks
	void ExtractFeatures(int featSetupIdx, SSFRect &window, FeatureBlocks *blocks, SSFFeatures &ssfFeats);

	// extract feature for a set of feature setups at once
	void ExtractFeatures(vector<int> &featSetupIdx, SSFRect &window, SSFFeatures &ssfFeats);

	// retrieve the blocks for a given feature setup (if it is not available, return NULL)
	FeatBlockDef *RetrieveBlocks(int featSetupIdx);

	// retrieve the actual blocks for a setup given the window size
	FeatureBlocks *RetriveActualBlocks(int featSetupIdx, SSFRect &window);

	// extract features for multiple images using setup featSetupID (if *ssfFeats == NULL, allocate memory)
	//void ExtractFeatures(int featSetupIdx, SSFFeatures **ssfFeats);

	// retrieve the index for a given setup ID
	int RetrieveSetupIdx(size_t featSetupID);

	// setup feature control
	// return a class that keeps the feature setup IDs according to their names
	FeatureIndex Setup(string paramFile);

	// save feature configuration to be used afterwards (FINISH IMPLEMENTATION)
	void Save(SSFStorage &storage);
};


extern FeatureExtControl glb_featExtControl;


#endif