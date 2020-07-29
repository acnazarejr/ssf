#include "headers.h"
#include "FES.h"
#include "co_occurrence_general.h"
#include "FESCoocurrence.h"


FESCoocurrence instance("");

FESCoocurrence::FESCoocurrence(string instanceID) : FeatureExtractionCPU2D(instanceID) {

	this->cooc = NULL;
	this->nbins = 16;
	this->distance = 1;
	this->channel = "H";

	inputParams.Add("nbins", &nbins, "Number of bins (matrix size)", false);
	inputParams.Add("distance", &distance, "Distance between two pixels", false);
	inputParams.Add("channel", &channel, "Channel to be used, can be either H, S, or V", false);

	this->Register(this, instanceID);
}


FESCoocurrence::~FESCoocurrence(){

	if (this->cooc != NULL)
		delete this->cooc;
}


FESCoocurrence *FESCoocurrence::Instantiate(string instanceID) { 

	return new FESCoocurrence(instanceID); 
}


void FESCoocurrence::Setup() {

	if (channel != "H" && channel != "S" && channel != "V")
		ReportError("Invalid colorband, must be H, S, or V");

	cooc = new co_occurrence_general();

	cooc->Initialize(this->nbins, this->distance);
}





void FESCoocurrence::ExtractFeatures(int x0, int y0, int x1, int y1, SSFMatrix<FeatType> &m) {

	m.create(1, cooc->GetNFeatures());

	cooc->ExtractFeatures(x0, y0, x1, y1, this->data, m);
}


void FESCoocurrence::SetImage(Mat &img) {
vector<cv::Mat> v_channel;
Mat hsv;

	if (this->data.data != NULL)
		this->data.release();

	// convert BRG to HSV
	cvtColor(img, hsv, CV_RGB2HSV);

	//split into three channels
	split(hsv, v_channel);

	if (channel == "H") {
		if (v_channel[0].data == 0)
			ReportError("Invalid conversion from RGB to HSV");
		v_channel[0].convertTo(data, CV_32S);
	}
	else if (channel == "S") {
		if (v_channel[1].data == 0)
			ReportError("Invalid conversion from RGB to HSV");
		v_channel[1].convertTo(data, CV_32S);
	}
	else if (channel == "V") {
		if (v_channel[2].data == 0)
			ReportError("Invalid conversion from RGB to HSV");
		v_channel[2].convertTo(data, CV_32S);
	}

	data = data / (256/this->nbins);
}