#ifndef _SSF_STRUCTURE_H_
#define _SSF_STRUCTURE_H_


class SSFStructure {
friend class SSFMethodControl;
string setupID;						// setup ID for this structure
	
protected:
SSFInputParameters inputParams;		// input parameters for this instance
SSFInfo	info;						// structure to keep information regarding this structure

	// save parameters
	void SaveParameters(SSFStorage &storage);

	// set setup ID for this structure
	void SetSetupID(string ID);

public:
	SSFStructure();
	~SSFStructure();

	// Execute setup for this method
	void Setup(SSFParameters *params, string setupID);

	// retrieve the method signature (unique for a set of parameters)
	size_t RetrieveSignature();

	// check if this method can is a SSFMethod
	virtual bool IsSSFMethod() { return false; }

	/* Functions to be implemented by the user */
	// retrieve the name of the method 
	virtual string GetName() = 0;

	// retrieve the type of the method
	virtual string GetType() = 0;

	// retrieve structure version
	virtual string GetVersion() = 0;

	// structure setup - function written by the user and called after parameters have been set
	virtual void Setup() = 0; 
};



#endif