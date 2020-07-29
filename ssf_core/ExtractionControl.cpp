#include "headers.h"
#include "parameters.h"
#include "ExtractionMethod.h"
#include "FeatureMethods.h"
#include "FeatureBlocks.h"
#include "ExtractionControl.h"
#include "SSFImage.h"
#include "SSFFeatures.h"


/*******
 * class FeatureIndex 
 *******/
void FeatureIndex::SetMap(string featSetupName, size_t ID) {

	setupMap.insert(pair<string, size_t>(featSetupName, ID));
}


size_t FeatureIndex::GetID(string featSetupName) {
unordered_map<string, size_t>::iterator it;

	it = setupMap.find(featSetupName);

	if (it == setupMap.end())
		ReportError("Feature setup name '%s' does not exist", featSetupName.c_str());
	
	return it->second;
}


size_t FeatureIndex::operator[](string featSetupName) {

	return GetID(featSetupName);
}


vector<size_t> FeatureIndex::GetAllIDs() {
unordered_map<string, size_t>::iterator it;
vector<size_t> v;

	for (it = setupMap.begin(); it != setupMap.end(); it++) {
		v.push_back(it->second);
	}

	return v;
}


/****
 * class FeatureExtraction
 ****/
FeatureExtractionSetup::FeatureExtractionSetup(ExtractionMethod *extMethod, FeatBlockDef *blockDef) {

	this->tmpData = NULL;
	this->extMethod = extMethod;
	this->defaultBlockDef = blockDef;
	this->dataTmp.create(0, 0);
}


FeatureExtractionSetup::~FeatureExtractionSetup() {

	delete tmpData;
	delete extMethod;
}



void FeatureExtractionSetup::SetImage(SSFImage *img) {

	extMethod->SetImage(img->RetrieveCopy());
}



void FeatureExtractionSetup::SetImage(vector<SSFImage*> &imgs) {
vector<vector<Mat> > data;
size_t i;

	for (i = 0; i < imgs.size(); i++) {
		data.push_back(imgs[i]->RetrieveCopy());
	}
}




void FeatureExtractionSetup::ExtractFeatures(SSFRect &window, FeatureBlocks *blocks, SSFFeatures &ssfFeats) {
vector<SSFRect *> *blockSelected;
SSFRect *blocksel;
int nfeats;

	// if window is not set, set it to the image size
	if (window.IsRectValid() == false) {
		ReportError("Rectangle window must be set prior to feature extraction");
	}

	// extract features for all blocks necessary
	blockSelected = blocks->RetrieveSelectedBlocks();

	// number of features per block (must be the same number for every block)
	nfeats = extMethod->GetNFeatures(*blockSelected->at(0));

	// adjust the vector size for each block (if already set, will not allocate again)
	dataTmp.create(1, nfeats);

	// reset final feature matrix
	ssfFeats.Reset();

	// extract features
	for (size_t i = 0; i < blockSelected->size(); i++) {
		// extract feature
		blocksel = blockSelected->at(i);
		extMethod->ExtractFeatures(window.x0 + blocksel->x0, 
			window.y0 + blocksel->y0, 
			window.x0 + blocksel->x0 + blocksel->w - 1, 
			window.y0 + blocksel->y0 + blocksel->h - 1,
			dataTmp);

		// set features to the final matrix
		ssfFeats.AddFeatures(dataTmp);
	}
}






void FeatureExtractionSetup::ExtractFeatures(SSFRect &window, SSFFeatures &ssfFeats) {
vector<FeatBlockInfo> defs;		// definition of blocks
FeatBlockInfo blockInfo;		// information regarding one block size
FeatBlockDef *blockDef;
int x, y, nfeats;
size_t i;

	// set the block definition to use the default provided by the feature parameters
	blockDef = this->defaultBlockDef;
	
	// if window is not set, set it to the image size
	if (window.IsRectValid() == false) {
		ReportError("Rectangle window must be set prior to feature extraction");
	}

	// clean the output feature vector
	ssfFeats.Reset();

	// extract feature for the entire image at once (without considering blocks)
	if (blockDef == NULL) {

		// retrieve the number of features 
		nfeats = extMethod->GetNFeatures(window);

		// allocate memory
		dataTmp.create(1, nfeats);

		// extract features
		extMethod->ExtractFeatures(window.x0, window.y0, window.w - 1, window.h - 1, dataTmp); 

		// set feature
		ssfFeats.AddFeatures(dataTmp);
	}
	else { // extract features according to the block definition (block size and strides in x and y)

		// retrieve block definitions
		defs = blockDef->RetrieveBlockDefinitions();

		// retrieve number of features
		nfeats = extMethod->GetNFeatures(defs[0].block);

		// set tmp matrix (it won't allocate memory if it already has the correct size)
		dataTmp.create(1, nfeats);

		for (i = 0; i < defs.size(); i++) {
			blockInfo = defs[i];
			for (y = window.y0; y <= window.h + window.y0 - blockInfo.block.h; y += blockInfo.strideY) {
				for (x = window.x0; x <= window.w + window.x0 - blockInfo.block.w; x += blockInfo.strideX) {

					// extract features
					extMethod->ExtractFeatures(x, y, x + blockInfo.block.w - 1, y + blockInfo.block.h - 1, dataTmp); 

					// set feature
					ssfFeats.AddFeatures(dataTmp);
				}
			}
		}
	}
}



FeatBlockDef *FeatureExtractionSetup::RetrieveBlockDefs() {

	return defaultBlockDef;
}



FeatureBlocks *FeatureExtractionSetup::RetriveFeatureBlocks(SSFRect &window) {
vector<FeatBlockInfo> defs;		// definition of blocks
FeatBlockInfo blockInfo;		// information regarding one block size
FeatBlockDef *blockDef;
FeatureBlocks *blocks; 
size_t i;

	// set the block definition to use the default provided by the feature parameters
	blockDef = this->defaultBlockDef;

	// check if there is a block definition
	if (blockDef == NULL)
		ReportError("Block definition must be set");

	// retrieve block definitions
	defs = blockDef->RetrieveBlockDefinitions();

	// if window is not set, set it to the image size
	if (window.IsRectValid() == false) {
		ReportError("Rectangle window must be set prior to feature extraction");
	}

	blocks = new FeatureBlocks();

	for (i = 0; i < defs.size(); i++) {
		blockInfo = defs[i];
		blocks->CreateBlockList(window, blockInfo);
	}

	return blocks;
}


/*******
 * class FeatureExtControl 
 *******/
FeatureExtControl::FeatureExtControl() {

	methods = new FeatureMethods();
}


FeatureExtractionSetup *FeatureExtControl::InstantiateExtSetup(string setupID, AllParameters *params) {
ExtractionMethod *extMethod;
FeatureParams featParams;
FeatureExtractionSetup *featExt;
multimap<string, string>::iterator it;
multimap<string, string> setupParams;
vector<FeatBlockInfo> blockInfo;
FeatBlockDef *featBlockDef = NULL;

	// retrieve setup parameters
	setupParams = params->GetParams(setupID);

	// instantiate extraction method to be used
	it = setupParams.find("method");
	if (it == setupParams.end())
		ReportError("Could not find the feature extraction method used in setup '%s' (see parameter method)", setupID.c_str());

	extMethod = methods->InstanciateMethod(it->second);

	// retrieve information regarding the blocks used 
	it = setupParams.find("blocks");
	if (it == setupParams.end()) {
		ReportWarning("Could not find the block setup used in setup '%s' (see parameter blocks)", setupID.c_str());
	}
	else {
		featParams = params->RetrieveFeatParams();
		blockInfo = featParams.GetBlocks(it->second);

		// create block definitions
		featBlockDef = new FeatBlockDef();
		featBlockDef->SetBlockDefinition(blockInfo);
	}

	// retrieve parameters to setup the feature extraction method
	it = setupParams.find("featSetup");
	if (it == setupParams.end())
		ReportError("Could not find the setup for the feature extraction methdo in setup '%s' (see parameter featSetup)",
		setupID.c_str());
	
	// setup the method 
	extMethod->Setup(it->second, params);

	// create the method 
	featExt = new FeatureExtractionSetup(extMethod, featBlockDef);	

	return featExt;
}



void FeatureExtControl::SetImage(int featSetupIdx, SSFImage *img) {
	
	extractSetup[featSetupIdx]->SetImage(img);
}


void FeatureExtControl::SetImage(vector<int> &featSetupIdx, SSFImage *img) {
size_t i;

	for (i = 0; i < featSetupIdx.size(); i++) 
		extractSetup[featSetupIdx[i]]->SetImage(img);
}


// set image from which the feature extraction will be performed (must be done before extracting features)
void FeatureExtControl::SetImage(int featSetupIdx, vector<SSFImage*> &imgs) {

	extractSetup[featSetupIdx]->SetImage(imgs);
}



void FeatureExtControl::ExtractFeatures(int featSetupIdx, SSFRect &window, FeatureBlocks *blocks, SSFFeatures &ssfFeats) {

	if (featSetupIdx > extractSetup.size()) 
		ReportError("Invalid feature extraction setup ID '%d'", featSetupIdx);

	// extract features
	if (blocks == NULL)	// extract when blocks have not been selected
		extractSetup[featSetupIdx]->ExtractFeatures(window, ssfFeats);
	else
		extractSetup[featSetupIdx]->ExtractFeatures(window, blocks, ssfFeats);
}




void FeatureExtControl::ExtractFeatures(vector<int> &featSetupIdx, SSFRect &window, SSFFeatures &ssfFeats) {
SSFMatrix<FeatType> mtmp;
size_t i;

	for (i = 0; i < featSetupIdx.size(); i++) {
		localSSFFeat[featSetupIdx[i]]->Reset();
		ExtractFeatures(featSetupIdx[i], window, NULL, *localSSFFeat[featSetupIdx[i]]);
	}
	ssfFeats.Reset();
	for (i = 0; i < featSetupIdx.size(); i++) {
		mtmp = localSSFFeat[featSetupIdx[i]]->RetrieveFeatures(true).t();
		ssfFeats.AddFeatures(mtmp);
	}
}



int FeatureExtControl::RetrieveSetupIdx(size_t featSetupID) {
unordered_map<size_t, int>::iterator it;

	it = IDMap.find(featSetupID);

	if (it == IDMap.end()) 
		ReportError("Invalid feature setup ID '%d'", featSetupID);

	return it->second;
}



FeatBlockDef *FeatureExtControl::RetrieveBlocks(int featSetupIdx) {

	if (featSetupIdx > extractSetup.size()) 
		ReportError("Invalid feature extraction setup ID '%d'", featSetupIdx);

	// return blocks
	return extractSetup[featSetupIdx]->RetrieveBlockDefs();
}


FeatureBlocks *FeatureExtControl::RetriveActualBlocks(int featSetupIdx, SSFRect &window) {

	if (featSetupIdx > extractSetup.size()) 
		ReportError("Invalid feature extraction setup ID '%d'", featSetupIdx);

	// return actual blocks
	return extractSetup[featSetupIdx]->RetriveFeatureBlocks(window);
}


FeatureIndex FeatureExtControl::Setup(string paramFile) {
FeatureIndex featIndex;
AllParameters *params;
FeatureExtractionSetup *featExt;
FeatureParams featParams;
vector<string> setups;
SSFFeatures *ssfFeat;
size_t id;

	// parse setup file
	params = new AllParameters();
	params->ParseFile(paramFile);
	featParams = params->RetrieveFeatParams(); 

	// allocate features for each setup
	setups = featParams.GetSetups();
	for (size_t i = 0; i < setups.size(); i++) {

		// instantiate a new feature extraction for the current setup
		featExt = InstantiateExtSetup(setups[i], params);

		// create a unique ID and set to the map
		id = GenerateSIZETRandomNumber();
		//cout << id << "\n";
		IDMap.insert(pair<size_t, int>(id, (int) i));

		// add feature extraction setup to the vector 
		extractSetup.push_back(featExt);

		// set generated id for the current setup
		featIndex.SetMap(setups[i], id);
	}

	// allocate tmp storage for the features
	for (size_t i = 0; i < setups.size(); i++) {
		ssfFeat = new SSFFeatures();
		localSSFFeat.push_back(ssfFeat);
	}

	delete params;

	return featIndex;
}



void FeatureExtControl::Save(SSFStorage &storage) {


	// must keep the same order as in the current ordering

	// check if the storage is up and running
	if (storage.isOpened() == false)
		ReportError("Invalid file storage!");
}