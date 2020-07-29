#ifndef _FES_HOG_H_
#define _FES_HOG_H_



class FESHOG :	public FeatureExtractionCPU2D {

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

	// get number of features for this method
	int GetNFeatures();//ncell*nbins

	// return current version of the method
	string GetVersion() { return "0.0.1"; }

public:
	FESHOG(string instanceID);
	~FESHOG();

	// Return the name of the module
	string GetName() { return SSF_METHOD_FES_FEATURE_HOG; }

	// retrieve the type of the method
	string GetType() { return SSF_METHOD_TYPE_FEATURE; }

	// create a new instance of this method
	FESHOG* Instantiate(string instanceID);

	// local setup
	void Setup();

	// set image
	void SetImage(Mat &img);

	// extract features without selection
	void ExtractFeatures(int x0, int y0, int x1, int y1, SSFMatrix<FeatType> &m);
};

#endif