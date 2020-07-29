#ifndef _HOG_H_
#define _HOG_H_

#include "headers.h"
#include "ExtractionMethod.h"


#define HOG_ID "HOG"

class HOG :	public ExtractionMethod{

	/*HOG:
	*We calculate the gradient for the whole image
	*for each block 
	**for each cell
	***extract descriptor
	**concat cells descriptors
	*/

	string input_module_positive;
	string input_module_negative;

	int nbins;	
	int cell_rows;
	int cell_cols;
	int ncells;
	float clipping;	
	string normalization_type;

	SSFMatrix<FeatType> *integral_mat;

	void computeGradient(Mat inp, SSFMatrix<FeatType> *gradient_direction);


public:
	HOG();
	~HOG();

	// Execute setup for the method
	void Setup(string ID, AllParameters *params);

	// set image
	void SetImage(Mat &img);

	// extract features without selection
	void ExtractFeatures(int x0, int y0, int x1, int y1, SSFMatrix<FeatType> &m);

	// get number of features for this method
	int GetNFeatures(SSFRect &rect);//ncell*nbins

	// get feature unique identifier
	string GetID() { return HOG_ID; }//TODO return HOG_ID

	// create a new instance of this method
	virtual HOG* New();
};

#endif