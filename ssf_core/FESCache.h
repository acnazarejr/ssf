#ifndef _FES_CACHE_H_
#define _FES_CACHE_H_


class ExtractionData2D;

typedef struct {
	string key;
	SSFMatrix<FeatType> data;
} HashEntry;



class FESCacheData {
SSFRWMutex mutex;
unordered_map<size_t, HashEntry *> data;
queue<size_t> creationQueue;		// queue to keep the newest ones
int maxSize;						// maximum number of elements in the queue
hash<string> hash_fn;

	
public:
	// allocate cache to support at most maxSize elements 
	FESCacheData(int maxSize);

	// reset cache
	void ResetCache();

	// add a feature to the cache
	void AddData(int x, int y, int w, int h, SSFMatrix<FeatType> &inData);

	// retrieve a feature from the cache - return false if the data is not there, otherwise, set the outData with the data
	bool GetData(int x, int y, int w, int h, SSFMatrix<FeatType> &outData);
};



// memory management for feature extraction
class FESCache {
vector<FESCacheData *> cache;
vector<int> indices;
int nbuckets;
SMIndexType nhits, nmisses;
SSFRWMutex mutex;

public:
	// create a cache divided in n parts to an image with width pixels, maxSize: maximum number of entries per cache, so n*maxSize
	FESCache(int n, int maxSize);

	// reset cache - adapt the cache for a different size of width
	void ResetCache(ExtractionData2D *data);

	// set data to cache
	void SetData(int x, int y, int w, int h, SSFMatrix<FeatType> &inData);

	// retrieve data from the cache - return true if available and also set outData
	bool GetData(int x, int y, int w, int h, SSFMatrix<FeatType> &outData);

	// report status
	void ReportCacheStatus();
};


#endif