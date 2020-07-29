#ifndef _SplitYML_H_
#define _SplitYML_H_



class ModSplitYML : public SSFUserModule {
string InputMod;
string modelFileTrain;
string modelFileTest;
int nTraining;


public:
	ModSplitYML(string modID);
	~ModSplitYML();

	// return ID of the input format 
	string GetName() { return "ModSplitYML"; }

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