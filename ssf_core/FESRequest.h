#ifndef _FES_REQUEST_H_
#define _FES_REQUEST_H_


class FESData;

class InterestPoints {


};



// feature extraction request
class FESRequest {
friend class FESControl;
friend class FESMethodManager;
SSFMutex mutex;
FESData *data;
SSFQueue<FESRegion *> inQueue;
SSFQueue<FESRegion *> outQueue;
vector<size_t> featSignatures;		// unique signatures of the feature extraction methods to be used
SMIndexType NumbertoProcess;		// number of regions to be processed
bool noMoreInputData;				// indicates that no more data will be written

protected:
	// called by the extraction server to get the next element to be processed (read from the input queue)
	FESRegion *GetRegionToProcess();

	// set region to process
	void SetRegionToProcess(FESRegion *region);

	// retrieve processed region
	FESRegion *GetProcessedRegion();

	// no more requests will be placed
	void FinishWriting();

	// get data that will be processed
	FESData *GetData();

	// get feature ID that will be used
	vector<size_t> GetFeatSignatures();

public:
	FESRequest();

	// retrieve the request type
	virtual int GetRequestType() {
		ReportError("Invalid request type!");
		return FES_REQUESTTYPE_INVALID;
	}

	// data to extract feature featID from
	void AddData(FESData *data, size_t featSignature);

	// data to extract a set of features, in featIDs, from
	void AddData(FESData *data, vector<size_t> &featISignatures);

	// return true if all requests have been processed
	bool IsProcessingEnded();

	// called by the server to inform that the an element has been processed (write to the output queue)
	void SetProcessedRegion(FESRegion *region);
};




/***
 * Requests using regions
 ***/
class FESRequestRegions : public FESRequest {
friend class FESMethodManager;
SSFMutex mutexLocal;
FESBlocks *blocks;		// specific blocks for this request

protected:
	// request type
	int GetRequestType() { return FES_REQUESTTYPE_REGIONS; }

	// get blocks
	FESBlocks *GetBlocks();

public:
	// set request to extract feature (write to the input queue)
	void AddRegion(FESRegion *region);

	// set blocks to be used with ExtractionData
	void SetBlocks(FESBlocks *blocks);

	// retrieve extracted features - if available - return NULL if there is no more regions to be processed
	FESRegion *RetrieveExtractedFeatures();

	// no more extraction requests will be allowed
	void EndRequests();
};







/***
 * Requests using interest points
 ***/
class FESRequestInterestPoints : public FESRequest {
SSFMutex mutexLocal;
InterestPoints *intPoints;				// interest points for this request

protected:
	// get insterest points
	InterestPoints *GetInterestPoints();

public:
	// request type
	int GetRequestType() { return FES_REQUESTTYPE_INTPOINTS; }

	// set interest points to be used with ExtractionData
	void SetInterestPoints(InterestPoints *intPoints);

	// retrieve extracted features (wait until they are extracted)
	SSFFeatures *RetrieveFeatures();
};








#endif