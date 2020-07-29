#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "ModScaleManager.h"

ModScaleManager instance("");

/******
 * class ScaleManager
 ******/
ModScaleManager::ModScaleManager(string modID) :  SSFUserModule(modID) {

	scParam = new ScaleParam;

    /* Default Parameters */
    // if object's location is not specified, consider the whole detection window as the object
    objLocation.x0 = -1;
    objLocation.y0 = -1;
    objLocation.w  = -1;
    objLocation.h  = -1;

	/* module information */
	info.Description("Module to create a set of detection windows to cover an image.");

	/* Add parameters */
	inputParams.Add("width", &width, "Detection window's width", true);
	inputParams.Add("height", &height, "Detection window's height", true);

	/* Add scale parameters */
	inputParams.Add("hmin", &scParam->hmin, "Minimum scale", true);
	inputParams.Add("hmax", &scParam->hmax, "Maximum scale", true);
	inputParams.Add("strideX", &scParam->strideX, "Horizontal movement of the window (percentage of the width)", true);
	inputParams.Add("strideY", &scParam->strideY, "Vertical movement of the window (percentage of the height)", true);
	inputParams.Add("deltaScale", &scParam->deltaScale, "Scaling factor", true);

    /* Object's location */
	inputParams.Add("obj_x0", &objLocation.x0, "Object's x-position within the detection window", false);
	inputParams.Add("obj_y0", &objLocation.y0, "Object's y-position within the detection window", false);
	inputParams.Add("obj_w",  &objLocation.w,  "Object's width within the detection window", false);
	inputParams.Add("obj_h",  &objLocation.h,  "Object's height within the detection window."
		" If object's location is not specified, use the whole detection window", false);

	/* set input and output data */
	modParams.Require(modID, SSF_DATA_IMAGE, &inModImg);	// input image
	modParams.Provide(modID, SSF_DATA_SLDWINDOWS);			// output

	/* register the module */
	this->Register(this, modID);
}


void ModScaleManager::Setup() {

	if (inModImg == "")
		ReportError("Input module with images has not been set (variable: inModImg)!");
}


SSFMethod *ModScaleManager::Instantiate(string modID) {

	return new ModScaleManager(modID); 
}


void ModScaleManager::Execute() {
SMIndexType frameID = 0;
SMIndexType position = 0;
SSFImage *img;
size_t n_windows = 0;

	ReportStatus("Executing module %s [%s]", this->GetName().c_str(), this->GetInstanceID().c_str());

    // check whether object's location has been set or not
	if (objLocation.x0 < 0 && objLocation.y0 < 0 && objLocation.w < 0 && objLocation.h < 0) {

        // detection window's size
        objLocation.x0 = 0;
        objLocation.y0 = 0;
        objLocation.w  = this->width;
        objLocation.h  = this->height;

		ReportStatus("Using object's location as the whole detection window: %d, %d, %d, %d", objLocation.x0,
			objLocation.y0, objLocation.w, objLocation.h);
	}

	// retrieve feedID
	while ((frameID = glb_sharedMem.RetrieveFrameIDbyPosition(inModImg, SSF_DATA_IMAGE, position++)) != SM_INVALID_ELEMENT) {
		// retrieve the next frame
		img = glb_sharedMem.GetFrameImage(frameID);

		ReportStatus("[Feed: %s] Detection for frameID '%d' at position '%d", inModImg.c_str(),  img->GetFrameID(), position - 1);
		
		// create several scales
		vector<ScaleInfo *> scales;
		CreateInputScales(scales);

		SSFSlidingWindow *slidingWindows = new SSFSlidingWindow();

		// for each scale, creates and saves detwins grouped by scale
		for (int i = 0; i < scales.size(); ++i) { 
			ScaleInfo *currScaleInfo = scales[i]; 

			// get image data and size of rescaled image
			Mat data = img->RetrieveCopy();
			int imgW = (int) floor((float) data.cols * currScaleInfo->scale); 
			int imgH = (int) floor((float) data.rows * currScaleInfo->scale);

			// create list of detection windows
			vector<DetWin> *detwins = new vector<DetWin>;
			for (int y = 0; y <= data.rows - currScaleInfo->detWindowSize.h; y += currScaleInfo->strideY) {
				for (int x = 0; x <= data.cols - currScaleInfo->detWindowSize.w; x += currScaleInfo->strideX) {
					DetWin detwin;
					detwin.h = currScaleInfo->detWindowSize.h; // detection window does not change its size
					detwin.w = currScaleInfo->detWindowSize.w;
					detwin.x0 = x;
					detwin.y0 = y;

					// remove those detection windows with coordinates outside of the image
					if ((detwin.x0 >= 0 && detwin.y0 >= 0) &&
						((detwin.x0 + detwin.w - 1) < imgW) &&
						((detwin.y0 + detwin.h - 1) < imgH)) {
						detwins->push_back(detwin);
						n_windows++;
					}
				} // for (int x = 0 ...
			} // for (int y = 0 ...

			// adds generated DetWin to SSFDetwins
			SSFDetwins *currDetWins = new SSFDetwins(detwins);
			
			// doesn't set to shared memory if there isn't any detwin
			if (currDetWins->GetNumberofDetWins() != 0) {
				slidingWindows->AddDetwins(currDetWins, currScaleInfo);  
			}
			else {
				delete currScaleInfo;
				delete currDetWins; 
			}
		} // for scale

		// sets sliding windows for each frame 
		glb_sharedMem.SetSlidingWindows(frameID, slidingWindows);		
	} // while ((frameID ...
	ReportStatus("total windows for all images: %d", n_windows);
}

void ModScaleManager::CreateInputScales(vector<ScaleInfo *> &scales) {
ScaleInfo *scaleInfo;
int h;
//	, w;
float scale;
int strideX;
int strideY;
SSFRect newObjSize;
SSFRect newImgSize;
SSFRect detWindowSize; 
	
    // detection window's size
	detWindowSize.w = this->width;
	detWindowSize.h = this->height;

    // object's size
	newObjSize.x0 = objLocation.x0;
	newObjSize.y0 = objLocation.y0;
	newObjSize.w  = objLocation.w;
	newObjSize.h  = objLocation.h;

	//w = (int) (((float) scParam->hmin / (float) this->height) * (float) this->width);
	//w = (int) (((float) scParam->hmin / (float) objLocation->h) * (float) objLocation->w); 
	// TODO1 use objLocation instead of detection window. For while, I'm considering that
	// a objLocation has the same size of a detection window

	for (h = scParam->hmin; h <= scParam->hmax;) {
		scaleInfo = new ScaleInfo;

		/// OLD: scale = (float) this->height / (float) h;
		scale = (float) objLocation.h / (float) h;
		//scale = (float) objLocation->h / (float) h; // TODO1 same situation described above

		
		strideX = max(1, (int) ceil((float) scParam->strideX * (float) objLocation.w));
		strideY = max(1, (int) ceil((float) scParam->strideY * (float) objLocation.h));

		//OLDstrideX = max(1, (int) ceil((float) scParam->strideX * (scale > 1 ? scale : 1) * (float) this->width));
		//OLDstrideY = max(1, (int) ceil((float) scParam->strideY * (scale > 1 ? scale : 1) * (float) this->height));

		// set scale information
		scaleInfo->strideX = strideX;
		scaleInfo->strideY = strideY;
		scaleInfo->scale = scale;
		scaleInfo->objSize = newObjSize;
		scaleInfo->detWindowSize = detWindowSize;
		
		// add scale to be used for this object class
		scales.push_back(scaleInfo);
		h = (int) floor((float) h * scParam->deltaScale);
		//w = (int) floor((float) w * scParam->deltaScale);
	}
}

ModScaleManager::~ModScaleManager() {
	delete scParam;
}