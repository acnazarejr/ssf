#include "headers.h"
#include "LAP.h"
#include "SSFString.h"
#include "ModControlCameraIP.h"

ModControlCameraIP instance("");

ModControlCameraIP::ModControlCameraIP(string modID) : SSFUserModule(modID) {

	/* Add parameters */
	initialSetup = "type:MJPEG";
	inputParams.Add("initialSetup", &initialSetup, "Initial setup for the camera", false);

	/* module information */
	info.Description("Module to control IP cameras.");

	/* register the module */
	this->Register(this, modID);
}


ModControlCameraIP::~ModControlCameraIP() {

}


void ModControlCameraIP::Setup() {


}


SSFMethod *ModControlCameraIP::Instantiate(string modID) {

	return new ModControlCameraIP(modID);
}






// execute module
void ModControlCameraIP::Execute() {
SSFString *command;

	// receives commands
//	while (1) {
//		command = (SSFString *) glb_sharedMem.GetData("", SSF_DATA_STRING);
//		ReportStatus("Command retrieved and sent to the camera %s [%s]", this->GetInstanceID().c_str(), command->GetString().c_str());
//	}
}