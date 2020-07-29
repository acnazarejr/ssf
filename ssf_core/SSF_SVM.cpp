#include "SSF_SVM.h"

SSFMatrix<float> SSF_SVM::formatResponses(SSFMatrix<float> responses){
  SSFMatrix<float> formated;
  formated.create(responses.rows, this->labels.size());

  formated = 0.0f;

  for(int i =0; i < responses.rows; i++){
	  formated[i][(int)responses[0,i]] = 1;
  }
  return formated;
}

void SSF_SVM::convertLabelsToMat(SSFMatrix<FeatType> &Ylab){//TODO FeatTYpe or Integer?
	set<string>::iterator it;	
	int i;

	if (dataX.rows == 0){
		ReportError("Set data first");	
	}

	// initialize label matrix
	Ylab.create(nsamples, 1);

	float label = 1.0f;
	for (it = labels.begin(); it != labels.end(); it++) {

		/* ignore if class label is -1 (negative extra samples) */
		if (*it == EXTRA_CLASS){
			continue;
		}

		/* reset label vector (all set to negatives) */
		Ylab = -1.0f;

		/* set positive samples according to the sample IDs */
		for (i = 0; i < (int) dataY.size(); i++) {
			if (dataY[i] == *it)
				Ylab[i][0] = label;
		}
		label++;					
	}		
}

SSF_SVM::SSF_SVM(DataStartClassification dataStart) : ClassificationMethod(dataStart){
   
	inputParams.Add("svm_type",&svm_type,"The svm Type, (C_SVC, NU_SVC, ONE_CLASS, EPS_SVR, NU_SVR), more information at : http://docs.opencv.org/modules/ml/doc/support_vector_machines.html#cvsvmparams-cvsvmparams" ,false);
	inputParams.Add("kernelType",&kernelType," Choices are: (Linear, RBF, Polynomial,Sigmoid)",false);
	
	iteration_count = 0;
	inputParams.Add("iteration_count", &iteration_count, "The type of the termCriteria, more information at: http://docs.opencv.org/modules/core/doc/basic_structures.html#termcriteria", false);

	epsilon = 0;
	inputParams.Add("epsilon",&epsilon,"The minimum acuracy that is required as termination criteria, more information at: http://docs.opencv.org/modules/core/doc/basic_structures.html#termcriteria", false);

}


SSF_SVM::~SSF_SVM(void)
{
//TODO
}


void SSF_SVM::Setup(AllParameters *params) {					

	if(svm_type == "C_SVC"){			
		cvParams.svm_type = CvSVM::C_SVC;			
	}else if(svm_type == "NU_SVC"){
		cvParams.svm_type = CvSVM::NU_SVC;
	}else if(svm_type == "ONE_CLASS"){
		cvParams.svm_type = CvSVM::ONE_CLASS;
	}else if(svm_type == "EPS_SVR"){
		cvParams.svm_type = CvSVM::EPS_SVR;
	}else if(svm_type == "NU_SVR"){
		cvParams.svm_type = CvSVM::NU_SVR;
	}else{
		cvParams.svm_type = CvSVM::C_SVC;
	}

	
		
	if(kernelType == "LINEAR"|| kernelType == "Linear" || kernelType == "linear"){			
		cvParams.kernel_type = CvSVM::LINEAR;			
	}else if(kernelType == "POLYNOMIAL"|| kernelType == "Polynomial" || kernelType == "polynomial"){
		cvParams.kernel_type = CvSVM::POLY;
	}else if(kernelType == "RBF"|| kernelType == "rbf"){
		cvParams.kernel_type = CvSVM::RBF;
	}else if(kernelType == "SIGMOID" || kernelType == "Sigmoid" || kernelType == "sigmoid" ){
		cvParams.kernel_type = CvSVM::SIGMOID;
	}else{
		cvParams.kernel_type = CvSVM::LINEAR;			
	}
	int termCrit = 0;
	if(iteration_count > 0){
		termCrit = TermCriteria::COUNT;
	}
	if(epsilon > 0){
		termCrit += TermCriteria::EPS;			
	}

	cvParams.term_crit   = cvTermCriteria(termCrit, iteration_count, epsilon);
}

	// Create a new instance of this method
SSF_SVM *SSF_SVM::Instantiate(DataStartClassification dataStart) { 
	return new SSF_SVM(dataStart); 
}
	
	
	// Classify samples and set responses.
void SSF_SVM::Classify(const SSFMatrix<FeatType> &X, SSFMatrix<FeatType> &responses) { 
	ReportStatus("SVM Classification started");
	svmClassifier.predict(X, responses);
	responses = formatResponses(responses);
	ReportStatus("SVM Classification done");
}
	
	// learn the classifier
void SSF_SVM::Learn() { 
	ReportStatus("SVM Learning started");

	SSFMatrix<FeatType> labelsMat;
	convertLabelsToMat(labelsMat);

	svmClassifier.train(dataX, labelsMat, Mat(), Mat(), this->cvParams);	
	ReportStatus("SVM Learning done");
}

	// save classifier 
void SSF_SVM::Save(SSFStorage &storage) {
	if (storage.isOpened() == false){
		ReportError("Invalid file storage!");
	}
	svmClassifier.write(*storage, "SVM");	
	ReportStatus("SVM Classification Model saved to storage as %s",this->GetID());	
}
	
	// load classifier
void SSF_SVM::Load(const FileNode &node, SSFStorage &storage)  { 
	svmClassifier.clear();	

//	C++: void CvStatModel::read(CvFileStorage* storage, CvFileNode* node)

	FileNode n = node["SVM"];	
	svmClassifier.read(*storage, *n);	//TODO How should this be done
}	