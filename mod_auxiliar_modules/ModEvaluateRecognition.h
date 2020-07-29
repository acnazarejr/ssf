#ifndef _MOD_EVALUATE_RECOGNITION_H_
#define _MOD_EVALUATE_RECOGNITION_H_



class ModEvaluateRecognition : public SSFUserModule {
string inModObjPred;
string inModObjLabels;
string inModObjcCandidates;
string outFile;
string outType;
int maxRank;
ofstream out;
unordered_map<SMIndexType, SMIndexType> trackletID2ObjID;

	void CM();

	void CMC();

public:
	ModEvaluateRecognition(string modID);

	// return name of the module
	string GetName() { return "ModEvaluateRecognition"; }

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
