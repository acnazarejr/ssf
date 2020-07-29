#ifndef _FES_BLOCKS_H_
#define _FES_BLOCKS_H_


class FESBlocks {

protected:

public:
	~FESBlocks();

	virtual void ClearMemory() = 0;

	virtual int GetDataType() = 0;
};



class FESBlocks2D : public FESBlocks {
vector<SSFRect *> *blistAll;	// vector with all blocks
vector<SSFRect *> *blistSel;	// vector with selected blocks
set<int> selectedFeatures;		// IDs of selected feature extraction methods

private:




public:
	FESBlocks2D();

	/* create a single block of the image size */
	FESBlocks2D(const Mat &img);

	// brief add a block, when added, it is selected.
	void AddBlock(int x0, int y0, int w, int h);

	void ClearMemory();

	~FESBlocks2D();
	int GetDataType() { return FES_DATATYPE_2D; }

	/* create blocks for a given window */
	void CreateBlockList(SSFRect window, FeatBlockInfo blockDef);

	/* select a set of blocks (change f0 and f1 for each selected block) */
	void SelectBlocks(vector<int> &selectedBlocks);

	/* retrieve blocks from which feature extraction will be performed */
	vector<SSFRect *> *RetrieveSelectedBlocks() { return blistSel; }
};


#endif