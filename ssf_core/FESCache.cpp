#include "headers.h"
#include "SSFCore.h"
#include "FES.h"
#include "FESCache.h"


/*********************
 * class FESCacheData
 *********************/
FESCacheData::FESCacheData(int maxSize) {
HashEntry *entry;
size_t key;
int i;

	mutex.LockWrite();
	this->maxSize = maxSize;
	
	for (i = 0; i < maxSize; i++) {
		entry = new HashEntry();
		entry->key = aToString(i);
		key = hash_fn(entry->key);
		data.insert(pair<size_t, HashEntry *>(key, entry));
		creationQueue.push(key);
	}

	// set the maximum load factor to 0.8
	data.max_load_factor(0.8f);
	mutex.Unlock();
}




void FESCacheData::ResetCache() {
unordered_map<size_t, HashEntry *>::iterator it;
size_t removalkey;
HashEntry *entry;
size_t key;
int i;

	mutex.LockWrite();
	for (i = 0; i < maxSize; i++) {
		// remove the oldest element from the queue
		removalkey = creationQueue.front();
		creationQueue.pop();
	
		// remove its entry from the cache
		it = data.find(removalkey);
		entry = it->second;
		data.erase(it);

		// set the new entry to the cache
		entry->key = aToString(i);
		key = hash_fn(entry->key);
		data.insert(pair<size_t, HashEntry *>(key, entry));

		// add the new value to the queue
		creationQueue.push(key);
	}

	mutex.Unlock();
}



void FESCacheData::AddData(int x, int y, int w, int h, SSFMatrix<FeatType> &inData) {
unordered_map<size_t, HashEntry *>::iterator it;
//string inputStr;
size_t value;
size_t removalkey;
HashEntry *entry;

	// generate key and the hash
	///inputStr = aToString(x) + ":" + aToString(y) + ":" + aToString(w) + ":" + aToString(h);
	///value = hash_fn(inputStr);
	value = (size_t) x << 48 | (size_t) y << 32 | (size_t) w << 16 | (size_t) h;

	mutex.LockWrite();
	// check if value exists, once more
	it = data.find(value);
	if (it != data.end()) {
		mutex.Unlock();
		return;
	}

	// remove the oldest element from the queue
	removalkey = creationQueue.front();
	creationQueue.pop();
	
	// remove its entry from the cache
	it = data.find(removalkey);
	entry = it->second;
	data.erase(it);



	// set the new entry to the cache
	///entry->key = inputStr;
	inData.copyTo(entry->data);
	data.insert(pair<size_t, HashEntry *>(value, entry));

	// add the new value to the queue
	creationQueue.push(value);
	mutex.Unlock();
}




bool FESCacheData::GetData(int x, int y, int w, int h, SSFMatrix<FeatType> &outData) {
unordered_map<size_t, HashEntry *>::iterator it;
///string inputStr;
size_t key;

	// generate key and the hash
	///inputStr = aToString(x) + ":" + aToString(y) + ":" + aToString(w) + ":" + aToString(h);
	///key = hash_fn(inputStr);
	key = (size_t) x << 48 | (size_t) y << 32 | (size_t) w << 16 | (size_t) h; 

	mutex.LockRead();
	// search for the key in the cache
	it = data.find(key);

	// cache miss
	if (it == data.end()) {
		mutex.Unlock();
		return false;
	}

	// entry is in the cache, check the key
	///if (it->second->key != inputStr) {
	///	mutex.Unlock();
	///	return false;
	///}

	// cache hit
	it->second->data.copyTo(outData);
	mutex.Unlock();

	return true;
}




/****************
 * class FESCache
 ****************/
FESCache::FESCache(int n, int maxSize) {
FESCacheData *data;
int i;

	this->nbuckets = n;
	
	for (i = 0; i < n; i++) {
		data = new FESCacheData(maxSize);
		cache.push_back(data);
	}

	this->nhits = 0;
	this->nmisses = 0;
}



void FESCache::ResetCache(ExtractionData2D *data) {
int bucketSize;			// width of each bucket
int i;
int width;

	width = data->GetWidth();

	// restart bucket size
	bucketSize = width / this->nbuckets;
	indices.clear();
	
	for (i = 0; i < this->nbuckets; i++) {
		cache[i]->ResetCache();
		if (i == this->nbuckets - 1)
			indices.push_back(width);
		else
			indices.push_back((i + 1) * bucketSize);	
	}
}




void FESCache::SetData(int x, int y, int w, int h, SSFMatrix<FeatType> &inData) {
size_t i;

	// find the region in which the feature will be added
	for (i = 0; i < indices.size(); i++) {
		if (x < indices[i])
			break;
	}

	cache[i]->AddData(x, y, w, h, inData);
}



bool FESCache::GetData(int x, int y, int w, int h, SSFMatrix<FeatType> &outData) {
size_t i;
bool value;

	// find the region in which the feature will be added
	for (i = 0; i < indices.size(); i++) {
		if (x < indices[i])
			break;
	}

	value = cache[i]->GetData(x, y, w, h, outData);

	mutex.LockWrite();
	if (value == true)
		this->nhits++;
	else
		this->nmisses++;
	mutex.Unlock();

	return value;
}



void FESCache::ReportCacheStatus() {

	ReportStatus("Cache hit rate: %5.3f", (float) this->nhits / (float) (this->nmisses + this->nhits));

	ReportStatus("Cache hits: %d, cache misses: %d", this->nhits, this->nmisses);
}