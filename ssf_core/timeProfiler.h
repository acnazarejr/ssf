#ifndef TIMEPROFILER_H
#define TIMEPROFILER_H


/////////////////////
// Profiling class //
/////////////////////



typedef struct {
	int64 timestart;
	int64 total;
	int64 owntime;
	int64 ntimes;
	int count;
	string name;
	string filename;
	int line0, line1;
	int parent;
	vector<int> children;
	string threadID;
} TimeProfileEntry;

typedef struct {
      float r,g,b;
} COLOR;






class TimeProfileThread {
vector<TimeProfileEntry *> entry;
map<string,int> entryMap;
vector<string> executionStack;
vector<int> executionStackIds;
SSFMutex mutex;

int id;

private:

	// return a color in the heatmap
	COLOR GetColorHeatMap(float v, float vmin, float vmax);

public:
	TimeProfileThread();

	void Start2(const char *file, const char *function, int line);
	void Stop2(const char *file, const char *function, int line); 

#ifdef TIME_PROFILER
#define Startx() Start2(__FILE__, __FUNCTION__, __LINE__)
#define Stopx() Stop2(__FILE__, __FUNCTION__, __LINE__)
#else
#define Startx()  Start2(__FILE__, __FUNCTION__, __LINE__)
#define Stopx()  Stop2(__FILE__, __FUNCTION__, __LINE__)
#endif


	int PrintGraph(FILE *f, int idx0);
};






class TimeProfile {
map<string, TimeProfileThread *> entries;
SSFMutex mutex;

public:

	void Start2(const char *file, const char *function, int line);
	void Stop2(const char *file, const char *function, int line); 

#ifdef TIME_PROFILER
#define Startx() Start2(__FILE__, __FUNCTION__, __LINE__)
#define Stopx() Stop2(__FILE__, __FUNCTION__, __LINE__)
#else
#define Startx()  Start2(__FILE__, __FUNCTION__, __LINE__)
#define Stopx()  Stop2(__FILE__, __FUNCTION__, __LINE__)
#endif

	void Printx();

};


extern TimeProfile glb_TP;







#endif