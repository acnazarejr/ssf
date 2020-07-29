#ifndef _MOD_FILE_LIST_H
#define _MOD_FILE_LIST_H



class ModFileList : public SSFUserModule {
string inputName;
Size rSize;
bool loadGrayscale;

	FileNode& ReadStorage(FileNode &node, string name, FileNode &value);

public:
	ModFileList(string modID);
	~ModFileList();

	// return ID of the input format 
	string GetName() { return "ModFileList"; }

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