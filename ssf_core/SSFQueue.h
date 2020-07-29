#ifndef _SSF_MUTEX_QUEUE_H_
#define _SSF_MUTEX_QUEUE_H_



template <class T>
class SSFQueue {
CondMutexManager<size_t> requestCond;	// conditional mutex to assert that a processing has been requested
vector<T> dataQueue;					// list of requests
bool finishRequested;
size_t requestIdx;
SSFMutex mutex;

public:
	SSFQueue() {
		finishRequested = false;
		requestIdx = 0;
	}

	// set a new request
	void push(T data) {
		size_t index;

		mutex.Lock();
		index = dataQueue.size();
		dataQueue.push_back(data);
		
		// unlock conditional mutex indicating that a new request has been placed
		requestCond.UnlockCondition(index);
		mutex.Unlock();
	}

	// retrieve the request at index idx to be processed
	T pop() {
		T data;

		// wait until data be available
		requestCond.CheckCondition(requestIdx);
		requestCond.EraseCondition(requestIdx);

		mutex.Lock();
		data = dataQueue[requestIdx];
		requestIdx++;	// next position that should be retrieved
		mutex.Unlock();

		return data;
	}

	// no more data will be written
	void finishWriting() {
		size_t index;

		mutex.Lock();
		index = dataQueue.size();
		dataQueue.push_back(NULL);
		mutex.Unlock();

		requestCond.UnlockCondition(index);
	}

	size_t size() {
		size_t v;
		mutex.Lock();
		v = dataQueue.size() - requestIdx;
		mutex.Unlock();
		return v;
	}
};

#endif