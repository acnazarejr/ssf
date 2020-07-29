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
#include "files.h"


FileManager::FileManager() {
	nentries = 0;
}


size_t FileManager::GetInputType(string inputname) {

	input = CheckInputType(inputname);
	return input.type;
}

void FileManager::SetInputFile(string inputname, string fileMask) {
InputType input;
	
	// check input type
	input = CheckInputType(inputname);

	/* file types */
	// invalid type
	if (input.type == FILETYPE_VIDEO) {
		ReportError("Invalid file type: %s", input.name.c_str());
	}

	// check if the input is a directory
	if (input.type == FILETYPE_DIR) {
		sampleList = ListFiles(inputname, fileMask);
		nentries = (int) sampleList.size();
	}

	// check if the input is a single file
	else if (input.type == FILETYPE_SINGLEIMG || input.type == FILETYPE_FILE) {
		sampleList.push_back(inputname);
		nentries = (int) sampleList.size();
	}

	if (nentries == 0)
		ReportError("No samples were found in '%s'\n", inputname.c_str());

	this->input = input;
}



string FileManager::RetrieveEntry(int idx) {

	// out of boundaries
	if (idx < 0 || idx > nentries) 
		ReportError("file index %d is out of boundaries [0,%d]", idx, nentries);

	return sampleList[idx];
}



InputType CheckInputType(string input) {
InputType  it;
IplImage *img;
CvCapture *cvCapt;
FILE *f;

	it = FileParts(input);
	img = cvLoadImage(input.c_str());
	if (img == NULL) 
		cvCapt = cvCreateFileCapture(input.c_str());
	else
		cvCapt = NULL;
	f = fopen(input.c_str(), "r");
#if defined(linux)
	DIR *dp;
	dp = opendir(input.c_str());
#endif

	if (img != NULL) { 
		it.type = FILETYPE_SINGLEIMG;
		cvReleaseImage(&img);
	}
	else if (cvCapt != NULL) {
		it.type = FILETYPE_VIDEO;
		cvReleaseCapture(&cvCapt);
	}
#if defined(WIN32)
	else if (f != NULL) {
#endif
#if defined(linux)
	else if (dp == NULL) {
#endif
		it.type = FILETYPE_FILE;
	}

	else {
		it.type = FILETYPE_DIR;
#if defined(linux)
		(void) closedir (dp);
#endif
	}

	if (f != NULL)
		fclose(f);

	return it;
}



InputType FileParts(string input) {
InputType ip;
int idx;

	ip.type = -1;
	// basename
	ip.name = ExtractOnlyName(input);

	// extension
	idx = (int) input.rfind(".");
	if (idx != input.npos)
		ip.ext = input.substr(idx, input.size());
	else
		ip.ext = string("");

	// path
	ip.path = input.substr(0, input.size() - ip.name.size() - ip.ext.size());

	if (ip.ext.size() > 0)
		ip.ext = ip.ext.substr(1, ip.ext.size());

	return ip;
}



string ExtractOnlyName(string filename) {
char namestr[1024];
int idx;

		idx = (int) filename.rfind(".");
		if (idx == filename.npos)
			return string("");

		memcpy(namestr, filename.c_str(), idx);
		namestr[idx] = 0;
		filename = namestr;
		idx = (int) filename.rfind("\\");
		filename = filename.substr(idx + 1, filename.size());

		// remove "/" is there is one
		idx = (int) filename.rfind("/");
		filename = filename.substr(idx + 1, filename.size());

		return filename;
}



///////////////////////////////
// list files in a directory //
///////////////////////////////
#if defined(_WIN32) 
vector<string> ListFiles(string ParentDir, string mask) {
UINT counter(0);
bool working(true);
string buffer, tmpbuffer;
//string fileName[1000];
set<string> filesMap;
vector<string> files;
set<string>::iterator it;

WIN32_FIND_DATA findFileData;
string FullPath;
wchar_t lpWideCharStr[200];
char bufff[260];
size_t convertedChars = 0;
//size_t  sizeInBytes = ((str->Length + 1) * 2);

	memset(lpWideCharStr, 0, 200 * sizeof(wchar_t));

	if ((int) mask.size() == 0)
		mask = "*.*";

	FullPath = ParentDir + string(FILESEP) + mask;

	MultiByteToWideChar(CP_ACP, 0, FullPath.c_str(), (int) FullPath.size(), lpWideCharStr, sizeof(lpWideCharStr));


	//wcstombs_s(convertedChars, bytechar, 
	HANDLE hFind = FindFirstFile(lpWideCharStr, &findFileData);

	if(hFind!=INVALID_HANDLE_VALUE) {
		if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
			memset(bufff, 0, 260);
			wtoc(bufff,  findFileData.cFileName);
			buffer = ParentDir + string(FILESEP) + string(bufff);
			filesMap.insert(buffer);
		}
       

	   while(working) {
			FindNextFile(hFind,&findFileData);
			if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
				continue;
			memset(bufff, 0, 260);
			wtoc(bufff,  findFileData.cFileName);
			tmpbuffer = ParentDir + string(FILESEP) + string(bufff); 
			if	(tmpbuffer != buffer) {
				memset(bufff, 0, 260);
				wtoc(bufff,  findFileData.cFileName);
				buffer = string(bufff);
				buffer = ParentDir + string(FILESEP) + buffer;
				filesMap.insert(buffer);
			}
			else {
				working=false;
			}
	   }
	}

	// convert from map to string
	for (it=filesMap.begin(); it!=filesMap.end(); it++) {
		files.push_back(*it);
	}

	return files;
}	



void wtoc(CHAR* Dest, const WCHAR* Source) {
int i = 0;

	while(Source[i] != '\0') {
		Dest[i] = (CHAR)Source[i];
		++i;
	}
}


string wtostring(TCHAR* Source) {
	string str;
	char ss[2048];
	memset(ss,0,2048);
	int i = 0;
	while(Source[i] != '\0') {
		ss[i] = (char)Source[i];
		i++;
	}
	str = string(ss);
	return str;
}
#endif


//#if 0
#if defined(linux)
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>

vector<string> ListFiles(string ParentDir, string mask) {
set<string> filesMap;
vector<string> files;
set<string>::iterator it;
string FullPath;
DIR *dp;
struct dirent *dirp;
string strr;
int idx;

	if ((int) mask.size() == 0)
		mask = "*.*";

	FullPath = ParentDir + string(FILESEP) + mask;
	mask = mask.substr(1);


    if((dp  = opendir(ParentDir.c_str())) == NULL) {
		printf("couldn't open directory: %s\n", ParentDir.c_str());
        exit(2);
    }

    while ((dirp = readdir(dp)) != NULL) {
		strr = dirp->d_name;
		idx = strr.find(mask.c_str());
		if (idx != -1)
	        filesMap.insert(ParentDir + string(FILESEP) + strr);
    }
    closedir(dp);

//	int i;
//	for (i = 0; i < (int) files.size(); i++) {
//		printf("%s\n", files.at(i).c_str()); 
//	}
//	printf("mask: %s\n", mask.c_str());
//	exit(2);

	// convert from map to string
	for (it=filesMap.begin(); it!=filesMap.end(); it++) {
		files.push_back(*it);
	}
    return files;
}

#endif



///////// Check if a directory exists
#if defined(_WIN32) 

bool CheckExistDirectory(string dirName) {
  DWORD ftyp = GetFileAttributesA(dirName.c_str());
  if (ftyp == INVALID_FILE_ATTRIBUTES)
    return false;  //something is wrong with your path!

  if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
    return true;   // this is a directory!

  return false;    // this is not a directory!
}
#endif


#if defined(linux)

bool CheckExistDirectory(string dirName) {
	
	ReportError("Must implement this function for linux!");

	return false;
}
#endif


