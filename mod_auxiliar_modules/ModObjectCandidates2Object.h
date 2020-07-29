#ifndef _MOD_OBJECTCANDIDATES_2_OBJECT_
#define _MOD_OBJECTCANDIDATES_2_OBJECT_


class ModObjectCandidates2Object : public SSFUserModule {
string inMod;

public:
	ModObjectCandidates2Object(string modID);

	// return name of the module
	string GetName() { return "ModObjectCandidates2Object"; }

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