#include "headers.h"
#include "structures.h"
#include "SSFRect.h"


SSFRect::SSFRect() {

	x0 = -1;
	y0 = -1;
	w = -1;
	h = -1;
}

SSFRect::SSFRect(Rect cvRect){
	x0 = cvRect.x;
	y0 = cvRect.y;
	w = cvRect.width;
	h = cvRect.height;
}

SSFRect::SSFRect(int x, int y, int width, int height) {

	x0 = x;
	y0 = y;
	w = width;
	h = height;
}


SSFRect::SSFRect(const SSFRect &r) {

	x0 = r.x0;
	y0 = r.y0;
	w = r.w;
	h = r.h;
}

// return true if this rectangle has been set
bool SSFRect::IsRectValid() {

	if (x0 != -1 && y0 != -1 && w > 0 && h > 0)
		return true;

	return false;
}

void SSFRect::SetRect(int x0, int y0, int w, int h) {

	this->x0 = x0;
	this->y0 = y0;
	this->w = w;
	this->h = h;
}


Rect SSFRect::GetOpencvRect() {
	Rect rect;

	rect.x = this->x0;
	rect.y = this->y0;
	rect.width = this->w;
	rect.height = this->h;

	return rect;
}

void SSFRect::operator=(const SSFRect& r) {

	x0 = r.x0;
	y0 = r.y0;
	w = r.w;
	h = r.h;
}


void SSFRect::Save(SSFStorage &storage) {

	storage	<< "SSFRect" << "{";
	storage << "x0" << x0;
	storage << "y0" << y0;
	storage << "w" << w;
	storage << "h" << h;
	storage	<< "}";
}


void SSFRect::Load(const FileNode &node) {
	FileNode node_rect;

	node_rect = node["SSFRect"];

	node_rect["x0"] >> x0;
	node_rect["y0"] >> y0;
	node_rect["w"] >> w;
	node_rect["h"] >> h;
}