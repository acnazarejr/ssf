#ifndef _SSF_RECT_H_
#define _SSF_RECT_H_
#include "SSFStorage.h" 

// Detwin structure
class SSFRect {

public:

	SSFRect();
	SSFRect(Rect cvRect);
	SSFRect(int x, int y, int width, int height);
	SSFRect(const SSFRect &r);

	// return true if this rectangle has been set
	bool IsRectValid();

	// set the rectangle
	void SetRect(int x0, int y0, int w, int h);

	// get openCV Rect
	Rect GetOpencvRect();

	// attribute a rectangle to this
	void operator=(const SSFRect& r);

	// save the data into the storage
	void Save(SSFStorage &storage);

	// load the data from the storage
	void Load(const FileNode &node);

	int x0, y0, w, h;
};


#endif