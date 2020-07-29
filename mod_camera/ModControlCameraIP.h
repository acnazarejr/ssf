#ifndef _MOD_CONTROL_CAMERA_IP_H
#define _MOD_CONTROL_CAMERA_IP_H


class ModControlCameraIP : public SSFUserModule {
string feedName;
string initialSetup;



public:
	ModControlCameraIP(string modID);
	~ModControlCameraIP();

	// return ID of the input format 
	string GetName() { return "ModControlCameraIP"; }

	// Return the type of the module  
	string GetClass() { return SSF_USERMOD_CLASS_Mod_Camera; }

	// retrieve structure version
	string GetVersion() { return "0.0.1"; }

	// function to generate a new instatiation of the detector
	SSFMethod *Instantiate(string modID);

	// function to check if input modules have been set correctly
	void Setup();

	// execute
	void Execute();
};



#endif