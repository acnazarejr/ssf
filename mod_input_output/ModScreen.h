#ifndef _MOD_SCREEN_H
#define _MOD_SCREEN_H



class ModScreen : public SSFUserModule {
int xwindow, ywindow;
string dataType;
string inModImg;
string inModSldwin;
string inModSamples;
int delay;

	// display the original image
	void DisplayImage(string windowName);

	// display BGS masks
	void DisplayMasks(string windowName);

	// display detwins
	void DisplayDetwins(string windowName);

	// display samples
	void DisplaySamples(string windowName);

public:
	ModScreen(string modID);

	// return ID of the input format 
	string GetName() { return "ModScreen"; }

	// Return the type of the module  
	string GetClass() { return SSF_USERMOD_CLASS_Mod_IO; }

	// retrieve structure version
	string GetVersion() { return "0.0.1"; }

	// Instantiate a new instance of this method
	SSFMethod *Instantiate(string instanceID); 	

	// function to check if input modules have been set correctly
	void Setup();

	// execute
	void Execute();
};





#endif