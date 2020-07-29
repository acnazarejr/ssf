#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "files.h"
#include "ModCameraIP.h"



ModCameraIP instance("");

ModCameraIP::ModCameraIP(string modID) : SSFUserModule(modID) {

	/* Add parameters */
	inputParams.Add("IPAdress", &IPAdress, "Ip adress to access the IP camera", true);
	inputParams.Add("CameraModel", &CamModel, "Camera Model", true);							  
	inputParams.Add("yml", &yml, "File yml containing cameras info", true);           

	/* module information */
	info.Description("Module to capture data from a IP camera.");

	/* set input and output data */
	modParams.Provide(modID, SSF_DATA_FEED);			// feed that will be created
	modParams.Provide(modID, SSF_DATA_FRAME);			// frames that will be created
	modParams.Provide(modID, SSF_DATA_IMAGE);			// images that will be created

	/* register the module */
	this->Register(this, modID);
}



SSFMethod *ModCameraIP::Instantiate(string modID) {

	return new ModCameraIP(modID); 
}


ModCameraIP::~ModCameraIP() {

}


void ModCameraIP::Setup() {            


}

// execute module
void ModCameraIP::Execute() {
size_t frameID, feedID;
SSFImage *img;
int i =0;

	FileStorage      fs;
	FileNode         node;
	FileNodeIterator inode;
	
	if(!fs.open(yml, FileStorage::READ)) {
		
		ReportError("Cannot open the yml file");
	}

	node = fs["cameras"];

	for( inode = node.begin(); inode != node.end(); inode++) {
		
		if((*inode).name()==CamModel) {
			
			protocol=(*inode)["protocol"];
			arguments =(*inode)["args"];
		}
	}

	IP_string=protocol+IPAdress+arguments; 
	
	feedID = glb_sharedMem.CreateFeed(SSF_DATA_IMAGE);			

	VideoCapture cap(IP_string);

	if ( !cap.isOpened()) {

		ReportStatus("Cannot open camera %d",CamNumber);
	}
	else 
		while(1) {
		
			Mat frame;

	        bool bSuccess = cap.read(frame); 

	        if (!bSuccess) {
				ReportStatus("Camera died during process");
				break;
			}
			else {
				
				img = new SSFImage(frame);
				frameID = glb_sharedMem.CreateFrame(feedID);					
				glb_sharedMem.SetFrameImage(frameID, img);						
				i++;
			}
		}
}