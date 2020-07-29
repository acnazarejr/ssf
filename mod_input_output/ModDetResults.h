#ifndef _MOD_DETRESULTS_H_
#define _MOD_DETRESULTS_H_


class SMSample;

class ModDetResults : public SSFUserModule {
string dirName;
string fileSuffix;
string operation; // read or write. DEFAULT=read
string dataType;  // SAMPLES or SLDWINDOWS
string inputModID;

	vector<DetWin> *LoadDetectionResults(string filename);

	void WriteDetectionResults(string filename, SSFSamples* ssfSamples);
	void WriteDetectionResults(string filename, SSFSlidingWindow* ssfSldWins);

	// operations
	void Writing();
	void Reading();

public:
	ModDetResults(string modID);
	~ModDetResults();

	// return ID of the input format 
	string GetName() { return "ModDetResults"; }

	// Return the type of the module  
	string GetClass() { return SSF_USERMOD_CLASS_Mod_IO; }

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