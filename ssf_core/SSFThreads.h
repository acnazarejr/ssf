#ifndef _SSFTHREADS_H_
#define _SSFTHREADS_H_


class SSFMutex {
friend class SSFMutexCond;
pthread_mutex_t lock;

public:
	SSFMutex();
	~SSFMutex();

	// lock the code
	void Lock();

	// unlock the code
	void Unlock();
};



class SSFRWMutex {
pthread_rwlock_t lock;

public:
	SSFRWMutex();
	~SSFRWMutex();

	// lock to read
	void LockRead();

	// lock to write
	void LockWrite();

	// unlock 
	void Unlock();
};



// mutex conditional
class SSFMutexCond {
SSFMutex mutex;
pthread_cond_t cond;
bool conditionMet;

public:
	SSFMutexCond(bool initialCondition = true);
	~SSFMutexCond();

	void SetConditionTrue();

	void SetConditionFalse();

	void WaitforConditiontobeTrue();
};






// manager for conditional mutex
template <class T>
class CondMutexManager {
SSFMutex lock;
unordered_map<T, SSFMutexCond *> condMutex;		// conditional mutex to be used to wait for feeds to be created


public:
	// destructor
	~CondMutexManager() {
		unordered_map<T, SSFMutexCond *>::iterator it;
		SSFMutexCond *mtexCond;

		for (it = condMutex.begin(); it != condMutex.end(); it++) {
			mtexCond = it->second;
			delete mtexCond;
		}
	}


	// check if condition 'value' is satisfied, if not, locks the thread execution
	void CheckCondition(T value) {
		unordered_map<T, SSFMutexCond *>::iterator it;
		SSFMutexCond *cond;

		lock.Lock();
		it = condMutex.find(value);
		if (it == condMutex.end()) {
			cond = new SSFMutexCond(false);
			condMutex.insert(pair<T, SSFMutexCond *>(value, cond));
		}
		else {
			cond = it->second;
		}
		lock.Unlock();
		cond->WaitforConditiontobeTrue();
	}

	// set condition 'value' that will unlock the thread
	void UnlockCondition(T value) {
		unordered_map<T, SSFMutexCond *>::iterator it;
		SSFMutexCond *cond;

		lock.Lock();
		it = condMutex.find(value);
		if (it == condMutex.end()) {
			cond = new SSFMutexCond(true);
			condMutex.insert(pair<T, SSFMutexCond *>(value, cond));
		}
		else {
			cond = it->second;
			cond->SetConditionTrue();
		}
		lock.Unlock();	
	}

	bool EraseCondition(T value) {
		unordered_map<T, SSFMutexCond *>::iterator it;
		bool removed = false;	// return true if the condition was actually removed
		SSFMutexCond *delCond;

		lock.Lock();
		it = condMutex.find(value);
		if (it != condMutex.end()) {
			delCond = it->second;
			if (delCond != NULL)
				delete delCond;
			condMutex.erase(value);
			removed = true;
		}
		lock.Unlock();
		return removed;
	}
};


// return unique identifier for the current thread
void *SSFThreadGetSelf();



#endif