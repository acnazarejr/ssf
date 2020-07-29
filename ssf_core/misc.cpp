#include "headers.h"
#include "SSFCore.h"
#include "files.h"
#include "SSFThreads.h"
#include "misc.h"


#if defined(linux)
#include <inttypes.h>
#include <unistd.h>
#else 
#include <windows.h> 
#endif


#if 0
void ReportError(const char* format_str, ...) {
int required_length;
string m_str;
va_list ap;

	va_start(ap, format_str);
	required_length = (int) strlen(format_str) + 1;  // for null terminator

	std::vector<char> tmp_vec(required_length); 
	vsnprintf(&(tmp_vec[0]), required_length, format_str, ap);
	va_end(ap);
	m_str = std::string(tmp_vec.begin(), tmp_vec.end());

	fprintf(stdout, "%s(%s.%d): \n  %s\n", __FILE__, "function", __LINE__, m_str.c_str());        
    exit(-1); 
}
#endif



void ReportError2(const char *file, const char *function, int line, const char* format_str, ...) {
int required_length;
string m_str;
va_list ap;
InputType inputName;
char buffer[10256];

	inputName = FileParts(file);

	va_start(ap, format_str);
	required_length = (int) strlen(format_str) + 1;  // for null terminator
		
	vsprintf (buffer, format_str, ap);
	m_str = buffer;

	va_end(ap);
	

#if defined(linux)
	fprintf(stdout, "A fatal error occurred!\nFile: %s:%d\nMessage: %s\n", file, line, 
		m_str.c_str());        
#else
	fprintf(stdout, "A fatal error occurred!\nFile: %s:%d\nFunction: %s()\nMessage: [%s] %s\n", file, line, 
		function, glb_SSFExecCtrl.GetInstanceIDCurrentThread().c_str(), m_str.c_str()); 
#endif
	exit(-1); 
}




void ReportWarning2(const char *file, const char *function, int line, const char* format_str, ...) {
int required_length;
string m_str;
va_list ap;
InputType inputName;
char buffer[10256];

	inputName = FileParts(file);

	va_start(ap, format_str);
	required_length = (int) strlen(format_str) + 1;  // for null terminator
		
	vsprintf (buffer, format_str, ap);
	m_str = buffer;
	va_end(ap);

#if defined(linux)
	fprintf(stdout, "\nWarning!\nFile: %s:%d\nMessage: %s\n\n", file, line, 
		m_str.c_str());        
#else
	fprintf(stdout, "\nWarning!\nFile: %s:%d\nFunction: %s()\nMessage: %s\n\n", file, line, 
		function, m_str.c_str()); 
#endif
}



void ReportStatus2(const char *file, const char *function, int line, const char* format_str, ...) {
int required_length;
string m_str;
va_list ap;
InputType inputName;
char buffer[10256];
static ofstream statusFile("statusFile.txt", ios::out);
static SSFMutex mutex;

	mutex.Lock();
	inputName = FileParts(file);

	va_start(ap, format_str);
	required_length = (int) strlen(format_str) + 1;  // for null terminator
		
	vsprintf (buffer, format_str, ap);
	m_str = buffer;
	va_end(ap);

#if defined(linux)
	fprintf(stdout, "\nStatus!\nFile: %s:%d\nMessage: %s\n\n", file, line, 
		m_str.c_str());        
#else
	fprintf(stdout, "Status:\n[File: %s:%d]\n[Function: %s()]\n[Message: [%s] %s]\n\n", file, line, function, glb_SSFExecCtrl.GetInstanceIDCurrentThread().c_str(), m_str.c_str()); 
	statusFile << "Status:\n[File:" << file << ":" << line << "]\n[Function: " << function << "()]\n[Message: [" << glb_SSFExecCtrl.GetInstanceIDCurrentThread().c_str() << "] " <<  m_str.c_str() << "]\n\n";
#endif
	mutex.Unlock();
}



string aToString(size_t value, char *mask) {
char str[1024];

	if (strlen(mask) == 0)
		sprintf(str, "%llu", value);
	else 
		sprintf(str, mask, value);

	return string(str);

}

string IntToString(int value, char *mask) {
char str[1024];

	sprintf(str, mask, value);

	return string(str);
}


string BoolToString(bool value) {

	if (value == true)
		return string("1");

	return string("0");
}


bool StringToBool(string value) {

	if (value.compare("true") == 0 || value.compare("1") == 0)
		return true;
	return false;
}


float StringToFloat(string value) {

	return (float) atof(value.c_str());
}


int StringToInt(string value) {

	return atoi(value.c_str());
}


size_t StringToSizet(string value) {
size_t ll;

	sscanf(value.c_str(), "%llu", &ll);
	return ll;
}

string FloatToString(float value, char *mask) {
char str[1024];

	sprintf(str, mask, value);

	return string(str);
}


size_t GenerateTimeStamp() {

	return (size_t) getTickCount();
}