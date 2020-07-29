#ifndef _SSF_SLIDING_WINDOW_H_
#define _SSF_SLIDING_WINDOW_H_
#include "SSFLAPData.h"
#include "DetWin.h" 
#include "SSFDetwins.h"
#include "ScaleInfo.h"
#include "SSFImage.h"




class SSFSlidingWindow : public SSFLAPData {
SSFMutex mutex;
vector<SSFDetwins *> detwins;
vector<ScaleInfo *> scaleInfo;
vector<SSFImage *> imgs;
int naccess;			// control the number of simultaneous access to this structure

	// save the data to a storage
	virtual void Save(SSFStorage &storage) { ; }

	// load the data from a storage
	void Load(const FileNode &node) { ; }

public:
	SSFSlidingWindow();
	~SSFSlidingWindow();

	void AddDetwins(SSFDetwins *detwins);
	void AddDetwins(SSFDetwins *detwins, ScaleInfo *scaleInfo);

	size_t GetNEntries();

	SSFDetwins *GetSSFDetwins(size_t idx);

	ScaleInfo *GetScaleInfo(size_t idx);

	SSFImage *GetSSFImage(size_t idx);

	// retrieve all detection windows independent of the scale
	vector<DetWin> *GetAllDetWins();

	// retrieve the total number of detection windows for all scales
	size_t GetNumberofWindows();

	// release images
	void ReleaseImages();

	// load images
	void LoadImages(SSFImage *img);
};



#endif