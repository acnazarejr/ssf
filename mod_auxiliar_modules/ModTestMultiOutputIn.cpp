#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "ModTestMultiOutputIn.h"

ModTestMultiOutputIn instance("");

/******
 * class ModTestMultiOutputIn
 ******/
ModTestMultiOutputIn::ModTestMultiOutputIn(string modID) :  SSFUserModule(modID) {

	/* set input and output data */
	modParams.Require(modID, SSF_DATA_TRACKLET, &inMod);		// input frames

	/* module description */
	info.Description("Module to test multiple outputs of the same type (input module).");

	/* register the module */
	this->Register(this, modID);
}



SSFMethod *ModTestMultiOutputIn::Instantiate(string modID) {

	return new ModTestMultiOutputIn(modID); 
}



void ModTestMultiOutputIn::Setup() {

	if (inMod == "")
		ReportError("Object candidates input module not set (variable: inMod)!");
}



void ModTestMultiOutputIn::Execute() {
SMIndexType position = 0;
SSFTracklet *tracklet;
SMIndexType idx;
	
	while ((tracklet = glb_sharedMem.RetrieveTracklet(inMod, position++)) != NULL) {

		idx = tracklet->GetSMID();

		ReportStatus("Tracklet -> %d (%s)", idx, inMod.c_str());

		glb_sharedMem.UnlinkDataItem(tracklet);
	}
}