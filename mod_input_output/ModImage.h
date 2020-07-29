#ifndef _MOD_IMAGE_H
#define _MOD_IMAGE_H


class ModImage : public SSFUserModule {
string inputName;
FileManager *fm;
string mask;

public:
	ModImage(string modID);
	~ModImage();

	// return ID of the input format 
	string GetName() { return "ModImage"; }

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