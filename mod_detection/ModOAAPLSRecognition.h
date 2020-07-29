#ifndef _OAAPLSRECOGNITION_H_
#define _OAAPLSRECOGNITION_H_


class ModOAAPLSRecognition : public SSFUserModule {
// args
string executionType;
string trainInputMod;
string testInputMod;
string featFile;
string featSetup;
string modelFile;
int featSetupIdx;
int nfactors;
int maxrank;
int width, height;
// internal maps and models
PLS_OAA *pls_oaa;
unordered_map<string, size_t> objString2objID;
unordered_set<size_t> objectIDSet;


	// function to perform feature extraction
	SSFFeatures *FeatureExtraction(string inputMod, vector<int> featIdxs);
	
	// execute learning
	void Learning();

	// execute testing
	void Testing();

	// save learned model
	void Save();

	// load stored model
	void Load();

public:
	ModOAAPLSRecognition(string modID);
	~ModOAAPLSRecognition();

	// return ID of the input format 
	string GetName() { return "ModOAAPLSRecognition"; }

	// Return the type of the module  
	string GetClass() { return SSF_USERMOD_CLASS_Mod_Recognition; }

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
