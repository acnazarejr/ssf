#ifndef _SSF_GARBAGE_COLLECTOR_H_
#define _SSF_GARBAGE_COLLECTOR_H_


// garbage collector for each module
typedef struct {
	unordered_set<SSFData *> entries;
} SSFGarbageMod;



class SSFGarbageCollector {
friend class SSFData;
friend class Module;
friend class SSFUserModule;
unordered_map<string, SSFGarbageMod *> data;
SSFMutex mutex;

protected:

	// add a data entry
	void AddEntry(SSFData *entry);

	// remove a data entry
	void RemoveEntry(SSFData *entry);

	// clean remaining entries of a module (entries that are not in the shared memory and weren't removed by the user)
	void CleanAllModEntries();

public:
	SSFGarbageCollector();
	~SSFGarbageCollector();

};

/* General functions */
extern SSFGarbageCollector glb_SSFGarbageCollector;

#endif