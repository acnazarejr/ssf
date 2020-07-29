#ifndef _KALMANFILTER_H_ //Evitar a chamada recursiva do .h
#define _KALMANFILTER_H_
//#include "headers.h"
//#include "module.h"
//#include "DetectionDefs.h"

class ModKalmanTracking : public SSFUserModule {
	
string inputModID;	//feed de treinamento
string feedNameTrain;	//feed de teste

  /*  static ModKalmanTracking& Instance() {
        static ModKalmanTracking instance("");
        return instance;
		
   }*/

public:
	ModKalmanTracking(string modID);//Construtor 
	~ModKalmanTracking();

	// return ID of the input format    
	string GetName() { return "ModKalmanTracking"; }

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
