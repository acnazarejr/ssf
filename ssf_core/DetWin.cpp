#include "headers.h"
#include "structures.h"
#include "DetWin.h"



DetWin::DetWin() {

	x0 = -1;
	y0 = -1;
	w = -1;
	h = -1;
	response = -1.0f;
}

DetWin::DetWin(int x, int y, int width, int height, float response) {

	x0 = x;
	y0 = y;
	w = width;
	h = height;
	this->response = response;
}
    

DetWin::DetWin(float response, int x0, int y0, int x1, int y1) {

	x0 = x0;
	y0 = y0;
	w = x1 - x0 + 1;
	h = y1 - y0 + 1;
	this->response = response;
}

DetWin::DetWin(const DetWin &d) {

	x0 = d.x0;
	y0 = d.y0;
	w = d.w;
	h = d.h;
	response = d.response;
}



DetWin::DetWin(const CvRect &r, float response) {

	x0 = r.x;
	y0 = r.y;
	w = r.width;
	h = r.height;
	this->response = response;
}


CvRect DetWin::GetDetWin() {
CvRect r;

	r.x = x0;
	r.y = y0;
	r.height = h;
	r.width = w;

	return r;
}


SSFRect DetWin::GetSSFRect() {
SSFRect rect;

	rect.SetRect(x0, y0, w, h);

	return rect;
}

bool DetWin::IsValidDetWin() {

	if (x0 != -1 && y0 != -1 && w > 0 && h > 0)
		return true;

	return false;
}



void DetWin::Save(SSFStorage &storage) {

	storage	<< "{:";
	storage << "x0" << x0;
	storage << "y0" << y0;
	storage << "w" << w;
	storage << "h" << h;
	storage << "respose" << response;
	storage	<< "}";
}


void DetWin::Load(const FileNode &node) {
FileNode node_det;

	//node_det = node["DetWin"];

	node["x0"] >> x0;
	node["y0"] >> y0;
	node["w"] >> w;
	node["h"] >> h;
	node["respose"] >> response;
}