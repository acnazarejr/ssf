#include "headers.h"
#include "misc.h"
#include "SSFData.h"
#include "structures.h"
#include "SSFCamera.h"
#include "SSFDetwins.h"
#include "SSFDict.h"
#include "SSFFeed.h" 
#include "SSFFrame.h" 
#include "SSFImage.h"
#include "SSFInvertedList.h" 
#include "SSFObject.h" 
#include "SSFSample.h" 
#include "SSFSamples.h" 
#include "SSFSlidingWindow.h"
#include "SSFString.h"
#include "SSFTracklet.h" 
//#include "SSFMatrices.h"
//#include "SSFUserData.h"
#include "SSFObjectCandidates.h"
#include "SSFEOD.h"



SSFStructures::SSFStructures() {

	SSFCamera *ssfCamera = new SSFCamera();
	structureMap.insert(pair<string, SSFData *>(ssfCamera->GetDataType(), ssfCamera));

	SSFDict *ssfDict = new SSFDict();
	structureMap.insert(pair<string, SSFData *>(ssfDict->GetDataType(), ssfDict));

	SSFFeed *ssfFeed = new SSFFeed();
	structureMap.insert(pair<string, SSFData *>(ssfFeed->GetDataType(), ssfFeed));

	SSFFrame *ssfFrame = new SSFFrame(SM_INVALID_ELEMENT);
	structureMap.insert(pair<string, SSFData *>(ssfFrame->GetDataType(), ssfFrame));

	SSFImage *ssfImage = new SSFImage();
	structureMap.insert(pair<string, SSFData *>(ssfImage->GetDataType(), ssfImage));

	SSFInvertedList *ssfInvertedList = new SSFInvertedList();
	structureMap.insert(pair<string, SSFData *>(ssfInvertedList->GetDataType(), ssfInvertedList));

	SSFObject *ssfObject = new SSFObject();
	structureMap.insert(pair<string, SSFData *>(ssfObject->GetDataType(), ssfObject));

	SSFSample *ssfSample = new SSFSample();
	structureMap.insert(pair<string, SSFData *>(ssfSample->GetDataType(), ssfSample));

	SSFSamples *ssfSamples = new SSFSamples();
	structureMap.insert(pair<string, SSFData *>(ssfSamples->GetDataType(), ssfSamples));

	SSFSlidingWindow *ssfSlidingWindow = new SSFSlidingWindow();
	structureMap.insert(pair<string, SSFData *>(ssfSlidingWindow->GetDataType(), ssfSlidingWindow));

	SSFString *ssfString = new SSFString();
	structureMap.insert(pair<string, SSFData *>(ssfString->GetDataType(), ssfString));

	SSFTracklet *ssfTracklet = new SSFTracklet();
	structureMap.insert(pair<string, SSFData *>(ssfTracklet->GetDataType(), ssfTracklet));

	//SSFMatrices *ssfMatrices = new SSFMatrices();
	//structureMap.insert(pair<string, SSFData *>(ssfMatrices->GetDataType(), ssfMatrices));

	//SSFUserData *ssfUserData = new SSFUserData("");
	//structureMap.insert(pair<string, SSFData *>(ssfUserData->GetDataType(), ssfUserData));

	SSFObjectCandidates *ssfObjCandidates = new SSFObjectCandidates();
	structureMap.insert(pair<string, SSFData *>(ssfObjCandidates->GetDataType(), ssfObjCandidates));

	SSFEOD *ssfEOD = new SSFEOD();
	structureMap.insert(pair<string, SSFData *>(ssfEOD->GetDataType(), ssfEOD));
}



bool SSFStructures::CheckDataType(string dataType) {
map<string, SSFData *>::iterator it;

	it = structureMap.find(dataType);

	if (it == structureMap.end())
		return false;

	return true;
}