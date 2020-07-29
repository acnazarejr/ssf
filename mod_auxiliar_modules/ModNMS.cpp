#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "SSFSlidingWindow.h"
#include "SSFDetwins.h" 
#include "ModNMS.h"

ModNMS instance("");

/******
 * class ModNMS
 ******/
ModNMS::ModNMS(string modID) :  SSFUserModule(modID) {

	inputParams.Add("threshold", &threshold, "Threshold to remove samples (remove samples with responses smaller)", true);
	inputParams.Add("intersection", &intersection, "Value of intersection/union to remove sample with lower response", true);

	/* module information */
	info.Description("Module to perform non-maximum suppression.");

	/* set input and output data */
	modParams.Require(modID,  SSF_DATA_SLDWINDOWS, &inputModID);	// input sliding windows
	modParams.Provide(modID, SSF_DATA_SLDWINDOWS);					// output sliding windows

	/* register the module */
	this->Register(this, modID);
}


SSFMethod *ModNMS::Instantiate(string modID) {

	return new ModNMS(modID); 
}


void ModNMS::Setup() {

	if (inputModID == "")
		ReportError("Input module with sliding windows has not been set (variable: inputModID)!");
}


void ModNMS::Execute() {
size_t position = 0;
SSFSlidingWindow *sldWindow, *newsldWindow;
SSFDetwins *ssfdetwins;
vector<DetWin> *detwins, *resDetwin;
size_t frameID;

	ReportStatus("Executing module %s [%s] with input from module '%s'", this->GetName().c_str(), this->GetInstanceID().c_str(),
		inputModID.c_str());

	while ((frameID = glb_sharedMem.RetrieveFrameIDbyPosition(inputModID, SSF_DATA_SLDWINDOWS, position++)) != SM_INVALID_ELEMENT) {

		// retrieve the sliding windows
		sldWindow = glb_sharedMem.GetSlidingWindows(inputModID, frameID);

		// processing the current frame
		ReportStatus("Non-maximum suppression frameID '%d'", sldWindow->GetFrameID());

		newsldWindow = new SSFSlidingWindow();

		detwins = sldWindow->GetAllDetWins();
		resDetwin = NonMaxSupression(*detwins, intersection, threshold);	
        delete detwins;

		ssfdetwins = new SSFDetwins(resDetwin);

		newsldWindow->AddDetwins(ssfdetwins, NULL);

		// set new samples
		glb_sharedMem.SetSlidingWindows(frameID, newsldWindow);
	}

	//glb_sharedMem.FinishProcessing(this->GetID(), SSF_DATA_SLDWINDOWS);
}



vector<DetWin> *ModNMS::NonMaxSupression(vector<DetWin> Listdetwins, float Inters, float threshold) {
vector<DetWin> *NewListDetWin;
DetWin detwin1, detwin2;
SSFRect rectA, rectB;
int i, j, areaInt, areaUnion;
vector<int> intObjs;
vector<DetWin> v;
float a0;


	for (i = 0; i < Listdetwins.size(); i++) {
		if (Listdetwins[i].response > threshold)
			v.push_back(Listdetwins[i]);
	}

	Listdetwins = v;

	NewListDetWin = new vector<DetWin>;
	//intObjs = new Vector<int>();
	intObjs.resize(Listdetwins.size(), 0);
	for (i = 0; i < (int) Listdetwins.size(); i++) {
		detwin1 = Listdetwins[i];
		rectA = detwin1.GetSSFRect();
		for (j = i + 1; j < (int) Listdetwins.size(); j++) {
			detwin2 = Listdetwins[j];
			rectB = detwin2.GetSSFRect();
			areaInt = SSFRectInt(rectA, rectB);

			if (areaInt == 0) // no intersection
				continue;
			
			areaUnion = (rectA.w * rectA.h) + (rectB.w * rectB.h) - areaInt;
			a0 = (float) areaInt / (float)areaUnion;
			if (a0 > Inters) {
				if (detwin1.response > detwin2.response) {
					intObjs[j] = 1;
				}
				else {
					intObjs[i] = 1;
				}
			}
		}
	}

	// remove detwin with low probability
	for (i = 0; i < intObjs.size(); i++) {
		detwin1 = Listdetwins[i];
		if (intObjs[i] == 0 && Listdetwins[i].response > threshold) {
			NewListDetWin->push_back(detwin1);
		}
	}


	//delete Listdetwins;
	return NewListDetWin;
}