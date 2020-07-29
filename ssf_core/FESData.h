#ifndef _FES_DATA_H_
#define _FES_DATA_H_


// image or sequence of images
class FESData {

public:
	virtual int GetDataType() = 0;
};


class ExtractionData2D : public FESData {
Mat img;

public:
	int GetDataType() { return FES_DATATYPE_2D; }

	Mat GetImage();

	void SetImage(Mat &img) { this->img = img; }

	int GetWidth() { return img.cols; }
};


class ExtractionDataTemporal : public FESData {

public:
	int GetDataType() { return FES_DATATYPE_TEMPORAL; }
};


#endif