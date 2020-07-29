#include "headers.h"
#include "ExtractionMethod.h"
#include "HistogramFeature.h"


HistogramFeature::HistogramFeature() {
	
	colorSpace = "BGR";
	nBins = 256;
	nChannel = 3;

	inputParams.Add("colorSpace", &colorSpace, "Color space chosen for creates the histogram ", true);
	inputParams.Add("nBins", &nBins, "Number of bins of the histogram ", true);
	inputParams.Add("normalization", &normalization, "Execute normalization", true);
}


HistogramFeature::~HistogramFeature() {

	if (img.data != NULL)
		img.release();
}


void HistogramFeature::Setup(string ID, AllParameters *params) {

	/* Set input parameters */
	inputParams.SetParameters(ID, params);

	// check if the input modules have been set correctly
	if (colorSpace != "RGB" && colorSpace != "Gray" && colorSpace != "HSV" && colorSpace != "XYZ" && colorSpace != "YrCrCb" && colorSpace != "HLS" && colorSpace != "Luv")
		ReportError("colorSpace must be either 'RGB', 'Gray', 'HSV', 'XYZ','YrCrCb','HLS', or 'Luv'");
	if (nBins < 0)
		ReportError("nBins must be greater than zero");
	if (normalization != "true" && normalization != "false")
		ReportError("Parameter 'normalization' must be set correctly");
}


void HistogramFeature::SetImage(Mat &img) {

	if (this->img.data != NULL)
		this->img.release();

	this->img = img;
}


void HistogramFeature::ExtractFeatures(int x0, int y0, int x1, int y1, SSFMatrix<FeatType> &m) {

	Mat src, dst, hist, auxR, auxG, auxB, b_hist, g_hist, r_hist, concatBG, concatBGR;
	vector<Mat> planes;

	// Color space conversion
	if(colorSpace == "RGB")
		cvtColor(img, img, CV_RGB2BGR);
	else if(colorSpace == "HSV")
		cvtColor(img, img, CV_BGR2HSV);
	else if(colorSpace == "XYZ")
		cvtColor(img, img, CV_BGR2XYZ);
	else if(colorSpace == "YrCrCb")
		cvtColor(img, img, CV_BGR2XYZ);
	else if(colorSpace == "HLS")
		cvtColor(img, img, CV_BGR2XYZ);
	else if(colorSpace == "Luv")
		cvtColor(img, img, CV_BGR2XYZ);
	else if(colorSpace == "Gray") {
		cvtColor(img, img, CV_BGR2GRAY);
		nChannel = 1;
	}

	// Setup a rectangle to define your region of interest
	Rect myROI(x0, y0, x1-x0, y1-y0);

	// Crop the full image to that image contained by the rectangle myROI
	src = img(myROI);

	// Separate the source image in its three R,G and B planes. 
	if(nChannel == 3) 
		split(src, planes);
	else
		planes.push_back(src);
	
	/// Set the ranges ( for B,G,R) )
	float range[] = {0, 256} ; //the upper boundary is exclusive
	const float* histRange = {range};

	// bins to have the same size (uniform) and to clear the histograms in the beginning
	bool uniform = true; 
	bool accumulate = false;

	// Compute the histograms:
	if(nChannel == 3) {
	
		calcHist(&planes[0], 1, 0, Mat(), b_hist, 1, &nBins, &histRange, uniform, accumulate);
		calcHist(&planes[1], 1, 0, Mat(), g_hist, 1, &nBins, &histRange, uniform, accumulate);
		calcHist(&planes[2], 1, 0, Mat(), r_hist, 1, &nBins, &histRange, uniform, accumulate);
	}
	else
		calcHist(&planes[0], 1, 0, Mat(), hist, 1, &nBins, &histRange, uniform, accumulate);

	// Normalize the result to [ 0, 1 ]
	if(normalization == "true") {

		if(nChannel == 3) {

			normalize(b_hist, b_hist, 0, 1, NORM_MINMAX, -1, Mat());
			normalize(g_hist, g_hist, 0, 1, NORM_MINMAX, -1, Mat());
			normalize(r_hist, r_hist, 0, 1, NORM_MINMAX, -1, Mat());
		}
		else if(nChannel == 1)
			normalize(hist, hist, 0, 1, NORM_MINMAX, -1, Mat());
	}

	if(nChannel == 3) {

		auxB = b_hist.reshape(0,1);
		auxG = g_hist.reshape(0,1);
		auxR = r_hist.reshape(0,1);

		// Matrix concat
		hconcat(auxB, auxG, concatBG);
		hconcat(concatBG, auxR, concatBGR);
		m = concatBGR;
	}
	else if(nChannel == 1)
		m = hist.reshape(0,1);
}

int HistogramFeature::GetNFeatures(SSFRect &rect) {

	return HISTOGRAM_FEATURES * nChannel * nBins;
}


HistogramFeature *HistogramFeature::New() { 

	return new HistogramFeature(); 
}