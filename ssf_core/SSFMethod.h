#ifndef _SSF_METHOD_H_
#define _SSF_METHOD_H_
#include "headers.h"


class SSFThreadExecuteNew;





/* 
 * Class for methods such as feature extraction and classification
 */
class SSFMethod : public SSFStructure {
friend class SSFMethodControl;
string instanceID;					// unique ID of this instantiation

protected:

	void Register(SSFMethod *modPtr, string instanceID);

public:
	SSFMethod(string instanceID);
	~SSFMethod();

	// check if this method can is a SSFMethod
	bool IsSSFMethod() { return true; }

	// check if this method can be launched to a thread
	virtual bool EnabletoLaunch() { return false; }

	// Retrieve the ID for this module
	string GetInstanceID() { return this->instanceID; }

	// Instantiate a new instance of this method
	virtual SSFMethod *Instantiate(string instanceID) = 0; 
};




#endif