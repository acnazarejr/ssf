#ifndef _MOD_TESTMULTIOUTPUTIN_
#define _MOD_TESTMULTIOUTPUTIN_


class ModTestMultiOutputIn : public SSFUserModule {
string inMod;

public:
	ModTestMultiOutputIn(string modID);

	// return name of the module
	string GetName() { return "ModTestMultiOutputIn"; }

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