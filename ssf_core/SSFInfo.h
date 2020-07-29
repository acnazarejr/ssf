#ifndef _SSF_INFO_H_
#define _SSF_INFO_H_



class SSFInfo {
string description;	// description of this structure
string email;		// author's email
string author;		// author of this structure
string version;		// version of this structure
int month;			// date of this structure
int day;			// date of this structure
int year;			// date of this structure 

protected:
	// set version
	void Version(string version);

public:
	// set a textual description of this structure
	void Description(string desc);

	// set author of the module
	void Author(string name, string email);

	// set date
	void Date(int day, int month, int year);
};



#endif