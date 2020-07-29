#ifndef _SLIDING_WINDOW_2_SAMPLES_
#define _SLIDING_WINDOW_2_SAMPLES_


class ModSlidingWindow2Samples : public SSFUserModule {
string inModSldWin;

public:
	ModSlidingWindow2Samples(string modID);

	// return ID of the input format 
	string GetName() { return "ModSlidingWindow2Samples"; }

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
