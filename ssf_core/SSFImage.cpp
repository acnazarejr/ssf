#include "headers.h"
#include "structures.h"
#include "SSFImage.h"

SSFImage::SSFImage() {

	threadlock.Lock();
	this->SetDataType(SSF_DATA_IMAGE);
	threadlock.Unlock();
}

SSFImage::SSFImage(Mat image) {

	threadlock.Lock();
	this->image = image.clone();
	this->SetDataType(SSF_DATA_IMAGE);
	threadlock.Unlock();
}


SSFImage::SSFImage(string filename) {


	threadlock.Lock();
	this->SetDataType(SSF_DATA_IMAGE);
	image = imread(filename, CV_LOAD_IMAGE_COLOR); 
	if (image.data == NULL)
		ReportError("Image '%s' could not be loaded!", filename.c_str());
	threadlock.Unlock();
}

SSFImage::SSFImage(const FileNode &node) {

	threadlock.Lock();
	this->SetDataType(SSF_DATA_IMAGE);
	this->Load(node);
	threadlock.Unlock();
}


SSFImage::~SSFImage() {

	threadlock.Lock();
	image.release();
	threadlock.Unlock();
}


SSFImage *SSFImage::RetrievePatch(SSFRect window) {
SSFImage *cropped;
Mat croppedImage;	
Rect roi;

	// if the detection window is invalid (not yet set), patch will be a copy of the image
	if (window.IsRectValid() == false) {
		threadlock.Lock();
		croppedImage = image.clone();
		threadlock.Unlock();
	}
	else {
		roi.x = window.x0;
		roi.y = window.y0;
		roi.height = window.h;
		roi.width = window.w;

		threadlock.Lock();
		Mat(image, roi).copyTo(croppedImage);
		threadlock.Unlock();
	}

	cropped = new SSFImage(croppedImage);

	return cropped;
}

SSFImage *SSFImage::RetrieveResizedPatch(SSFRect window, int width, int height, int interpolationMethod) {
	return RetrieveResizedPatch(window, Size(width, height), 0.0, 0.0, interpolationMethod);
}

SSFImage *SSFImage::RetrieveResizedPatch(SSFRect window, double fx, double fy, int interpolationMethod) {
	return RetrieveResizedPatch(window, Size(), fx, fy, interpolationMethod);
}

SSFImage *SSFImage::RetrieveResizedPatch(SSFRect window, Size size, double fx, double fy, int interpolationMethod) {
SSFImage *cropped;
Mat croppedImage;	
Rect roi;

	// if the detection window is invalid (not yet set), patch will be a copy of the image
	if (window.IsRectValid() == false) {
		threadlock.Lock();
		croppedImage = image.clone();
		threadlock.Unlock();
	}
	else {
		roi.x = window.x0;
		roi.y = window.y0;
		roi.height = window.h;
		roi.width = window.w;

		threadlock.Lock();
		Mat(image, roi).copyTo(croppedImage);
		threadlock.Unlock();
	}

	cv::resize(croppedImage, croppedImage, size, fx, fy, interpolationMethod);
	cropped = new SSFImage(croppedImage);

	return cropped;
}

SSFImage *SSFImage::RetrieveResized(int width, int height, int interpolationMethod) {
SSFImage *resizedClone;
Mat clonedImage;	

    threadlock.Lock();
    clonedImage = image.clone();
    threadlock.Unlock();

	cv::resize(clonedImage, clonedImage, Size(width,height), 0.0, 0.0, interpolationMethod);
	resizedClone = new SSFImage(clonedImage);

	return resizedClone;
}

void SSFImage::Save(string filename) {

	threadlock.Lock();
	imwrite(filename, image);
	threadlock.Unlock();
}

void SSFImage::Save(SSFStorage &storage) {
vector<uchar> buff;
vector<int> param;

	// set image parameters
	param.push_back(CV_IMWRITE_PNG_COMPRESSION);  // file format
	param.push_back(3);	// default(3)  0-9. A higher value means a smaller size and longer compression time
	
	if (imencode(".png", image, buff, param) == false)
		ReportError("Image could not be coded!");

	storage << "SSFImage" << "{";
	storage << "format" << "PNG";
	storage << "param" << param;
	storage << "img"  << buff;
	storage << "}";
}



void SSFImage::Load(const FileNode &node) {
vector<uchar> buff;
FileNode n;
string format;

	image.release();

	n = node["SSFImage"];
	n["format"] >> format;

	if (format != "PNG")
		ReportError("Invalid image format '%s'", format.c_str());

	n["img"] >> buff;

	imdecode( buff, CV_LOAD_IMAGE_COLOR, &image);
}


Mat SSFImage::RetrieveCopy() {
Mat dataCopy;

	threadlock.Lock();
	dataCopy = image.clone();
	threadlock.Unlock();

	return dataCopy;
}




int SSFImage::GetNCols() {
int cols;

	threadlock.Lock();
	cols = image.cols;
	threadlock.Unlock();
	
	return cols;
}


int SSFImage::GetNRows() {
int rows;

	threadlock.Lock();
	rows = image.rows;
	threadlock.Unlock();
	
	return rows;
}



