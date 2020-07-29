#ifndef _DETWIN_H_
#define _DETWIN_H_
#include "SSFData.h"
#include "SSFRect.h"

// Detwin structure
class DetWin {

public:
	DetWin();
	DetWin(int x, int y, int width, int height, float response);
	DetWin(float respose, int x0, int y0, int x1, int y1);
	DetWin(const DetWin &d);
	DetWin(const CvRect &r, float response);
	CvRect GetDetWin();
	SSFRect GetSSFRect();
	bool IsValidDetWin();

	// save the data into the storage
	void Save(SSFStorage &storage);

	// load the data from the storage
	void Load(const FileNode &node);

	int x0, y0, w, h;
	float response;
};


#endif