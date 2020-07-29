#ifndef _CVPD_H_ //Evitar a chamada recursiva do .h
#define _CVPD_H_
#include "headers.h"
#include "module.h"
#include "DetectionDefs.h"

class CVPedestrianDetection : public Module {
	
	string inputModID;	//feed de treinamento
	string feedNameTrain;	//feed de teste

    static CVPedestrianDetection& Instance() {
        static CVPedestrianDetection instance("");
        return instance;
   }	

public:
	CVPedestrianDetection(string modID);//Construtor 

	// Return the name of the module   
	string GetModName() { return DETECTOR_ID_CV_PEDDETECTION; }

	// Return the type of the module  
	string GetModType() { return TYPE_Mod_Detection; }

	// function to generate a new instatiation of the detector
	Module *Instantiate(string modID);

	// execute
	void Execute();

	// function to check if the parameters are set correctly
	void Setup();

};


#endif
