#ifndef _FEATBLOCKINFO_H_
#define _FEATBLOCKINFO_H_
#include "SSFRect.h"


// information regarding the block passed to the feature extraction
typedef struct {
	SSFRect block;
	int strideX, strideY;
} FeatBlockInfo;

#endif