#ifndef _HAAR_DETECTOR_H_
#define _HAAR_DETECTOR_H_


class ModHaarDetector : public SSFUserModule{	

	string execution_type;	
	string model_file;
	string inModImage; //the IMAGE input module name
	string inModSample;//the SAMPLE input module name

	/*PreProcessing of Image parameters*/
	/**/bool grayScale;//treat image as grayscale or not
	/**/bool histogramNormalization;// histogram equalization for dark images, or unequal brightness
	/*PreProcessing of Image parameters END*/

	/*Detector parameters:*/
	/**/int flags; //Flags for the detector
	/**/string flagString; //"int flags" Is dependant on this parameter
	/**/cv::Size minFeatureSize; //smallest face detection Window
	/**/int detWinSizeH; //Parameter that specifies the height of the minFeatureSize
	/**/int detWinSizeW; //Parameter that specifies the height of the minFeatureSize
	/**/float searchScaleFactor; //How many sizes to search
	/**/int minNeighbors; //higher value implies higher reliability
	/**/CascadeClassifier cascadeClassifier;//
	/*Detector parameters END*/

	/*ROI Parameters. A value of 0.5 in W means the width of the ROI will be half of the Sample,
	 a value of 0.25 of X means that X will be placed on a 1/4 of the height*/
	/**/float X; // A value between 0 and 1 that defines the X of the ROI as a Proportion of the Size of the Sample
	/**/float Y; // A value between 0 and 1 that defines the Y of the ROI as a Proportion of the Size of the Sample
	/**/float W; // A value between 0 and 1 that defines the Width of the ROI as a Proportion of the Size of the Sample
	/**/float H; // A value between 0 and 1 that defines the Height of the ROI as a Proportion of the Size of the Sample
	/**/Rect rectROI;
	/*ROI Parameters END*/


	void scaleROItoSample(cv::Rect ROI, vector<cv::Rect> &detections);/*This method tranforms the detection coordinates in order for them to be mapped with the origin in the face sample, instead of the ROI*/
	void getObjectROI(cv::Mat patch,cv::Rect &rectROI, cv::Mat &ROI);

	void convertGrayscale(cv::Mat &inp, cv::Mat &outp);/*Convert image to grayscale*/

	void imageDetection();

	void SampleDetection();	


public:
	ModHaarDetector(string modID);
	~ModHaarDetector();

	// return ID of the input format 
	string GetName() { return "ModHaarDetector"; }

	// Return the type of the module  
	string GetClass() { return SSF_USERMOD_CLASS_Mod_Detection; }

	// retrieve structure version
	string GetVersion() { return "0.0.1"; }

	// function to generate a new instatiation of the detector
	SSFMethod *Instantiate(string modID);

	// function to check if input modules have been set correctly
	void Setup();

	// execute
	void Execute();

};
#endif