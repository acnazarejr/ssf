/*====================================================================================
* SSF wrapper for OpenCV SVM
*author: Ricardo Kloss
*contact: rbk@dcc.ufmg.br
======================================================================================*/
#ifndef _SSF_SVM_H_
#define _SSF_SVM_H_
#include "classificationmethod.h"
#include "ClassificationDefs.h"
#include "ClassificationControl.h"

class SSF_SVM :	public ClassificationMethod{
protected:
	cv::SVM svmClassifier;
	CvSVMParams cvParams;
	string svm_type;
	string kernelType;
	int termination_criteria;
	int epsilon;
	int iteration_count;	

	void SSF_SVM::convertLabelsToMat(SSFMatrix<FeatType> &Ylab);//Ylab is the matrix to receive the numerical labels

	SSFMatrix<float> SSF_SVM::formatResponses(SSFMatrix<float> responses);
			
public:
	SSF_SVM(DataStartClassification dataStart);
	~SSF_SVM(void);
	
	

	// Execute setup for the method
	void Setup(AllParameters *params);

	// Create a new instance of this method
	SSF_SVM * Instantiate(DataStartClassification dataStart);
	
	// Initialize the classifier
	void Initialize() { }

	// Classify samples and set responses.
	void Classify(const SSFMatrix<FeatType> &X, SSFMatrix<FeatType> &responses);

	// learn the classifier
	void Learn();

	// save classifier 
	void Save(SSFStorage &storage);

	// load classifier
	void Load(const FileNode &node, SSFStorage &storage) ;

	// return ID of the classification method 
	string GetName() { return CLASSIFIER_ID_SVM; }
};
#endif
