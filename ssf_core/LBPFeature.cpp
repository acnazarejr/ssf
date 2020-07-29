#include "headers.h"
#include "misc.h"
#include "ExtractionMethod.h"
#include "LBPFeature.h"


LBPFeature::LBPFeature() :  ExtractionMethod() {

}

LBPFeature::~LBPFeature() {

	if (img.data != NULL)
		img.release();
}

void LBPFeature::Setup(string ID, AllParameters *params) {

	/* Set input parameters */
	inputParams.SetParameters(ID, params);
}



void LBPFeature::SetImage(Mat &img) {

	if (this->img.data != NULL)
		this->img.release();

	this->img = img;
}




int LBPFeature::GetNFeatures(SSFRect &rect) {

	return LBP_FEATURES;
}



void LBPFeature::ExtractFeatures(int x0, int y0, int x1, int y1, SSFMatrix<FeatType> &m) {
int x,y,w0,h0,n=0;
int *result=(int*)malloc(256*sizeof(int));
int *imgvalue; 
w0 = x1 - x0 + 1;
h0 = y1 - y0 + 1;
Vec3b color;


	imgvalue = (int*)malloc(w0 * h0 * sizeof(int));

	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++) {
			color = img.at<Vec3b>(y,x);
            imgvalue[n++] = color.val[0]; //image array to a vector
		}
	}

	result = lbp_histogram(imgvalue, w0, h0, result, 0);

    for (int i = 0; i < 256; i++)
	{
		m(0,i) = (FeatType) result[i];
	}

	free(result);
	free(imgvalue);
}


LBPFeature *LBPFeature::New() { 

	return new LBPFeature(); 
}