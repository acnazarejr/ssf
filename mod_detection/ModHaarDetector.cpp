#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "ModHaarDetector.h"

ModHaarDetector instance("");

/*Class 
* HaarDetector
*/
ModHaarDetector::ModHaarDetector(string modID) : SSFUserModule(modID){
	X =-1; Y=-1; H=-1; W=-1;
	detWinSizeW = -1; detWinSizeH = -1;
	
	/* module information */
	info.Description("Haar detector - OpenCV implementation.");

	/* Add parameters */
	/*The modParams.Add sets the classes parameters with the values specified in the param file*/
	inputParams.Add("execution_type", &execution_type, "Execute type (Image(e.g., for face detection) or sample)", true);	

	//PREPROCESSING PARAMS
	grayScale = true;
	inputParams.Add("grayScale",&grayScale,"Whether or not to detect objects on a grayscale image, \
										 if not provided it is default to true",false);
	histogramNormalization = true;
	inputParams.Add("histogramNormalization",&histogramNormalization,"Whether or not to equalize histogram before detection\
																   equalization helps on dark images, or uneven light sources\
																   if not provided it is default to true",false);
	//DETECTION PARAMS
	inputParams.Add("model_file", &model_file, "XML file containing training data for the detection.\
										   It's provided by opencv library, and is usually at the data folder of the opencv", true);
	flagString = CV_HAAR_FIND_BIGGEST_OBJECT;
	inputParams.Add("flagString", &flagString," Flag to be passed to the Haar detector",false);//if not provided it is default to CV_HAAR_FIND_BIGGEST_OBJECT
	inputParams.Add("detWinSizeH",&detWinSizeH,"The Height of the minimum detection Window",true);
	inputParams.Add("detWinSizeW", &detWinSizeW, "The Width of the minimum detection Window",true);
	minNeighbors = 5;
	inputParams.Add("minNeighbors",&minNeighbors,"The higher value this parameter has, the higher \
											   the reliability of the detection is, \
											   if not provided it is default to 5 ",false);
	searchScaleFactor = 1.1f;
	inputParams.Add("searchScaleFactor",&searchScaleFactor,"This is the delta multiplier \
														 of the detection windows \
														 if not provided it is default to 1.1f",false);
	//ROI PARAMS
	inputParams.Add("X",&X,"A value between 0 and 1 that defines the X of the ROI as a Proportion of the Size of the Sample",false);
	inputParams.Add("Y",&Y,"A value between 0 and 1 that defines the Y of the ROI as a Proportion of the Size of the Sample",false);
	inputParams.Add("W",&W,"A value between 0 and 1 that defines the W of the ROI as a Proportion of the Size of the Sample",false);
	inputParams.Add("H",&H,"A value between 0 and 1 that defines the H of the ROI as a Proportion of the Size of the Sample",false);


	//	set attributes for this module
	/* set input and output data */
	/**/modParams.Require(modID, SSF_DATA_IMAGE, &inModImage);
	//modParams.Require(modID, SSF_DATA_TYPE, Module you are requiring data from, DataID);
	// first param is the ID of the calling module
	// second param is the type of data the module will require
	// third param is the name of the module it is requiring data
	// fourth param is the id of the required input

	modParams.Require(modID, SSF_DATA_SAMPLES, &inModSample);	// for detections of objects within other objects

	modParams.Provide(modID, SSF_DATA_SAMPLES);//Specify what will be provided to the shared mem
	modParams.Provide(modID, SSF_DATA_SAMPLE);//Specify what will be provided to the shared mem
	//first param is the provider module ID
	// second param is the type of data provided


	/* register the module */
	this->Register(this, modID);
}



ModHaarDetector::~ModHaarDetector(void){
}

void ModHaarDetector::scaleROItoSample(cv::Rect ROI, vector<cv::Rect> &detections){
	int x,y,w,h;	
	x = ROI.x;
	y = ROI.y;
	w = ROI.width;
	h = ROI.height;

	for(vector<cv::Rect>::iterator it = detections.begin(); it != detections.end();it++){
		it->x = x + it->x;
		it->y = y + it->y;		
	}
}

void ModHaarDetector::getObjectROI(cv::Mat patch,cv::Rect &rectROI, cv::Mat &ROI){
	int left,top, width, height;
	
	left = cvRound(patch.cols * X);/* x = start from leftmost */          
	top = cvRound(patch.rows * Y); /* y = 1/4 of height from bottom */
	width = cvRound(patch.cols * W);/* width = same width with the face */
	height = cvRound(patch.rows * H);/* height = 1/3 of face height */

	rectROI = Rect(left, top, width, height);
	ROI = patch(rectROI);
}

SSFMethod *ModHaarDetector::Instantiate(string modID){
	return new ModHaarDetector(modID); 
}

void ModHaarDetector::Setup() {

	/* Things to be checked:
	*\/ Whether the xml training file has been properly loaded.
	*  Whether the detector parameters have been set
	*  Whether the ROI parameter have been set, for detection from samples.
	*  Whether its input is Sample or Image
	*/	

	if (execution_type == "IMAGES" && inModImage == ""){
		ReportError("Input module to provide the image has not been set (variable: inModImage)!");
	}

	if (execution_type == "SAMPLES" && inModSample == ""){
		ReportError("Input module to provide the samples has not been set (variable: inModSample)!");
	}

	try{
		this->cascadeClassifier.load(model_file);
	} catch (cv::Exception e){
		ReportError("Exception Message at Facialfeaturedetector::detect %s",e.msg);
	}

	if( cascadeClassifier.empty()){
		ReportError("Error: Couldnt load Face Detector (%s)!",model_file.c_str());
	}

	//check for ROI limits
	if(X<0 || Y<0 || H<0 || W<0){
		//ROI Region was not defined
		if(execution_type != "IMAGES"){
		//if input is Image, no Problem
		//else, report error
			ReportError("ROI for detection has not been set( variables: X,Y,H,W)!");
		}
	}

	//check for minimum detection window size
	if(detWinSizeW <= 0 || detWinSizeH <= 0){
		//detWin was not set;
		ReportError("Minimal size of the detection window has not been set(variables: detWinSizeW, detWinSizeH)!");
	}else{
		minFeatureSize.height = detWinSizeH;
		minFeatureSize.width = detWinSizeW;
	}
	//TODO Flags handling
	this->flags = CV_HAAR_FIND_BIGGEST_OBJECT;	

}

void ModHaarDetector::convertGrayscale(cv::Mat &inp, cv::Mat &outp){
	if(inp.channels() == 3){
		cv::cvtColor(inp, outp, CV_BGR2GRAY);
	}
	else if(inp.channels() == 4){
		cv::cvtColor(inp, outp, CV_BGRA2GRAY);
	}
	else{
		outp = inp;
	}
}

void ModHaarDetector::imageDetection(){
	size_t feedID, frameID;
	size_t position = 0;
	//vector<SMIndexType> samples;
	char nameImage[255];

	SSFSample *smp;
	DetWin detwin;
	vector<Rect> detections;

	ReportStatus("Executing module %s [%s]", this->GetName().c_str(), this->GetInstanceID().c_str());

	// retrieve feedID
	feedID = glb_sharedMem.GetFeedIDbyName(this->inModImage);
	int frame_it = 0;

	// perform detection for every frame
	while (true)
	{
		frameID = glb_sharedMem.RetrieveLastProcessedFrameID(inModImage, SSF_DATA_IMAGE, true) ;
		if(frameID == SM_INVALID_ELEMENT){
			break;
		}

		string filename = glb_sharedMem.GetFrameFilename(frameID);
	
		SSFImage *img = glb_sharedMem.GetFrameImage(frameID);
		
		Mat sourceImage = img->RetrieveCopy();
		Mat preProcImage = sourceImage.clone();

		if(grayScale){
			convertGrayscale(sourceImage,preProcImage);
		}
		if(histogramNormalization){
			cv::equalizeHist(preProcImage, preProcImage);
		}

		cascadeClassifier.detectMultiScale( preProcImage, detections, searchScaleFactor, minNeighbors, flags, minFeatureSize );

		// processing the current frame
		ReportStatus("[Feed: %d] Detection for frameID '%d'", inModImage,  frameID);

		int itNumber = 0;
		for(vector<cv::Rect>::iterator it = detections.begin(); it != detections.end();it++){			
			detwin.x0 = it->x;
			detwin.y0 = it->y;
			detwin.w = it->width;
			detwin.h = it->height;
			detwin.response = 1000;
			Mat matDetection(sourceImage, *it);
			/*TODO  this block is printing each detected face for a frame.
			* It should also print a text file with the detections*/					
			unsigned pos_st =filename.find_last_of("\\");
			pos_st++;
			unsigned pos_end = filename.find(".png");
			string root_name = filename.substr(pos_st, (pos_end-pos_st) );						
			/**/sprintf_s(nameImage,"FACES%s_frame_%d_%s_Detection_%d_.jpg",root_name.c_str(),frame_it,this->GetInstanceID().c_str(), itNumber);			
			/**/imwrite(nameImage, matDetection);//writes to a file named nameImage the image of a face detection defined by matDetection
			/*Detection printing END*/
			itNumber++;
			//todo debug detector and screen before reading something from the shared memory
			smp = new SSFSample();

			smp->SetPatch(SSFRect(*it));			

			//glb_sharedMem.SetFrameSample(frameID, smp);
			glb_sharedMem.AddFrameSample(frameID, smp);

			//samples.push_back(smp->GetSampleID());/*TODO Is this Correct? the new way to do this, is by passing Sample ID?*/
		}//frame detection loop END

		// setting detection windows 
		//glb_sharedMem.SetFrameSamples(frameID, samples);//Place the provided Data onto the shared Memory
		glb_sharedMem.EndFrameSamples(frameID);

		//samples.clear();
		frame_it++;
	}
}

void ModHaarDetector::SampleDetection(){
	size_t frameID;
	size_t position = 0;
	vector<SMIndexType> samples;
	char nameImage[255];

	SSFSample *sample;
	DetWin detwin;
	vector<Rect> detections;

	ReportStatus("Executing module %s [%s]", this->GetName().c_str(), this->GetInstanceID().c_str());

	int frame_it = 0;
	// perform detection for every frame
	while ((frameID = glb_sharedMem.RetrieveLastProcessedFrameID(inModSample, SSF_DATA_SAMPLES, true)) != SM_INVALID_ELEMENT){
		SSFImage *img = glb_sharedMem.GetFrameImage(frameID);		
		Mat sourceImage = img->RetrieveCopy();	
		Mat preProcImage = sourceImage.clone();

		string filename = glb_sharedMem.GetFrameFilename(frameID);
		
		SSFSamples *ssfSamples  = glb_sharedMem.GetFrameSamples(inModSample, frameID);
		samples = ssfSamples->GetSamples();
		
		if(grayScale){
			convertGrayscale(sourceImage,preProcImage);
		}
		if(histogramNormalization){
			cv::equalizeHist(preProcImage, preProcImage);
		}

		for (int i = 0; i < samples.size(); i++) {
		/* aplies detection for each sample*/
			sample = glb_sharedMem.GetSample(samples[i]);
			cv::Rect original_patch =sample->GetWindow().GetOpencvRect(); 
			Mat patch = preProcImage(original_patch);
			
			Mat ROI;

			this->getObjectROI(patch, rectROI, ROI);
			
			cascadeClassifier.detectMultiScale( ROI, detections, searchScaleFactor, minNeighbors, flags, minFeatureSize );

			this->scaleROItoSample(rectROI, detections);
			this->scaleROItoSample(original_patch, detections);

			// processing the current frame
			ReportStatus("[Feed: %d] Detection for frameID '%d'", inModSample,  frameID);

			int itNumber = 0;
			for(vector<cv::Rect>::iterator it = detections.begin(); it != detections.end();it++){			
				detwin.x0 = it->x;
				detwin.y0 = it->y;
				detwin.w = it->width;
				detwin.h = it->height;
				detwin.response = 1000;


				Mat matDetection(sourceImage, *it);
				/*TODO  this block is printing each detected face for a frame.
				* It should also print a text file with the detections*/					
				unsigned pos_st = filename.find_last_of("\\");
				pos_st++;
				unsigned pos_end = filename.find(".png");
				string root_name = filename.substr(pos_st, (pos_end-pos_st) );						
				/**/sprintf_s(nameImage,"FACES%s_frame_%d_%s_Detection_%d_.jpg",root_name.c_str(),frame_it,this->GetInstanceID().c_str(), itNumber);			
				/**/imwrite(nameImage, matDetection);//writes to a file named nameImage the image of a face detection defined by matDetection
				/*Detection printing END*/
				itNumber++;

				sample = new SSFSample();

				sample->SetPatch( SSFRect(*it));
				//glb_sharedMem.SetFrameSample(frameID, sample);//TODO do this method also needs the moduleID?
				glb_sharedMem.AddFrameSample(frameID, sample);

				//samples->push_back(sample->GetSampleID());/*TODO Is this Correct? 
				//									*the new way to do this, is by passing Sample ID?*/


			}//frame detection loop END

			// setting detection windows 
			//glb_sharedMem.SetFrameSamples(frameID, *samples);//Place the provided Data onto the shared Memory
			glb_sharedMem.EndFrameSamples(frameID);

			//samples.clear();												
		}				
		frame_it++;
	}
}

void ModHaarDetector::Execute(){

	if(execution_type=="IMAGES"){
		imageDetection();
	}else if(execution_type == "SAMPLES"){
		SampleDetection();
	}	
}