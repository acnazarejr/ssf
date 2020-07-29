#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "ModBGSMethods.h"

ModBGSMethods instance("");

/******
 * class ModBGSMethods
 ******/
ModBGSMethods::ModBGSMethods(string modID) : SSFUserModule(modID) {

	/* set input and output data */
	modParams.Require(modID,  SSF_DATA_IMAGE, &inputModID);	// input image
	modParams.Provide(modID, SSF_DATA_IMAGE);				// output mask

	/* module information */
	info.Description("Module to perform background subtraction.");

	/* register the module */
	this->Register(this, modID);
}


SSFMethod *ModBGSMethods::Instantiate(string modID) {

	return new ModBGSMethods(modID); 
}


void ModBGSMethods::Setup() {

	if (inputModID == "")
		ReportError("Input module to provide the input images has not been set (variable: inputModID)!");
}

void ModBGSMethods::Execute() {
size_t position;
SSFImage *img, *img2;
Mat data;
Mat mask;
Mat back;
BackgroundSubtractorMOG2 bg;
size_t frameID;
	
	ReportStatus("Executing module %s [%s]", this->GetName().c_str(), this->GetInstanceID().c_str());

	//feedID = glb_sharedMem.GetFeedIDbyName(feedNameTest);

	/* Test camera
	SSFCamera *cmd = new SSFCamera();
	SSFCamera *cmd2;
	cmd->SetCommand("test");
	glb_sharedMem.EnableCameraControl(feedID);
	glb_sharedMem.SetCameraCommand(feedID, cmd);
	*/

	/* Test object 
	SSFObject *obj = new SSFObject();
	SSFObject *obj2 = NULL;
	glb_sharedMem.SetObject(obj);
	glb_sharedMem.SetObject(obj);
	glb_sharedMem.SetFinishProcessing(SSF_DATA_OBJECT);

	for (size_t idx = 0; ; idx++) {
		obj2 = glb_sharedMem.GetObject(this->GetID(), idx);

		if (obj2 == NULL)
			break;
	} 
	*/

	// perform test using all test images
	position = 0;
	size_t fid = 1;

	while ((frameID = glb_sharedMem.RetrieveFrameIDbyPosition(inputModID, SSF_DATA_IMAGE, position++)) != SM_INVALID_ELEMENT) {
		
		img = glb_sharedMem.GetFrameImage(frameID);

		data = img->RetrieveCopy();

		// APLICA A SUBTRACAO DE FUNDO
		bg(data, mask);
		/*bg.operator()(data, mask);*/

		// RETORNA A IMAGEM DE FUNDO
		bg.getBackgroundImage(back);

		// OPERAÇÃO DE EROSÃO E DILATAÇÃO
		erode(mask,mask,Mat());
        dilate(mask,mask,Mat());

		// TRANSFORMA A MASCARA EM UMA IMAGEM
		img2 = new SSFImage(mask);

		glb_sharedMem.SetFrameMask(frameID, img2);
        
		ReportStatus("BGS Execution done for position %d", img->GetFrameID());

		fid = frameID;
	}

	//glb_sharedMem.SetFinishProcessing(SSF_DATA_IMAGE);
}