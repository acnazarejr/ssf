#ifndef _SCALE_MANAGER_H_
#define _SCALE_MANAGER_H_


typedef struct {
	int hmin;
	int hmax;
	float strideX;
	float strideY;
	float deltaScale;
} ScaleParam;


class ModScaleManager : public SSFUserModule {
string inModImg;

// size of the detection window
int width;  // detwin's width
int height; // detwin's height
ScaleParam *scParam; // scale parameters
SSFRect objLocation; // location of the object within 


	// creates detection windows on several scales
	void CreateInputScales(vector<ScaleInfo *> &scales);

	void SavePatch(SSFImage *img, DetWin *detwin, int position, int scale);

public:
	ModScaleManager(string modID);
	~ModScaleManager();

	// return ID of the input format 
	string GetName() { return "ModScaleManager"; }

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