#ifndef _SCALEINFO_H_
#define _SCALEINFO_H_
#include "SSFRect.h"

/* object scale information */
typedef struct {
	int strideX;
	int strideY;
	float scale;
	SSFRect objSize;
	SSFRect detWindowSize;
} ScaleInfo;


#endif