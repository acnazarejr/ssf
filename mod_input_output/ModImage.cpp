#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "files.h"
#include "ModImage.h"


ModImage instance("");

ModImage::ModImage(string modID) : SSFUserModule(modID) {

	fm = NULL;

	/* set default values */
	mask = "*.png";

	/* Add parameters */
	inputParams.Add("inputName", &inputName, "Directory containing images or file name", true);
	inputParams.Add("mask", &mask, "File mask", false);

	/* module information */
	info.Description("Module to load images stored in the computer.");

	/* set input and output data */
	modParams.Provide(modID, SSF_DATA_FEED);			// feed that will be created
	modParams.Provide(modID, SSF_DATA_FRAME);			// frames that will be created
	modParams.Provide(modID, SSF_DATA_IMAGE);			// images that will be created
	modParams.Provide(modID, SSF_DATA_STRING);			// image filenames that will be created

	/* register the module */
	this->Register(this, modID);
}


ModImage::~ModImage() {

	if (fm != NULL)
		delete fm;
}



SSFMethod *ModImage::Instantiate(string instanceID) {

	return new ModImage(instanceID); 
}


void ModImage::Setup() {

}


// execute module
void ModImage::Execute() {
SMIndexType frameID, feedID;
size_t format;
SSFImage *img;
int i;

	fm = new FileManager();

	// check if the type is correct
	format = fm->GetInputType(inputName);
	if (format != FILETYPE_DIR && format != FILETYPE_SINGLEIMG)
		ReportError("Input '%s' is not an image or a directory with images", inputName.c_str());

	// load files
	fm->SetInputFile(inputName, mask);

	feedID = glb_sharedMem.CreateFeed(SSF_DATA_IMAGE);

	// set all images to the feed
	for (i = 0; i < fm->GetNEntries(); i++) {

		// load image
		img = new SSFImage(fm->RetrieveEntry(i));

		// set to the shared memory
		frameID = glb_sharedMem.CreateFrame(feedID);
		glb_sharedMem.SetFrameFilename(frameID, fm->RetrieveEntry(i));
		glb_sharedMem.SetFrameImage(frameID, img);
	}
}