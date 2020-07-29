#include "headers.h"
#include "parameters.h"
#include "FeatureBlocks.h"


/****
 * Class FeatBlockDef
 ****/
void FeatBlockDef::AddBlockDefinition(FeatBlockInfo def) {

	blockDefs.push_back(def);
}


void FeatBlockDef::SetBlockDefinition(vector<FeatBlockInfo> def) {

	this->blockDefs = def;
}

vector<FeatBlockInfo> FeatBlockDef::RetrieveBlockDefinitions() {

	return blockDefs;
}


void FeatBlockDef::Save(SSFStorage &storage) {
FeatBlockInfo info;
size_t i;

	if (storage.isOpened() == false)
		ReportError("Invalid file storage!");

	storage << "FeatBlockDef" << "{";
	storage << "ndefs" << (int) blockDefs.size();

	// print PLS model for each class
	for (i = 0; i < blockDefs.size(); i++) {

		// new level
		storage << "def" + IntToString((int) i) << "{";

		// save info
		info = blockDefs[i];

		storage << "strideX" << info.strideX;
		storage << "strideY" << info.strideY;
		info.block.Save(storage);

		// return the level
		storage << "}";
	}
	storage << "}";
}


void FeatBlockDef::Load(const FileNode &node) {
int ndefs, i;
FileNode n, n2;
FeatBlockInfo info;

	if (blockDefs.size() > 0)
		ReportError("Block definition already loaded");

	n = node["FeatBlockDef"];
	n["ndefs"] >> ndefs;

	for (i = 0; i < ndefs; i++) {
		n2 = n["def" + IntToString(i)];

		// read the label
		n2["strideX"] >> info.strideX;
		n2["strideY"] >> info.strideY;
		info.block.Load(n2);

		blockDefs.push_back(info);
	}
}


/*****
 * Class FeatureBlocks
 *****/
FeatureBlocks::FeatureBlocks() {

	selectedFeatures.clear();
	blistAll = new vector<SSFRect *>();
	blistSel = new vector<SSFRect *>();
}


FeatureBlocks::~FeatureBlocks() {
int i;

	for (i = 0; i < (int) blistAll->size(); i++) {
		delete blistAll->at(i);
	}

	delete blistAll;
	delete blistSel;
}


// create a block of feature extraction
void FeatureBlocks::AddBlock(int x0, int y0, int w, int h) {
SSFRect *bf;

	bf = new SSFRect();
	bf->x0 = x0;
	bf->y0 = y0;
	bf->h = h;
	bf->w = w;

	blistAll->push_back(bf);
	blistSel->push_back(bf);
}



// select a set of blocks (change f0 and f1 for each selected block)
void FeatureBlocks::SelectBlocks(vector<int> &selectedBlocks) {
int i;

	blistSel->clear();

	for (i = 0; i < (int) selectedBlocks.size(); i++) {
		blistSel->push_back(blistAll->at(selectedBlocks[i]));
	}
}







void FeatureBlocks::CreateBlockList(SSFRect window, FeatBlockInfo blockDef) {
int x, y;

	for (y = window.y0; y <= window.y0 + window.h - blockDef.block.h; y += blockDef.strideY) {
		for (x = window.x0; x <= window.x0 + window.w - blockDef.block.w; x += blockDef.strideX) {
			AddBlock(x, y, blockDef.block.w, blockDef.block.h);
		}
	}
}

