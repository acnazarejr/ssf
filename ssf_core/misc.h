#ifndef _MISC_H_
#define _MISC_H_



/*****************
 * Error handling 
 *****************/
void ReportError2(const char *file, const char *function, int line, const char* format_str, ...); 
void ReportWarning2(const char *file, const char *function, int line, const char* format_str, ...);
void ReportStatus2(const char *file, const char *function, int line, const char* format_str, ...);

#if defined(linux)
#define ReportError(fmt, ...) ReportError2(__FILE__, "", __LINE__, fmt, ##__VA_ARGS__)
#define ReportWarning(fmt, ...) ReportWarning2(__FILE__, "", __LINE__, fmt, ##__VA_ARGS__)
#define ReportStatus(fmt, ...) ReportStatus2(__FILE__, "", __LINE__, fmt, ##__VA_ARGS__)
#else
#define ReportError(fmt, ...) ReportError2(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define ReportWarning(fmt, ...) ReportWarning2(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define ReportStatus(fmt, ...) ReportStatus2(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#endif


/********************
 * String functions 
 ********************/
string aToString(size_t value, char *mask = "");
string IntToString(int value, char *mask = "%d");
string BoolToString(bool value);
bool StringToBool(string value);
float StringToFloat(string value);
int StringToInt(string value);
size_t StringToSizet(string value);
string FloatToString(float value, char *mask = "%f");

// retrieve timestamp
size_t GenerateTimeStamp();



#endif