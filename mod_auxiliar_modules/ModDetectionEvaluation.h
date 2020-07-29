#ifndef _DETECTCTION_EVALUATION_
#define _DETECTCTION_EVALUATION_


class ModDetectionEvaluation : public SSFUserModule {
friend class SSFTest;
string inModSldWinPos;
string inModSldWinNeg;
string inModMatricesPos;
string inModMatricesNeg;
string metric;
string outFile;

	// FPPW
	void FPPW();

	// actual evaluate FFPW
	void EvaluateFPPW(vector<FeatType> &posResp, vector<FeatType> &negResp);

public:
	ModDetectionEvaluation(string modID);

	// return ID of the input format 
	string GetName() { return "ModDetectionEvaluation"; }

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
