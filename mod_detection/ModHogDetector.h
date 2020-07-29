#ifndef _CVPD_H_ //Evitar a chamada recursiva do .h
#define _CVPD_H_
#include "headers.h"
//#include "module.h"
//#include "DetectionDefs.h"

class ModHogDetector : public SSFUserModule {
int strideX; 
int strideY;
float scale; // scaling factor
string inputModID;	//feed de treinamento

public:
	ModHogDetector(string modID);//Construtor 
	//~ModHogDetector();

	// Return the name of the module   
	string GetName() { return "ModHogDetector"; }

	// Return the type of the module  
	string GetClass() { return SSF_USERMOD_CLASS_Mod_Detection; }

	// retrieve structure version
	string GetVersion() { return "0.0.1"; }

	// function to generate a new instatiation of the detector
	SSFMethod *Instantiate(string modID);

	// function to check if the parameters are set correctly
	void Setup();

	// execute
	void Execute();

};


#endif
