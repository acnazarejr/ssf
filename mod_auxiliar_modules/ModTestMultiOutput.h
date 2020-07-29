#ifndef _MOD_TESTMULTIOUTPUT_
#define _MOD_TESTMULTIOUTPUT_


class ModTestMultiOutput : public SSFUserModule {
string inMod;

public:
	ModTestMultiOutput(string modID);

	// return name of the module
	string GetName() { return "ModTestMultiOutput"; }

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