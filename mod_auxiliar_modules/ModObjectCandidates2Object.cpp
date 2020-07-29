#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "ModObjectCandidates2Object.h"

ModObjectCandidates2Object instance("");

/******
 * class ModObjectCandidates2Object
 ******/
ModObjectCandidates2Object::ModObjectCandidates2Object(string modID) :  SSFUserModule(modID) {

	/* set input and output data */
	modParams.Require(modID, SSF_DATA_OBJECTCANDIDATES, &inMod);	// input object candidate
	modParams.Provide(modID, SSF_DATA_OBJECT);						// output

	/* module description */
	info.Description("Module to convert SSFObjectCandidate to SSFObject.");

	/* register the module */
	this->Register(this, modID);
}

SSFMethod *ModObjectCandidates2Object::Instantiate(string modID) {

	return new ModObjectCandidates2Object(modID); 
}


void ModObjectCandidates2Object::Setup() {

	if (inMod == "")
		ReportError("Object candidates input module not set (variable: inMod)!");
}


void ModObjectCandidates2Object::Execute() {
SMIndexType position = 0;
SSFObjectCandidates *objCandidates;
vector<SMIndexType> trackletIDs;
const vector<SSFObjectCandidate> *candidates;

	while( (objCandidates = glb_sharedMem.GetObjectCandidates(inMod, position++)) != NULL ) {

		// add
		trackletIDs.push_back(objCandidates->GetTrackletID());
		candidates = objCandidates->GetObjectCandidates();
		if( candidates->empty() ) {
			ReportError("There is no object candidate for tracklet %d of module %s.", objCandidates->GetTrackletID(), inMod.c_str());
		}
		glb_sharedMem.CreateObject(
			trackletIDs,
			glb_sharedMem.RetrieveObject( candidates->front().objCandidate )->GetObjID()
		);
		trackletIDs.clear();
	}
}
