#ifndef _RANDOM_FILTERING_H_
#define _RANDOM_FILTERING_H_


class ModRandomFiltering : public SSFUserModule {
string inModSldWin_;	// input sliding windows 

// parameters
float percentDetwins_;  // percentage of detection windows that will be selected
int seed_; // FIXME this should be unsigned

public:
	ModRandomFiltering(string modID);

	// return ID of the input format 
	string GetName() { return "ModRandomFiltering"; }

	// Return the type of the module  
	string GetClass() { return SSF_USERMOD_CLASS_Mod_Filtering; }

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
