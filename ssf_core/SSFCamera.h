#ifndef _SSF_CAMERA_H_
#define _SSF_CAMERA_H_
#include "SSFData.h"




// class to control the camera
class SSFCamera : public SSFData  {
SSFMutex threadlock;
string data;

public:
	SSFCamera();	

	string GetCommand();
	void SetCommand(string str);
};



#endif