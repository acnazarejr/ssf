#include "headers.h"
#include "SSFInfo.h"



void SSFInfo::Version(string version) {

	this->version = version;
}


void SSFInfo::Description(string desc) {

	this->description = desc;
}



void SSFInfo::Author(string name, string email) {

	this->author = name;
	this->email = email;
}


void SSFInfo::Date(int day, int month, int year) {

	this->day = day;
	this->month = month;
	this->year = year;
}
