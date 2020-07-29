#ifndef _PLSDETECTOR_H_
#define _PLSDETECTOR_H_

class ModPLSDetector : public SSFUserModule {
FeatureExtControl featExt;
string executionType;
string posInputMod;
string negInputMod;
string testInputMod;
string trainModelMod;
string featFile;
string featSetup;
string modelFile;
float threshold;
int nfactors;
PLS *pls;
QDA *qda;

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
	ModPLSDetector(string modID);
	~ModPLSDetector();

	// return ID of the input format 
	string GetName() { return "ModPLSDetector"; }

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