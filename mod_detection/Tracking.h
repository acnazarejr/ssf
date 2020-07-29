#ifndef _CVPD_H_ //Evitar a chamada recursiva do .h
#define _CVPD_H_
#include "headers.h"
#include "module.h"
#include "DetectionDefs.h"

class Tracking : public Module {
	
	string inputModID;	//feed de treinamento
	string feedNameTrain;	//feed de teste

    static Tracking& Instance() {
        static Tracking instance("");
        return instance;
   }	


public:
	Tracking(string modID);//Construtor 

	// Return the name of the module   
	string GetModName() { return DETECTOR_ID_TRACKING; }

	// Return the type of the module  
	string GetModType() { return TYPE_Mod_Detection; }

	// function to generate a new instatiation of the detector
	Module *Instantiate(string modID);

	// execute
	void Execute();

	// function to check if the parameters are set correctly
	void Setup();

};

static inline Point calcPoint(Point2f center, double R, double angle)
{
    return center + Point2f((float)cos(angle), (float)-sin(angle))*(float)R;
}

#endif
