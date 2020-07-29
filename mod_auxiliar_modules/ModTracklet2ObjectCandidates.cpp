#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "ModTracklet2ObjectCandidates.h"

ModTracklet2ObjectCandidates instance("");


/******
 * class ModTracklet2ObjectCandidates
 ******/
ModTracklet2ObjectCandidates::ModTracklet2ObjectCandidates(string modID) :  SSFUserModule(modID) {

	/* set input and output data */
	modParams.Require(modID, SSF_DATA_TRACKLET, &inModTracklet);	// input tracklet
	modParams.Provide(modID, SSF_DATA_OBJECTCANDIDATES);			// output

	/* module information */
	info.Description("Module to convert from SSFTracklet to SSFObjectCandidates.");

	/* register the module */
	this->Register(this, modID);
}

SSFMethod *ModTracklet2ObjectCandidates::Instantiate(string modID) {

	return new ModTracklet2ObjectCandidates(modID); 
}


void ModTracklet2ObjectCandidates::Setup() {

	if (inModTracklet == "")
		ReportError("Tracklet input module has not been set (variable: inModTracklet)!");
}


void ModTracklet2ObjectCandidates::Execute() {
size_t position = 0;
SSFTracklet *tracklet;

	// retrieve feedID
	while( (tracklet = glb_sharedMem.RetrieveTracklet( inModTracklet, position++ )) != NULL ) {

		glb_sharedMem.SetObjectCandidates(
			new SSFObjectCandidates(tracklet->GetTrackletID())
		);
	}
}
