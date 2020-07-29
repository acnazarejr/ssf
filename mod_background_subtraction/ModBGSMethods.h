#ifndef _BGS_H_
#define _BGS_H_


class ModBGSMethods : public SSFUserModule {
string inputModID;	//feed de treinamento
//string feedNameTest;	//feed de teste

public:
	ModBGSMethods(string modID);

	// return ID of the input format 
	string GetName() { return "ModBGSMethods"; }

	// Return the type of the module  
	string GetClass() { return SSF_USERMOD_CLASS_Mod_BGS; }

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