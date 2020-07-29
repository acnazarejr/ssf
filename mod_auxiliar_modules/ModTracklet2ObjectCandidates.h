#ifndef _TRACKLET_2_OBJECTCANDIDATES_
#define _TRACKLET_2_OBJECTCANDIDATES_


class ModTracklet2ObjectCandidates : public SSFUserModule {
string inModTracklet;

public:
	ModTracklet2ObjectCandidates(string modID);

	// return ID of the input format 
	string GetName() { return "ModTracklet2ObjectCandidates"; }

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
