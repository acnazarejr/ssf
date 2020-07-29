#ifndef _PLSDETECTOR_FEAT_H_
#define _PLSDETECTOR_FEAT_H_


class ModPLSDetectorFeat : public SSFUserModule {
string executionType;
string posInputMod;
string negInputMod;
string testInputMod;
string trainModelMod;
string featFile;
string featSetup;
string modelFile;

	// function to perform feature extraction
	SSFFeatures *FeatureExtraction(string inputMod, vector<int> featIdxs);
	
	// execute learning
	void Learning();

	// execute testing
	void Testing();

	// execute FES 
	void Features();

	// save learned model
	void Save();

	// load stored model
	void Load();

public:
	ModPLSDetectorFeat(string modID);
	~ModPLSDetectorFeat();

	string GetName() { return "ModPLSDetectorFeat"; }

	// Return the type of the module  
	string GetClass() { return SSF_USERMOD_CLASS_Mod_Detection; }

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