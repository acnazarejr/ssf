#include "headers.h"
#include "SSFCore.h"
#include "FESCache.h"
#include "FESMethodManager.h"
#include "FES.h"
#include "FESMethodInstance.h"


/*************************
 * class FESMethodInstance
 *************************/
FESMethodInstance::FESMethodInstance(string instanceID) : SSFMethodThread(instanceID) {
	
	this->extMetManager = NULL;
	this->cache = NULL;
	this->instanceID = -1;
}


void FESMethodInstance::SetExtractionManager(FESMethodManager *extMetManager) {
	this->extMetManager = extMetManager;
}


void FESMethodInstance::SetInstanceID(int instanceID) {

	this->instanceID = instanceID;
}


void FESMethodInstance::SetCache(FESCache *cache) {

	this->cache = cache;
}


/***************************
 * class FESMethodInstanceCPU
 ***************************/
FESMethodInstanceCPU::FESMethodInstanceCPU(string instanceID) : FESMethodInstance(instanceID) {

}








////
// class ExtractionFeatureInstanceCPU2D
////
FESMethodInstanceCPU2D instance("");

FESMethodInstanceCPU2D::FESMethodInstanceCPU2D(string instanceID) : FESMethodInstanceCPU(instanceID) {
	
	this->blocks = NULL;
	this->method = NULL;

	/* parameters */
	//inputParams.Add("methodManager", &methodManager, "Name of the internal module used as feature extraction manager", false);

	this->Register(this, instanceID);
}


SSFMethod *FESMethodInstanceCPU2D::Instantiate(string instanceID) {

	return new FESMethodInstanceCPU2D(instanceID); 
}


void FESMethodInstanceCPU2D::Setup() {


}



void FESMethodInstanceCPU2D::SetData(FESData *data, FESBlocks *blocks, FESRequestRegions *request) {
Mat img;

	// check data type
	if (data->GetDataType() != FES_DATATYPE_2D)
		ReportError("Invalid data type! It must be a 2D data");

	// retrieve image
	img = ((ExtractionData2D *) data)->GetImage();

	// set image
	((FeatureExtractionCPU2D *) method)->SetImage(img);

	if (blocks == NULL)
		ReportError("Blocks must be set!");

	if (blocks->GetDataType() != FES_DATATYPE_2D)
		ReportError("2D blocks must be used!");

	// set blocks
	this->blocks = blocks;

	// set current request
	this->currentRequest = request;
}



void FESMethodInstanceCPU2D::Launch() {

	this->Execute();
}



void FESMethodInstanceCPU2D::Execute() {
FESRegionWindow2D *region;
SSFMatrix<FeatType> m;
vector<SSFRect *> *allBlocks;
SSFRect *block;
SSFFeatures *features;
size_t signature, blockID;
SSFRect rect;
bool foundCache;

	if (method	== NULL)
		ReportError("Feature extraction method must be instantiated first!");

	// retrieve the signature for the current feature extraction
	signature = method->RetrieveSignature();
	glb_TP.Startx();
	while (1) {
		glb_TP.Startx();
		// retrieve the next request
		region = regions.pop();
		glb_TP.Stopx();

		// end of processing
		if (region == NULL)
			break;

		// retrieve feature
		features = region->RetrieveSingleExtraction(signature);
		
		// retrieve region
		rect = region->GetRegion();

		// run for each block
		glb_TP.Startx();
		allBlocks = ((FESBlocks2D *) blocks)->RetrieveSelectedBlocks();
		for (blockID = 0; blockID < allBlocks->size(); blockID++) {

			// retrieve current block
			block = allBlocks->at(blockID);

			// check if the feature for this block is in the cache
			if (cache != NULL) {
				foundCache = cache->GetData(rect.x0 + block->x0, rect.y0 + block->y0, block->w, block->h, m);
				if (foundCache == false) { // check if the feature is available in the cache
					// extract features
					//glb_TP.Startx();
					method->ExtractFeatures(rect.x0 + block->x0, 
						rect.y0 + block->y0, 
						rect.x0 + block->x0 + block->w - 1,
						rect.y0 + block->y0 + block->h - 1, 
						m);
					//glb_TP.Stopx();

					// set extracted feature to the cache
					//glb_TP.Startx();
					cache->SetData(rect.x0 + block->x0, rect.y0 + block->y0, block->w, block->h, m);
					//glb_TP.Stopx();
				}
			}

			// cache is not being used
			else {
				//glb_TP.Startx();
				method->ExtractFeatures(rect.x0 + block->x0, 
					rect.y0 + block->y0, 
					rect.x0 + block->x0 + block->w - 1,
					rect.y0 + block->y0 + block->h - 1, 
					m);
				//glb_TP.Stopx();
			}

			// set the feature to the returning matrix
			features->AddFeatures(m);
		}
		glb_TP.Stopx();

		region->SetExtractedOneMethod();

		// set response
		currentRequest->SetProcessedRegion(region);

		// inform that this element has been processed
		extMetManager->SetRequestProcessed(this->instanceID);
	}
	glb_TP.Stopx();
}



size_t FESMethodInstanceCPU2D::GetExtractionSignature() {

	return method->RetrieveSignature();
}



void FESMethodInstanceCPU2D::SetRequest(FESRegion *region) {

	regions.push((FESRegionWindow2D *) region);
}


void FESMethodInstanceCPU::RequestFinish() {

	// set a NULL request
	this->SetRequest(NULL);
}





////
// class ExtractionFeatureInstanceGPU2D
////
void FESMethodInstanceGPU2D::ExtractFeatures(FESRequest *request) {


}



