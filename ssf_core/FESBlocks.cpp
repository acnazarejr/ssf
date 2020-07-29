#include "headers.h"
#include "SSFCore.h"
#include "FES.h"
#include "FESBlocks.h"


/*****
 * Class FESBlocks
 *****/
FESBlocks::~FESBlocks() {

}

/*****
 * Class FESBlocks2D
 *****/
void FESBlocks2D::ClearMemory() {
int i;

	for (i = 0; i < (int) blistAll->size(); i++) {
		delete blistAll->at(i);
	}

	delete blistAll;
	delete blistSel;
}


FESBlocks2D::FESBlocks2D() {

	selectedFeatures.clear();
	blistAll = new vector<SSFRect *>();
	blistSel = new vector<SSFRect *>();
}

FESBlocks2D::FESBlocks2D(const Mat &img) {

	selectedFeatures.clear();
	blistAll = new vector<SSFRect *>();
	blistSel = new vector<SSFRect *>();

	this->AddBlock(0, 0, img.cols, img.rows);
}

FESBlocks2D::~FESBlocks2D() {

	this->ClearMemory();
}



// create a block of feature extraction
void FESBlocks2D::AddBlock(int x0, int y0, int w, int h) {
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
void FESBlocks2D::SelectBlocks(vector<int> &selectedBlocks) {
int i;

	blistSel->clear();

	for (i = 0; i < (int) selectedBlocks.size(); i++) {
		blistSel->push_back(blistAll->at(selectedBlocks[i]));
	}
}




void FESBlocks2D::CreateBlockList(SSFRect window, FeatBlockInfo blockDef) {
int x, y;

	for (y = window.y0; y <= window.y0 + window.h - blockDef.block.h; y += blockDef.strideY) {
		for (x = window.x0; x <= window.x0 + window.w - blockDef.block.w; x += blockDef.strideX) {
			AddBlock(x, y, blockDef.block.w, blockDef.block.h);
		}
	}
}