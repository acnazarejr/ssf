#ifndef _SSFFEATURES_H_
#define _SSFFEATURES_H_
#include "SSFData.h"



class SSFFeatures {
SSFMatrix<FeatType> dataM;	// matrix to store data
int lastRow;				// last used row

public:
	SSFFeatures();
	SSFFeatures(const FileNode &node) { ; } 
	~SSFFeatures(); 
	 
	// add a feature vector at the end of the current data (m.cols must be the same as dataM.cols)
	void AddFeatures(SSFMatrix<FeatType> &m);

	// reset the number of feature vectors - keep the memory allocated
	void Reset();

	// retrieve extracted features (if singleRow == true, return a single row matrix)
	SSFMatrix<FeatType> RetrieveFeatures(bool singleRow);
	
	// save the features in the storage
	void Save(SSFStorage &storage);

	// load the features from the storage
	void Load(const FileNode &node);

	//  create a copy of this feature (allocate new memory for the features)
	SSFFeatures *Copy();

	// return the number of bytes occupied for the features
	SMIndexType GetNumberBytes();
};



class SSFMultipleFeatures {
friend class FESRegion;
SSFMutex mutex;
vector<SSFFeatures *> feats;
unordered_map<size_t, size_t> featMap;		// map between feature ID and position in the vector
int methodsExtracted;						// number of extraction methods

protected:
	// retrieve the feature extraction structure for feature featID
	SSFFeatures *RetrieveSingleFeatureExtraction(size_t featID);

public:

	SSFMultipleFeatures();

	~SSFMultipleFeatures();

	// add a feature vector at the end of the current data (m.cols must be the same as dataM.cols) for the feature in position
	void AddFeatures(SSFMatrix<FeatType> &m, size_t position = 0);

	// reset the number of feature vectors - keep the memory allocated for the feature in position
	void Reset(size_t position = 0);

	// reset all methods
	void ResetAll();

	// retrieve extracted features (if singleRow == true, return a single row matrix) for the feature in position - does not allocate memory
	SSFMatrix<FeatType> RetrieveFeatures(bool singleRow, size_t position = 0);

	// retrieve all features by concatenating them into a single row
	void RetrieveAllFeatures(SSFMatrix<FeatType> &m);

	// retrieve the indexing position for feature with ID featID - if not available, create one 
	size_t RetrieveFeatureIndex(size_t featID);

	// save the features in the storage
	void Save(SSFStorage &storage);

	// load the features from the storage
	void Load(const FileNode &node);

	// create a copy of all features (allocate new memory for the features)
	SSFMultipleFeatures *Copy();

	// return the number of bytes occupied by the features
	SMIndexType GetNumberBytes();

	// return the number of feature extraction methods already performed
	int NumberExtractionMethods();

	// set that one feature method has been performed
	void SetExecutedMethod();
};

#endif