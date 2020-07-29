#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_
#include "headers.h"
#if defined(_WIN32)
#include <windows.h> 
#include <ctime>
#endif
#if defined(linux)
#include <stddef.h>
#include <stdio.h> 
#include <sys/types.h>
#include <dirent.h>
#endif


// type of input file
#define FILETYPE_VIDEO 0x1		//  video sequence
#define FILETYPE_SINGLEIMG 0x2	// single image
#define FILETYPE_DIR 0x3		// directory
#define FILETYPE_FILE 0x4		// another type of file


typedef struct {
	int type;
	string path;
	string name;
	string ext;
} InputType;


// Class to manage the input files
class FileManager {
vector<string> sampleList;	// list of samples
int nentries;				// number of entries
InputType input;			// input type

public:
	// constructor
	FileManager();

	// check input type
	size_t GetInputType(string inputname);

	// Input considering files in different formats 
	void SetInputFile(string inputname, string fileMask);

	// Retrieve file according to the position idx.
	string RetrieveEntry(int idx);

	// Get number of entries (e.g., return the number of images in the directory).
	int GetNEntries() { return nentries; }
};


#if defined(_WIN32) || defined(__CYGWIN__)
#define FILESEP "/"
#elif linux
#define FILESEP "/"
#endif


// check type of input
InputType CheckInputType(string input);

// retrieve file parts
InputType FileParts(string input);

// extract filename
string ExtractOnlyName(string filename);

// list files in a directory
vector<string> ListFiles(string ParentDir, string mask);
#if defined(_WIN32)
void wtoc(CHAR* Dest, const WCHAR* Source);
string wtostring(TCHAR* Source);
#endif

// check if a directory exist
bool CheckExistDirectory(string dirName);



#endif