#include "headers.h"
#include "FES.h"
#include "FESIntensity.h"


FESIntensity instance("");


FESIntensity::FESIntensity(string instanceID) : FeatureExtractionCPU2D(instanceID) {

	this->useColor = false;

	inputParams.Add("useColor", &useColor, "Use color information instead of grayscale (3x more features descriptors)", false);

	this->Register(this, instanceID);
}


FESIntensity::~FESIntensity(){

	if (img.data != NULL)
		img.release();
}



FESIntensity *FESIntensity::Instantiate(string instanceID) { 

	return new FESIntensity(instanceID); 
}



void FESIntensity::Setup() {


}



int FESIntensity::GetNFeatures(int w, int h) { 

	if (useColor == false)
		return w * h;

	return 3 * w * h;
}



void FESIntensity::ExtractFeatures(int x0, int y0, int x1, int y1, SSFMatrix<FeatType> &m) {
int x, y, idx;
Vec3b color;
FeatType v;

	m.create(1, this->GetNFeatures(x1 - x0 + 1, y1 - y0 + 1));

	if (useColor == false) {
		for (idx = 0, y = y0; y <= y1; y += 1) {
			for (x = x0; x <= x1; x += 1) {
				color = img.at<Vec3b>(y,x);
				v = ((FeatType) (color.val[0] + color.val[1] + color.val[2])) / (FeatType) 3.0;
				m(0,idx++) = v;
			}
		}
	}
	else {
		for (idx = 0, y = y0; y <= y1; y += 1) {
			for (x = x0; x <= x1; x += 1) {
				color = img.at<Vec3b>(y,x);
				m(0,idx++) = (FeatType) color.val[0];
				m(0,idx++) = (FeatType) color.val[1];
				m(0,idx++) = (FeatType) color.val[2];
			}
		}
	}
}



void FESIntensity::SetImage(Mat &img) {

	if (this->img.data != NULL)
		this->img.release();

	this->img = img;
}