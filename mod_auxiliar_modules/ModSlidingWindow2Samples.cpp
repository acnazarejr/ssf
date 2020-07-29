#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "ModSlidingWindow2Samples.h"

ModSlidingWindow2Samples instance("");

/******
 * class ScaleManager
 ******/
ModSlidingWindow2Samples::ModSlidingWindow2Samples(string modID) :  SSFUserModule(modID) {

	/* set input and output data */
	modParams.Require(modID, SSF_DATA_SLDWINDOWS, &inModSldWin);	// input image
	modParams.Provide(modID, SSF_DATA_SAMPLES);			// output
	modParams.Provide(modID, SSF_DATA_SAMPLE);			// output

	/* module information */
	info.Description("Module to convert SSFSlidingWindows to SSFSamples.");

	/* register the module */
	this->Register(this, modID);
}

SSFMethod *ModSlidingWindow2Samples::Instantiate(string modID) {

	return new ModSlidingWindow2Samples(modID); 
}


void ModSlidingWindow2Samples::Setup() {

	if (inModSldWin == "")
		ReportError("Input module with sliding windows has not been set (variable: inModSldWin)!");
}


void ModSlidingWindow2Samples::Execute() {
size_t frameID = 0;
size_t position = 0; 
SSFSlidingWindow *sldWin;
SSFImage *img;
//vector<SMIndexType> samples;
SMIndexType id;

	ReportStatus("Executing module %s [%s]", this->GetName().c_str(), this->GetInstanceID().c_str());

	// retrieve feedID
	while ((frameID = glb_sharedMem.RetrieveFrameIDbyPosition(inModSldWin, SSF_DATA_SLDWINDOWS, position++)) != SM_INVALID_ELEMENT) {
		// SSFSamples *samples = new SSFSamples(); 
		// TODO change SetFrameSamples() to receive SSFSamples instead of vector<SSFSamples *> 
		
		//samples.clear();
		
		ReportStatus("[FrameID: %d] creating detwins from sliding windows at position '%d", frameID, position);

		// retrieve the next frame
		sldWin = glb_sharedMem.GetSlidingWindows(inModSldWin, frameID);
		img = glb_sharedMem.GetFrameImage(frameID);

		// get image data and size of rescaled image
		size_t nscales = sldWin->GetNEntries();

		for (size_t i = 0; i < nscales; ++i) {
			// retrieving scale 
			ScaleInfo *info = sldWin->GetScaleInfo(i);
			SSFDetwins *currScaleDetwins = sldWin->GetSSFDetwins(i);
			
			// retrieving sliding windows
			vector<DetWin> *detwins = currScaleDetwins->GetDetwins();


			// generating sample for each sliding window
			for (size_t j = 0; j < detwins->size(); ++j) {
				DetWin detwin = detwins->at(j); 

				// rescales detwin 
				int x0 = (int) floor((float) (detwin.x0 + info->objSize.x0) / info->scale );
                int y0 = (int) floor((float) (detwin.y0 + info->objSize.y0) / info->scale );
                int h  = (int) floor((float) info->objSize.h / info->scale );
                int w  = (int) floor((float) info->objSize.w / info->scale );

				// retrieve patch 
				SSFRect window(x0, y0, w, h);
	
				// creating a new sample and saving into the vector
				SSFSample *sample = new SSFSample();
				sample->SetPatch(window);
				sample->SetResponse(detwin.response);
				//id = glb_sharedMem.SetFrameSample(frameID, sample);
				id = glb_sharedMem.AddFrameSample(frameID, sample);
				//samples.push_back(id);
			}
		}
		glb_sharedMem.EndFrameSamples(frameID);
	}
}
