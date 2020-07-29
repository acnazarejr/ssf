#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "SSFData.h" 
#include "ModScreen.h"



ModScreen instance("");

ModScreen::ModScreen(string modID) : SSFUserModule(modID) {

	/* set default values */
	dataType = SSF_DATA_IMAGE;
	delay = 1;
	xwindow = 0;
	ywindow= 0;

	/* Add parameters */
	inputParams.Add("delay", &delay, "Delay between frames (in usec)", false);
	inputParams.Add("dataType", &dataType, "Type of data to display (IMAGES, MASKS, DETWINS, SAMPLES)", true);
	inputParams.Add("xwindow", &xwindow, "Location of the window in x-axis", false);
	inputParams.Add("ywindow", &ywindow, "Location of the window in y-axis", false);

	/* module information */
	info.Description("Module to show results in the screen.");

	/* set input and output data */
	modParams.Require(modID, SSF_DATA_IMAGE, &inModImg);			// input images
	modParams.Require(modID, SSF_DATA_SLDWINDOWS, &inModSldwin);	// input sliding windows 
	modParams.Require(modID, SSF_DATA_SAMPLES, &inModSamples);		// input samples 

	/* register the module */
	this->Register(this, modID);
}


void ModScreen::Setup() {

	if (dataType == "DETWINS" && inModSldwin == "")
		ReportError("Input module to provide the sliding windows has not been set (variable: inModSldwin)!");

	if (dataType == "IMAGES" && inModImg == "")
		ReportError("Input module to provide the image has not been set (variable: inModImg)!");

	if (dataType == "SAMPLES" && inModSamples == "")
		ReportError("Input module to provide the samples has not been set (variable: inModSamples)!");
}


SSFMethod *ModScreen::Instantiate(string instanceID) {

	return new ModScreen(instanceID); 
}




void ModScreen::DisplayImage(string windowName) {
SSFImage *img;
Mat data;
size_t frameID;
SMIndexType position = 0;

	while ((frameID = glb_sharedMem.RetrieveFrameIDbyPosition(inModImg, SSF_DATA_IMAGE, position++)) != SM_INVALID_ELEMENT) {

		img = glb_sharedMem.GetFrameImage(frameID);

		data = img->RetrieveCopy();

		imshow(windowName, data);

		data.release();

		ReportStatus("Showing frame position %d", img->GetFrameID());

		cv::waitKey(delay);
	}
}




void ModScreen::DisplayMasks(string windowName) {
SSFImage *img;
Mat data;
size_t frameID;
SMIndexType position = 0;

	while ((frameID = glb_sharedMem.RetrieveFrameIDbyPosition(inModImg, SSF_DATA_IMAGE, position++)) != SM_INVALID_ELEMENT) {

		img = glb_sharedMem.GetFrameMask(inModImg, frameID);

		data = img->RetrieveCopy();

		imshow(windowName, data);

		data.release();

		ReportStatus("Showing mask position %d", img->GetFrameID());

		cv::waitKey(delay);
	}
}



void ModScreen::DisplayDetwins(string windowName) {
vector<DetWin> *detwins;
SSFImage *img;
SSFDetwins *ssfdetwins;
SSFSlidingWindow *ssfSldWin;
SMIndexType position = 0;
size_t idx = 0;
size_t frameID;
Mat data;
int i;


	while ((frameID = glb_sharedMem.RetrieveFrameIDbyPosition(inModSldwin, SSF_DATA_SLDWINDOWS, position++)) != SM_INVALID_ELEMENT) {

		img = glb_sharedMem.GetFrameImage(frameID);
		ssfSldWin = glb_sharedMem.GetSlidingWindows(inModSldwin, frameID);
		
		ssfdetwins = ssfSldWin->GetSSFDetwins(0);
		detwins = ssfdetwins->GetDetwins();

		data = img->RetrieveCopy();

		// draw rectangles
		for (i = 0; i < detwins->size(); i++) {
			Rect rec;
			rec = detwins->at(i).GetDetWin();
			rectangle(data, rec, CV_RGB(255,255,255), 2);
		}

		ReportStatus("Showing detwins position %d", img->GetFrameID());

		imshow(windowName, data);

		data.release();

		cv::waitKey(delay);
	}
}



void ModScreen::DisplaySamples(string windowName) {
SSFImage *img;
SSFRect ssfRect;
SSFSamples *ssfSamples;
vector<SMIndexType> samples;
SSFSample *sample;
size_t frameID;
Mat data;
int i;
SMIndexType position = 0;


	while ((frameID = glb_sharedMem.RetrieveFrameIDbyPosition(inModSamples, SSF_DATA_SAMPLES, position++)) != SM_INVALID_ELEMENT) {

		img = glb_sharedMem.GetFrameImage(frameID);
		ssfSamples = glb_sharedMem.GetFrameSamples(inModSamples, frameID);
		samples = ssfSamples->GetSamples();

		data = img->RetrieveCopy();

		// draw rectangles
		for (i = 0; i < samples.size(); i++) {
			sample = glb_sharedMem.GetSample(samples[i]);
			ssfRect = sample->GetWindow();
			rectangle(data, ssfRect.GetOpencvRect(), CV_RGB(255,255, 0), 4);
			glb_sharedMem.UnlinkDataItem(sample);
		}

		imshow(windowName, data);

		data.release();

		glb_sharedMem.UnlinkDataItem(ssfSamples);

		cv::waitKey(delay);
	}
}





// execute module
void ModScreen::Execute() {
string windowName;

	windowName = this->GetInstanceID() + "::" +  dataType;

	namedWindow(windowName, CV_WINDOW_AUTOSIZE);

	moveWindow(windowName, xwindow, ywindow);

	if (dataType == "IMAGES") 
		DisplayImage(windowName);

	else if (dataType == "MASKS")
		DisplayMasks(windowName);

	else if (dataType == "DETWINS") 
		DisplayDetwins(windowName);

	else if (dataType == "SAMPLES") 
		DisplaySamples(windowName);

	else
		ReportError("Invalid display type: %s", dataType.c_str());

	// release window
	destroyWindow(this->GetInstanceID().c_str());
}