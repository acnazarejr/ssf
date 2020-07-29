#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "FES.h"
#include "ModPLSDetectorFeat.h"


ModPLSDetectorFeat instance("");

/******
 * class ModPLSDetectorFeat
 ******/
ModPLSDetectorFeat::ModPLSDetectorFeat(string modID) :  SSFUserModule(modID) {

	executionType = "testing";

	/* Add parameters */
	inputParams.Add("modelFile", &modelFile, "File storing the model to save or to load", true);
	inputParams.Add("executionType", &executionType, "Execute type (learning or testing)", true);
	inputParams.Add("featFile", &featFile, "File containing the feature setup for detection (only for learning)", false);
	inputParams.Add("featSetup", &featSetup, "Feature setup that will be considered (must be defined in file featFile", false);

	/* module information */
	info.Description("Module to evaluate FES.");

	/* set input and output data */
	modParams.Require(modID, SSF_DATA_SLDWINDOWS, &posInputMod, "positive");	// sliding windows for positive samples
	modParams.Require(modID, SSF_DATA_SLDWINDOWS, &negInputMod, "negative");	// sliding windows for negative samples
	modParams.Require(modID, SSF_DATA_SLDWINDOWS, &testInputMod, "test");		// sliding windows for test images
	//modParams.Require(modID, SSF_DATA_USER, &trainModelMod, "ModelPLSDetector");// PLS model generated on the fly

	modParams.Provide(modID, SSF_DATA_SLDWINDOWS);

	/* register the module */
	this->Register(this, modID);
}

ModPLSDetectorFeat::~ModPLSDetectorFeat() {


}



void ModPLSDetectorFeat::Setup() {
	
	// check if the input modules have been set correctly
	if (executionType != "learning" && executionType != "testing" && executionType != "features")
		ReportError("executionType must be either 'learning' or 'testing' or 'features'");

	else if (executionType == "learning" && (posInputMod == "" || negInputMod == ""))
		ReportError("posInputMod and negInputMod must be set for learning");

	else if ((executionType == "testing" || executionType == "features") && testInputMod == "")
		ReportError("testInputMod must be set for testing");

	if ((executionType == "testing" || executionType == "features") && modelFile == "" && trainModelMod == "") 
		ReportError("trainModelMod has to be set to read PLS model build on-the-fly (when modelFile has not been set)");


	// check if the parameters are set correctly
	// learning
	if (executionType == "learning") {
		if (featFile == "")
			ReportError("Parameter 'featFile' has to be set");
		if (featSetup == "")
			ReportError("Parameter 'featSetup' has to be set");
	}

	// testing
	else {
		if (featFile == "")
			ReportError("Parameter 'featFile' has to be set");
		if (featSetup == "")
			ReportError("Parameter 'featSetup' has to be set");
	}
}





SSFMethod *ModPLSDetectorFeat::Instantiate(string modID) {

	return new ModPLSDetectorFeat(modID); 
}




void ModPLSDetectorFeat::Features() {
size_t signature;
ExtractionData2D *data;
FESRegionWindow2D *region;
FESRequestRegions *request;
size_t frameID;
SSFSlidingWindow *sldWins;
SSFDetwins *ssfDetwins;
FESBlocks2D *blocks;
size_t scaleID;
size_t position = 0;
SSFRect rect;
vector<DetWin> *detwins;
size_t i;

	signature = glb_sharedMem.FES_InitializeExtractionMethod("params.yml", "feat2");

	data = new ExtractionData2D();
	//data->SetImage(img.RetrieveCopy());

	// set blocks
	FeatBlockInfo blockDef;
	blockDef.strideX = 8;
	blockDef.strideY = 8;
	blockDef.block.w = 16;
	blockDef.block.h = 16;
	blockDef.block.x0 = 0;
	blockDef.block.y0 = 0;
	SSFRect window;
	window.h = 128;
	window.w = 64;
	window.x0 = 0;
	window.y0 = 0;
	blocks = new FESBlocks2D();
	//blocks->AddBlock(0, 0, 8, 8);
	//blocks->AddBlock(8, 8, 8, 8);
	//void CreateBlockList(SSFRect window, FeatBlockInfo blockDef);
	blocks->CreateBlockList(window, blockDef);

	// extract features for the positive samples	
	while ((frameID = glb_sharedMem.RetrieveFrameIDbyPosition(testInputMod, SSF_DATA_SLDWINDOWS, position++)) != SM_INVALID_ELEMENT) {

		// set sliding windows
		//outputSldWins = new SSFSlidingWindow();

		// retrieve sliding windows for frameID
		sldWins = glb_sharedMem.GetSlidingWindows(testInputMod, frameID);

		// load images for the input sliding windows
		sldWins->LoadImages(glb_sharedMem.GetFrameImage(frameID));

		ReportStatus("[ID frame: %d] Detecting for %d scales and %d detection windows)", frameID, sldWins->GetNEntries(), sldWins->GetNumberofWindows());
		glb_TP.Startx();
		// run for each scale
		for (scaleID = 0; scaleID < sldWins->GetNEntries(); scaleID++) {

			// new request
			request = new FESRequestRegions();

			// set image for this scale
			data->SetImage(sldWins->GetSSFImage(scaleID)->RetrieveCopy());

			// add data to request
			request->AddData(data, signature);

			// set blocks
			request->SetBlocks(blocks);

			// set request to compute
			glb_sharedMem.FES_SetRequest(request);

			// retrieve sliding windows of this scale
			ssfDetwins = sldWins->GetSSFDetwins(scaleID);

			// extract features for each detection window
			detwins = ssfDetwins->GetDetwins();

			
			// run for each detection window
			for (i = 0; i < detwins->size(); i++) {
				glb_TP.Startx();
				// retrieve detection window
				rect = detwins->at(i).GetSSFRect();

				// creat region
				region = new FESRegionWindow2D();

				// set region
				region->SetRegion(rect);
				request->AddRegion(region);
				glb_TP.Stopx();
			}
			

			// end of requests
			request->EndRequests();

			// wait for the responses
			glb_TP.Startx();
			while (1) {
				region = (FESRegionWindow2D *) request->RetrieveExtractedFeatures();
				if (region == NULL)
					break;
				delete region;
			}
			glb_TP.Stopx();

			//delete request;

			ReportStatus("Scale %d/%d", scaleID, sldWins->GetNEntries());
		}
		glb_TP.Stopx();

		// release the images for the input sliding windws
		sldWins->ReleaseImages();
	}
}	


void ModPLSDetectorFeat::Execute() {

	if (executionType == "features")
		Features();

	// invalid execution type
	else 
		ReportError("Invalid executionType '%s'", executionType.c_str());
}
