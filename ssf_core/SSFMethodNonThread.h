#ifndef _SSF_METHOD_NONTHREAD_H_
#define _SSF_METHOD_NONTHREAD_H_



/* 
 * Class for methods that cannot be launched as a new thread, such as feature extraction
 */
class SSFMethodNonThread : public SSFMethod {

public:
	SSFMethodNonThread(string instanceID);
};



#endif