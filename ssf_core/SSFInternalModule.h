#ifndef _SSF_INTERNAL_MODULE_H_
#define _SSF_INTERNAL_MODULE_H_






/* 
 * Class for Internal Modules
 */
class SSFInternalModule : public SSFMethodThread {
friend class SSFMethodControl;

protected:

	// function to launch this method
	void Launch();

public:
	SSFInternalModule(string instanceID);

	// retrieve the type of the method
	string GetType() { return SSF_METHOD_TYPE_INTMOD; }
};



#endif