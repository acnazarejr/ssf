#ifndef _HEADERS_H_
#define _HEADERS_H_

// C
#include <stdio.h>
#include <stdlib.h>  

// C++
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <typeinfo>
#include <queue> 
#include <chrono>
#include <random>

// opencv
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

// pthreads
#include <pthread.h>



// namespaces
using namespace cv;
using namespace std;

// feature type
typedef float FeatType;


// shared memory definitions
#define SMIndexType long long int
#define SM_INVALID_ELEMENT -1

/*****
 * General Structures
 *****/
typedef struct {
	int x, y;
} SSFPoint;



// module types
#define TYPE_Mod_Filtering "Filtering"
#define TYPE_Mod_Detection "Detection"
#define TYPE_Mod_IO "IO"
#define TYPE_Mod_BGS "BGS"
#define TYPE_Mod_Camera "Camera"
#define TYPE_Mod_Filtering "Filtering"
#define TYPE_Mod_Activity "Activity"
#define TYPE_Mod_Auxiliar "Auxiliar"
#define TYPE_Mod_REID "REID"
#define TYPE_Mod_Recognition "Recognition"


// data structures
#define SSF_DATA_IMAGE "IMAGE"
#define SSF_DATA_FEED "FEED"
#define SSF_DATA_FRAME "FRAME" 
#define SSF_DATA_DETWINS "DETWINS"
#define SSF_DATA_DICT "DICT"
#define SSF_DATA_INVERTEDLIST "INVERTEDLIST"
#define SSF_DATA_SLDWINDOWS "SLDWINDOWS"
#define SSF_DATA_SAMPLE "SAMPLE"
#define SSF_DATA_SAMPLES "SAMPLES"
#define SSF_DATA_STRING "STRING"
#define SSF_DATA_CAMCOMM "CAMCOMMAND"
#define SSF_DATA_TRACKLET "TRACKLET"
#define SSF_DATA_OBJECT "OBJECT"
#define SSF_DATA_MATRICES "MATRICES"
//#define SSF_DATA_USER "USERDATA"
#define SSF_DATA_OBJECTCANDIDATES "OBJECTCANDIDATES"
#define SSF_DATA_MOD_EOD "MODEOD"		// indicate that module ended processing


// variables used to start a module
typedef struct {
	string ID;
	string modType;
	unordered_set<string> dataTypes;	// list of data types provided by this module
} DataStartModule;


// variables used to start a classification method
typedef struct {
	string ID;		// identifier of the module
} DataStartClassification;

// definition extra classes
#define EXTRA_CLASS "classExtra"






/*****
 * Matrix
 ****/
#define SSFMatrix Mat_




#define SSF_CURR_VERSION "0.0.3"
#define SSF_NEXT_VERSION "0.0.4"





// miscelaneous functions
#include "misc.h"

// math functions
#include "SSFMath.h"

// statistics 
#include "SSFStatistics.h" 

// threads
#include "SSFThreads.h"

// structure to store extracted features 
#include "SSFFeatures.h"

// queue
#include "SSFQueue.h"

// Time profiler for execution
#include "timeProfiler.h"

// input parameters
#include "SSFParameters.h"

#endif