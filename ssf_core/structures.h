#ifndef _STRUCTURES_H_
#define _STRUCTURES_H_



// class that keeps a list of structures used with the shared memory
class SSFStructures {
map<string, SSFData *> structureMap;

public:
	SSFStructures();

    static SSFStructures& Instance() {
        static SSFStructures instance;
        return instance;
    }

	// check if dataType is valid
	bool CheckDataType(string dataType);
};





#endif