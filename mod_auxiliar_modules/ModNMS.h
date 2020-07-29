#ifndef _AUX_NOM_MAX_SUP_H_
#define _AUX_NOM_MAX_SUP_H_


class ModNMS : public SSFUserModule {
string inputModID;
float threshold;
float intersection;

	vector<DetWin> *NonMaxSupression(vector<DetWin> Listdetwins, float Inters, float threshold);


public:
	ModNMS(string modID);

	// return name of the module
	string GetName() { return "ModNMS"; }

	// Return the type of the module  
	string GetClass() { return SSF_USERMOD_CLASS_Mod_Auxiliar; }

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
