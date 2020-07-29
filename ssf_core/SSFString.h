#ifndef _SSFSTRING_H_
#define _SSFSTRING_H_
#include "SSFLAPData.h"




class SSFString : public SSFLAPData {
SSFMutex threadlock;
string data;

	// save the data to a storage
	virtual void Save(SSFStorage &storage);

	// load the data from a storage
	void Load(const FileNode &node) { ; }


public:
	SSFString();	

	string GetString();
	void SetString(string str);
};


#endif