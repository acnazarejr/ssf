#ifndef _IO_CAMERAIP_H_
#define _IO_CAMERAIP_H_



class ModCameraIP : public SSFUserModule {
string inputName; // nao precisa
FileManager *fm;
string mask;
int CamNumber;

string protocol,arguments,CamModel,IP_string,yml,teste;

string IPAdress;


public:
	ModCameraIP(string modID);
	~ModCameraIP();

	// return ID of the input format 
	string GetName() { return "ModCameraIP"; }

	// Return the type of the module  
	string GetClass() { return SSF_USERMOD_CLASS_Mod_IO; }

	// retrieve structure version
	string GetVersion() { return "0.0.1"; }

	// Instantiate a new instance of this method
	SSFMethod *Instantiate(string instanceID); 	

	// function to check if input modules have been set correctly
	void Setup();

	// execute
	void Execute();
};


#endif