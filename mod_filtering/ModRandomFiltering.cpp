#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "ModRandomFiltering.h"

ModRandomFiltering instance("");

ModRandomFiltering::ModRandomFiltering(string modID) : SSFUserModule(modID) {

	/* Default parameters */
	seed_ = 0;

	/* add parameters */
	inputParams.Add("percentDetwins", &percentDetwins_, "Percentage of detection windows to be randomly selected", true);
	inputParams.Add("seed", &seed_, "Seed of the random generator", false);

	/* module information */
	info.Description("Implementation of the random filtering.");

	/* set input and output data */
	modParams.Require(modID, SSF_DATA_SLDWINDOWS, &inModSldWin_); // input sliding windows
	modParams.Provide(modID, SSF_DATA_SLDWINDOWS);				 // provides a new and smaller subset of sliding windows

	/* register the module */
	this->Register(this, modID);
}

SSFMethod *ModRandomFiltering::Instantiate(string modID) {

	return new ModRandomFiltering(modID); 
}


void ModRandomFiltering::Setup() {

	if (inModSldWin_ == "")
		ReportError("Input module with sliding windows has not been set (variable: inModSldWin_)!");
}

void ModRandomFiltering::Execute() {
size_t position = 0;
size_t frameID = 0;
SSFSlidingWindow *sldWins;
SSFSlidingWindow *selectedSldWins;
size_t totalWindowsSelected = 0;
size_t numWindowsSelected;
vector<DetWin> *selDetwins;
vector<size_t> permutation;
size_t maxSize;
SSFDetwins *detWins;
size_t localIndex, currScale, idxPerm, baseIndex;
size_t nAllDetWins;
size_t kDetWins;
DetWin dw;

	ReportStatus("Executing module %s [%s]", this->GetName().c_str(), this->GetInstanceID().c_str());

	// change seed of the random generator
	if (seed_ != 0) 
        std::srand( seed_ );
	else 
		ReportStatus("Using clock as seed for RandonGenerator");

	while ((frameID = glb_sharedMem.RetrieveFrameIDbyPosition(inModSldWin_, SSF_DATA_SLDWINDOWS, position++)) != SM_INVALID_ELEMENT) {

		sldWins = glb_sharedMem.GetSlidingWindows(inModSldWin_, frameID);
		selectedSldWins = new SSFSlidingWindow();

		// get all detwins and generates permutation index
		nAllDetWins = sldWins->GetNumberofWindows();
		kDetWins = (size_t) floor((float) nAllDetWins * percentDetwins_ );
		ReportStatus("[FrameID: %d] randomly selecting %d detwins windows (%.5f) at position '%d", frameID, kDetWins, percentDetwins_, position);

		permutation = GenerateRandomPermutation(nAllDetWins, kDetWins);

		// sort permutation vector
		std::sort(permutation.begin(),permutation.end());

		// determine to which scale a given detwin belongs
		baseIndex = 0; // relative base index of a given scale
		idxPerm = 0;
		maxSize = 0;

		for (currScale = 0; currScale < sldWins->GetNEntries(); currScale++) {
			detWins = sldWins->GetSSFDetwins(currScale);
			selDetwins = new vector<DetWin>();
			maxSize += detWins->GetNumberofDetWins();

			for (; idxPerm < permutation.size(); idxPerm++) {
				if (permutation[idxPerm] < maxSize) {
					localIndex = permutation[idxPerm] - baseIndex;
					dw = detWins->GetDetwins()->at(localIndex);
					selDetwins->push_back(dw); 
				}
				else {
					baseIndex += detWins->GetNumberofDetWins(); // relative size of a scale				
					break;
				}
			}

			// add these subwindows
			if (selDetwins->size() != 0) 
				selectedSldWins->AddDetwins(new SSFDetwins(selDetwins), sldWins->GetScaleInfo(currScale));
			else 
				delete selDetwins;
		}

		numWindowsSelected = selectedSldWins->GetNumberofWindows();
		ReportStatus("[Position: %d] Number of windows: %d", position, numWindowsSelected);

		totalWindowsSelected += numWindowsSelected;

		glb_sharedMem.SetSlidingWindows(frameID, selectedSldWins);
	}

	ReportStatus("Total selected windows: %d", totalWindowsSelected);
}

