#ifndef _FEATURE_BLOCKS_H_
#define _FEATURE_BLOCKS_H_
#include "headers.h"
#include "ExtractionMethod.h"


/* Definitions for feature blocks */
class FeatBlockDef {
vector<FeatBlockInfo> blockDefs;

public:
	// add a definition of blocks
	void AddBlockDefinition(FeatBlockInfo def);

	// set definitions of blocks (remove what is already set)
	void SetBlockDefinition(vector<FeatBlockInfo> def);

	// retrieve block definitions
	vector<FeatBlockInfo> RetrieveBlockDefinitions();

	// save the block definition
	void Save(SSFStorage &storage);

	// load the block definition
	void Load(const FileNode &node);
};



/*  Create struct to extract features from multiple blocks and sizes inside a window */
class FeatureBlocks {
vector<SSFRect *> *blistAll;	// vector with all blocks
vector<SSFRect *> *blistSel;	// vector with selected blocks
set<int> selectedFeatures;		// IDs of selected feature extraction methods

private:
	// brief add a block, when added, it is selected.
	void AddBlock(int x0, int y0, int w, int h);

public:
	FeatureBlocks();
	~FeatureBlocks();

	/* create blocks for a given window */
	void CreateBlockList(SSFRect window, FeatBlockInfo blockDef);

	/* select a set of blocks (change f0 and f1 for each selected block) */
	void SelectBlocks(vector<int> &selectedBlocks);

	/* retrieve blocks from which feature extraction will be performed */
	vector<SSFRect *> *RetrieveSelectedBlocks() { return blistSel; }
};




#endif