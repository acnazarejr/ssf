#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
//#include "SSFMatrices.h"
#include "ModDetectionEvaluation.h"


ModDetectionEvaluation instance("");

/******
 * class DetectionEvaluation
 ******/
ModDetectionEvaluation::ModDetectionEvaluation(string modID) :  SSFUserModule(modID) {

	/* parameters */
	inputParams.Add("metric", &metric, "Metric used in the evaluation (FPPI or FPPW). FPPI requires ground truth and FPPW requires negative and positive inputs", true);
	inputParams.Add("outFile", &outFile, "Output file where the results will be stored (it will have yml format)", true);

	/* module information */
	info.Description("Module to evaluate detection performance.");
	info.Author("William Robson Schwartz", "william@dcc.ufmg.br");
	info.Date(9, 2, 2014);

	/* set input and output data */
	modParams.Require(modID, SSF_DATA_SLDWINDOWS, &inModSldWinPos, "positive");	// input being sliding windows (positive results)
	modParams.Require(modID, SSF_DATA_SLDWINDOWS, &inModSldWinNeg, "negative");	// input being sliding windows (negative results)
	//modParams.Require(modID, SSF_DATA_MATRICES, &inModMatricesPos, "positive");	// input being matrices (positive results)
	//modParams.Require(modID, SSF_DATA_MATRICES, &inModMatricesNeg, "negative");	// input being matrices (negative results)
	// still need to add the ground truth as input in case of the FPPI

	/* register the module */
	this->Register(this, modID);
}


SSFMethod *ModDetectionEvaluation::Instantiate(string modID) {

	return new ModDetectionEvaluation(modID); 
}




void ModDetectionEvaluation::Setup() {

	if (metric != "FPPI" && metric != "FPPW")
		ReportError("Metric to be evaluated must be either 'FPPI' or 'FPPW'");

	else if (metric == "FPPW" && (inModMatricesPos == "" || inModMatricesNeg == ""))
		ReportError("Must set positive (inModMatricesPos) and negative (inModMatricesNeg) matrices for metric 'FPPW'");
}




void ModDetectionEvaluation::EvaluateFPPW(vector<FeatType> &posResp, vector<FeatType> &negResp) {
vector<FeatType> missRate, FPPW;
SSFStorage storage;
FeatType currTh;
size_t i, j = 0;


	// sort both vectors
	sort(posResp.begin(), posResp.end());
	sort(negResp.begin(), negResp.end());

	for (i = 0; i < posResp.size(); i++) {
		currTh = posResp[i];
		missRate.push_back((FeatType) i / (FeatType) posResp.size());

		while (j < negResp.size() && negResp[j++] <= currTh );
		FPPW.push_back(((FeatType) (negResp.size() - (j-1))) / (FeatType) negResp.size());
	}

	storage.open(outFile, SSF_STORAGE_WRITE);
	storage << "missRate" << missRate;
	storage << "FPPW" << FPPW;
	storage << "posResp" << posResp;
	storage << "negResp" << negResp;
	storage.release();
}



void ModDetectionEvaluation::FPPW() {
#if 0
SSFMatrices *matrices;
vector<SSFMatrix<FeatType> *> vmatrices;
SSFMatrix<FeatType> *mat;
vector<FeatType> posResp, negResp;		// store responses
SMIndexType position;

	// get negative results
	position = 0;
	while ((matrices = (SSFMatrices *) glb_sharedMem.GetData(inModMatricesNeg, SSF_DATA_MATRICES, position++)) != NULL) {

		vmatrices = matrices->GetMatrices();
		if (vmatrices.size() != 1 || vmatrices[0]->cols != 1 || vmatrices[0]->rows != 1)
			ReportError("The number of results must be 1 in a matrix 1x1");

		mat = vmatrices[0];
		negResp.push_back((*mat)(0,0));

		// unlink the data
		glb_sharedMem.UnlinkDataItem(matrices);
	}

	// get positive results
	position = 0;
	while ((matrices = (SSFMatrices *) glb_sharedMem.GetData(inModMatricesPos, SSF_DATA_MATRICES, position++)) != NULL) {

		vmatrices = matrices->GetMatrices();
		if (vmatrices.size() != 1 || vmatrices[0]->cols != 1 || vmatrices[0]->rows != 1)
			ReportError("The number of results must be 1 in a matrix 1x1");

		mat = vmatrices[0];
		posResp.push_back((*mat)(0,0));

		// unlink the data
		glb_sharedMem.UnlinkDataItem(matrices);
	}
#endif
}




void ModDetectionEvaluation::Execute() {

	if (metric == "FPPW") {
		ReportStatus("Evaluating Miss rate vs. FPPW for detection");
		this->FPPW();
	}

	else {
		ReportError("Metric '%s' is not implemented", metric.c_str()); 
	}
}