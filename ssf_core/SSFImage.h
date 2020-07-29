#ifndef _SSFIMAGE_H_
#define _SSFIMAGE_H_
#include "SSFLAPData.h"
#include "SSFRect.h" 




class SSFImage : public SSFLAPData {
friend class SSFTest;
SSFMutex threadlock;
Mat image;

	// save the image in the storage
	void Save(SSFStorage &storage);

	// load the image from the storage
	void Load(const FileNode &node);

    // create an image with the patch defined in window and rescale it
	SSFImage *RetrieveResizedPatch(SSFRect window, Size size, double fx, double fy, int interpolationMethod);

public:
	~SSFImage();
	SSFImage();
	SSFImage(Mat image);
	SSFImage(string filename);
	SSFImage(const FileNode &node);

	// create an image with the patch defined in window
	SSFImage *RetrievePatch(SSFRect window);

	// create an image with the patch defined in window and resize it to width x height
	SSFImage *RetrieveResizedPatch(SSFRect window, int width, int height, int interpolationMethod = INTER_LINEAR);

	// create an image with the patch defined in window and resize it by factors fx, fy
	SSFImage *RetrieveResizedPatch(SSFRect window, double fx, double fy, int interpolationMethod = INTER_LINEAR);

    // retrieve image resized by width x height
    SSFImage *RetrieveResized(int width, int height, int interpolationMethod = INTER_LINEAR);

	// save the image as a file
	void Save(string filename);

	// retrieve a copy of this image (does not provide the original to avoid multiple accesses)
	Mat RetrieveCopy();

	// retrieve number of columns
	int GetNCols();

	// retrieve number of rows
	int GetNRows();
};


#endif