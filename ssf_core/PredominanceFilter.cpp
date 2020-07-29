#include "headers.h"
#include "ExtractionMethod.h"
#include "PredominanceFilter.h"


PredominanceFilter::PredominanceFilter() {
	
	threshold = (float)0.1;

	inputParams.Add("threshold", &threshold, "Color space chosen for creates the histogram ", true);
}


PredominanceFilter::~PredominanceFilter() {

	if (img.data != NULL)
		img.release();
}


void PredominanceFilter::Setup(string ID, AllParameters *params) {

	/* Set input parameters */
	inputParams.SetParameters(ID, params);

	// check if the input modules have been set correctly
	if (threshold < 0 || threshold > 1)
		ReportError("Parameter 'threshold' must be set correctly. Interval [0-1]");
}


void PredominanceFilter::SetImage(Mat &img) {

	if (this->img.data != NULL)
		this->img.release();

	this->img = img;
}


void PredominanceFilter::ExtractFeatures(int x0, int y0, int x1, int y1, SSFMatrix<FeatType> &m) {

	vector<Mat> planes;
	Mat src, b, g, r;
	Scalar Mean, mB, mG, mR;
	float meanB, meanG, meanR, maxGB, maxRB, maxRG;

	// Conversion BGR
	cvtColor(img, img, CV_RGB2BGR);

	// Setup a rectangle to define your region of interest
	Rect myROI(x0, y0, x1-x0, y1-y0);

	// Crop the full image to that image contained by the rectangle myROI
	src = img(myROI);

	Mean =  mean(src);

	meanB = (float)Mean[0];
	meanG = (float)Mean[1];
	meanR = (float)Mean[2];

	float g_b = Subt(meanG, meanB);
	float r_b = Subt(meanR, meanB);
	float r_g = Subt(meanR, meanG);

	maxGB = Max(meanG, meanB);
	maxRB = Max(meanR, meanB);
	maxRG = Max(meanG, meanR);


	if((meanR - maxGB) > threshold) {
		m(0,0) = meanR;
		m(0,1) = 0;
		m(0,2) = 0;
	}
	if((meanG - maxRB) > threshold) {
		m(0,0) = 0;
		m(0,1) = meanG;
		m(0,2) = 0;
	}
	if((meanB - maxRG) > threshold) {
		m(0,0) = 0;
		m(0,1) = 0;
		m(0,2) = meanB;
	}
	if(g_b < threshold) {
		if (g_b > (meanR + threshold)) {
			m(0,0) = 0;
			m(0,1) = meanG;
			m(0,2) = meanB;
		}
	}
	if(r_b < threshold) {
		if (r_b > (meanG + threshold)) {
			m(0,0) = meanR;
			m(0,1) = 0;
			m(0,2) = meanB;
		}
	}
	if(r_g < threshold) {
		if (r_g > (meanB + threshold)) {
			m(0,0) = meanR;
			m(0,1) = meanG;
			m(0,2) = 0;
		}
	}
	if(r_g < threshold){
		double resp = Subt(r_g, meanB);
		if(resp > 0.5) {
			m(0,0) = meanR;
			m(0,1) = meanG;
			m(0,2) = meanB;
		}
	}
	if(r_g < threshold){
		double resp = Subt(r_g, meanB);
		if(resp <= 0.5) {
			m(0,0) = 0;
			m(0,1) = 0;
			m(0,2) = 0;
		}
	}
}

int PredominanceFilter::GetNFeatures(SSFRect &rect) {

	return PREDOMINANCE_FEATURES * 3;
}


PredominanceFilter *PredominanceFilter::New() { 

	return new PredominanceFilter(); 
}

float PredominanceFilter::Max(float v1, float v2) {
	
	if(v1 > v2)
		return v1;
	else
		return v2;
}

float PredominanceFilter::Subt(float v1, float v2) {
	
	float valor = (v1 - v2);
	if(valor >= 0)
		return valor;
	else
		return (-1)*(valor);
}