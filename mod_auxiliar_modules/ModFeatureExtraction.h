#ifndef _MODFEATURE_EXTRACTION_
#define _MODFEATURE_EXTRACTION_


class ModFeatureExtraction : public SSFUserModule {
string inModFrames;
string inModTracklets;
string inputType;
string featFile;
string featSetup;
int featSetupIdx;

	// extract features from frames
	void ExtractFeatFrames();

	// extract features from samples
	void ExtractFeatSamples();

	// extract features from tracklets
	void ExtractFeatTracklets();

public:
	ModFeatureExtraction(string modID);

	// return name of the module
	string GetName() { return "ModFeatureExtraction"; }

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
