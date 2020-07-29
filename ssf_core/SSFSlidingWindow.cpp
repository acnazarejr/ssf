#include "headers.h"
#include "structures.h"
#include "SSFSlidingWindow.h"




/**************************
 * class SSFSlidingWindow *
 **************************/
SSFSlidingWindow::SSFSlidingWindow() {

	mutex.Lock();
	this->SetDataType(SSF_DATA_SLDWINDOWS);
	this->naccess = 0;
	mutex.Unlock();
}


SSFSlidingWindow::~SSFSlidingWindow() {
size_t i;

	mutex.Lock();
	for (i = 0; i < detwins.size(); i++)
		delete detwins[i];

	for (i = 0; i < scaleInfo.size(); i++) 
		delete scaleInfo[i];

	for (i = 0; i < imgs.size(); i++)
		delete imgs[i];
	mutex.Unlock();
}

void SSFSlidingWindow::AddDetwins(SSFDetwins *detwins) {

	this->AddDetwins(detwins, NULL);	
}

void SSFSlidingWindow::AddDetwins(SSFDetwins *detwins, ScaleInfo *scaleInfo) {

	mutex.Lock();
	this->detwins.push_back(detwins);
	this->scaleInfo.push_back(scaleInfo);
	mutex.Unlock();
}

size_t SSFSlidingWindow::GetNEntries() {
size_t n;

	mutex.Lock();
	n = this->detwins.size();
	mutex.Unlock();	

	return n;
}

SSFDetwins *SSFSlidingWindow::GetSSFDetwins(size_t idx) {
SSFDetwins *det;

	mutex.Lock();
	if (idx >= detwins.size())
		ReportError("Invalid index '%d' (of %d)", idx, detwins.size());

	det = detwins[idx];
	mutex.Unlock();	

	return det;
}


ScaleInfo *SSFSlidingWindow::GetScaleInfo(size_t idx) {
ScaleInfo *info;

	mutex.Lock();
	if (idx >= scaleInfo.size())
		ReportError("Invalid index '%d' (of %d)", idx, scaleInfo.size());

	info = scaleInfo[idx];
	mutex.Unlock();	

	return info;
}


vector<DetWin> *SSFSlidingWindow::GetAllDetWins() {
vector<DetWin> *v, *v1;
size_t i;

	v = new vector<DetWin>();
	mutex.Lock();
	for (i = 0; i < detwins.size(); i++) {
		v1 = detwins[i]->GetDetwins();
		v->insert(v->end(), v1->begin(), v1->end());
	}
	mutex.Unlock();

	return v;
}

SSFImage *SSFSlidingWindow::GetSSFImage(size_t idx) {
SSFImage *img;

	mutex.Lock();
	if (imgs.size() == 0) {
		ReportError("Call LoadImage() before calling GetSSFImage().");
		return NULL;
	}
	else if (idx >= imgs.size()) {
		ReportError("Invalid index '%d' (of %d)", idx, imgs.size());
	}

	img = imgs[idx];
	mutex.Unlock();	

	return img;
}


size_t SSFSlidingWindow::GetNumberofWindows() {
size_t i, total = 0;

	mutex.Lock();
	for (i = 0; i < detwins.size(); i++)
		total += detwins[i]->GetNumberofDetWins();
	mutex.Unlock();

	return total;
}



void SSFSlidingWindow::ReleaseImages() {
SSFImage *img;
size_t i;

	mutex.Lock();
	if (naccess > 1) {
		naccess--;
		mutex.Unlock();
		return;
	}
	for (i = 0; i < imgs.size(); i++) {
		img = imgs[i];
		delete img;
	}
	imgs.clear();
	naccess--;
	mutex.Unlock();
}


void SSFSlidingWindow::LoadImages(SSFImage *img) {
Mat data = img->RetrieveCopy();


	mutex.Lock();
	if (naccess >= 1) {
		naccess++;
		data.release();
		mutex.Unlock();
		return;
	}
	for (size_t i = 0; i < this->scaleInfo.size(); i++) {
		ScaleInfo *currScaleInfo = this->scaleInfo[i];

		int imgW = (int) floor((float) data.cols * currScaleInfo->scale); 
		int imgH = (int) floor((float) data.rows * currScaleInfo->scale);

		// rescale the image
		Mat newData(imgH, imgW, CV_8UC3);
		resize(data, newData, newData.size());

		// adds detwins of all scales to SSFSlidingWindows
		SSFImage *scaledImg = new SSFImage(newData); // SSFImage makes a clone of newData

		imgs.push_back(scaledImg);
	}
	naccess++;

	data.release();
	mutex.Unlock();
}
