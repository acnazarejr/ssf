#ifndef _FES_REGION_H_
#define _FES_REGION_H_

class FESMemoryManagement;

class FESRegion {
friend class FESMethodManager;
friend class FESMethodInstanceCPU2D;
FESMemoryManagement *memManager;		// pointer to release memory when deleted
SSFMultipleFeatures *features;	

protected:
	// set features with memory already allocated
	void SetFeatures(SSFMultipleFeatures *feats, FESMemoryManagement *memManager);

	// retrieve memory for extracted using the featID extraction method
	SSFFeatures *RetrieveSingleExtraction(SMIndexType featID);

public:
	FESRegion();

	~FESRegion();

	// make a copy of the features to a new matrix
	const SSFMultipleFeatures *RetrieveFeatures();

	// return the number of methods already executed
	int NumberExtractionMethods();

	// set that one more extraction method was executed
	void SetExtractedOneMethod();

	// region type
	virtual int GetRegionType() = 0;

	// data type 
	virtual int GetDataType() = 0;
};




class FESRegion2D : public FESRegion {

public:
	int GetDataType() { return FES_DATATYPE_2D; }
};




class FESRegionWindow2D : public FESRegion2D {
SSFRect rect;

public:
	
	int GetRegionType() { return FES_REGIONTYPE_WINDOW; } 

	void SetRegion(SSFRect &rect) { this->rect = rect; } 

	SSFRect GetRegion() { return rect; }
};




class FESRegionPatch2D : public FESRegion2D {
Mat img;

public:
	int GetRegionType() { return FES_REGIONTYPE_PATCH; } 
};



#endif