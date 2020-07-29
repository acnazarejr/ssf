#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "ModTestMultiOutput.h"

ModTestMultiOutput instance("");

/******
 * class ModTestMultiOutput
 ******/
ModTestMultiOutput::ModTestMultiOutput(string modID) :  SSFUserModule(modID) {

	/* set input and output data */
	modParams.Require(modID, SSF_DATA_IMAGE, &inMod);		// input frames
	modParams.Provide(modID, SSF_DATA_TRACKLET, "training");	// output1
	modParams.Provide(modID, SSF_DATA_TRACKLET, "test");		// output2
	modParams.Provide(modID, SSF_DATA_TRACKLET, "GT");			// output3

	/* module description */
	info.Description("Module to test multiple outputs of the same type.");

	/* register the module */
	this->Register(this, modID);
}



SSFMethod *ModTestMultiOutput::Instantiate(string modID) {

	return new ModTestMultiOutput(modID); 
}



void ModTestMultiOutput::Setup() {

	if (inMod == "")
		ReportError("Object candidates input module not set (variable: inMod)!");
}



void ModTestMultiOutput::Execute() {
SSFImage *img;
Mat data;
size_t frameID;
SMIndexType position = 0;
SMIndexType idx;
SSFTracklet *tracklet;

	while ((frameID = glb_sharedMem.RetrieveFrameIDbyPosition(inMod, SSF_DATA_IMAGE, position++)) != SM_INVALID_ELEMENT) {

		img = glb_sharedMem.GetFrameImage(frameID);

		data = img->RetrieveCopy();

		data.release();

		tracklet = new SSFTracklet();
		idx = glb_sharedMem.SetTracklet(tracklet, "training");
		ReportStatus("idx: %d -> training", idx);


		tracklet = new SSFTracklet();
		idx = glb_sharedMem.SetTracklet(tracklet, "test");
		ReportStatus("idx: %d -> test", idx);

		tracklet = new SSFTracklet();
		idx = glb_sharedMem.SetTracklet(tracklet, "GT");
		ReportStatus("idx: %d -> GT", idx);
	}
}