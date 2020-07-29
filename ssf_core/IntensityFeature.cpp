#include "headers.h"
#include "ExtractionMethod.h"
#include "IntensityFeature.h"


IntensityFeature::IntensityFeature() {
	;
}


IntensityFeature::~IntensityFeature() {

	if (img.data != NULL)
		img.release();
}


void IntensityFeature::Setup(string ID, AllParameters *params) {

	/* Set input parameters */
	inputParams.SetParameters(ID, params);
}


void IntensityFeature::SetImage(Mat &img) {

	if (this->img.data != NULL)
		this->img.release();

	this->img = img;
}


void IntensityFeature::ExtractFeatures(int x0, int y0, int x1, int y1, SSFMatrix<FeatType> &m) {
int x, y, idx;
Vec3b color;
FeatType v;


	for (idx = 0, y = y0; y <= y1; y += 1) {
		for (x = x0; x <= x1; x += 1) {
			color = img.at<Vec3b>(y,x);
			v = ((FeatType) (color.val[0] + color.val[1] + color.val[2])) / (FeatType) 3.0;
			m(0,idx++) = v;
		}
	}
}


int IntensityFeature::GetNFeatures(SSFRect &rect) {

	if (rect.IsRectValid() == false)
		return  INTENSITY_FEATURES * img.rows * img.cols;

	return INTENSITY_FEATURES * rect.w * rect.h;
}


IntensityFeature *IntensityFeature::New() { 

	return new IntensityFeature(); 
}