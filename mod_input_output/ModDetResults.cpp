#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "files.h"
#include "ModDetResults.h"


ModDetResults instance("");


ModDetResults::ModDetResults(string modID) : SSFUserModule(modID) {

	/* set default values */
	fileSuffix = "Body";
    operation  = "read";

	/* Add parameters */
	inputParams.Add("dirName", &dirName, "Directory where detection results are stored", true);
	inputParams.Add("fileSuffix", &fileSuffix, "Suffix of the file", false);
    inputParams.Add("operation", &operation, "Operation: read or write. DEFAULT: read", false);
	inputParams.Add("dataType", &dataType, "Which data type will be written to disk", false);

	/* module information */
	info.Description("Module to load detection results from a txt.");

	/* set input and output data */
    // write operation
	modParams.Require(modID, SSF_DATA_SAMPLES, &inputModID); 
	modParams.Require(modID, SSF_DATA_SLDWINDOWS, &inputModID); 

    // read operation
	modParams.Require(modID, SSF_DATA_STRING, &inputModID);	// input files with saved detection results
	modParams.Provide(modID, SSF_DATA_SLDWINDOWS);			// sliding windows mask 

	/* register the module */
	this->Register(this, modID);
}


ModDetResults::~ModDetResults() {

}


void ModDetResults::Setup() {

	if (inputModID == "")
		ReportError("Input module with detection results has not been set (variable: inputModID)!");

	if (operation == "write" && dataType == "") 
		ReportError("dataType must be set for 'write' operation");
}

SSFMethod *ModDetResults::Instantiate(string modID) {

	return new ModDetResults(modID);
}


//================================================================================
// WRITING OPERATION
//================================================================================

void ModDetResults::WriteDetectionResults(string filename, SSFSamples* ssfSamples) {
InputType inputType;
ofstream outfile;
vector<SMIndexType> samplesIDs;

	inputType = FileParts(filename);

	if (fileSuffix.size() == 0)
		filename = dirName + FILESEP + inputType.name + ".txt";
	else
		filename = dirName + FILESEP + inputType.name + "." + fileSuffix + ".txt";

	outfile.open(filename.c_str());

	// if couldn't open file
	if (outfile.good() == false)
		ReportError("File '%s' couldn't be opened", filename.c_str());

	// write sliding windows
	samplesIDs = ssfSamples->GetSamples();
	for (int i = 0; i < samplesIDs.size(); ++i) {
		SSFSample *sample = glb_sharedMem.GetSample( samplesIDs[i] );
		SSFRect window = sample->GetWindow();
		outfile 
			<< window.x0 << " " 
			<< window.y0 << " "
			<< window.w  << " " 
			<< window.h  << " " 
			<< sample->GetResponse() << "\n";
	}
	
	outfile.close();
}


void ModDetResults::WriteDetectionResults(string filename, SSFSlidingWindow* ssfSldWins) {
InputType inputType;
ofstream outfile;
vector<DetWin> *detwins;
DetWin detwin;

	inputType = FileParts(filename);

	if (fileSuffix.size() == 0)
		filename = dirName + FILESEP + inputType.name + ".txt";
	else
		filename = dirName + FILESEP + inputType.name + "." + fileSuffix + ".txt";

	outfile.open(filename.c_str());

	// if couldn't open file
	if (outfile.good() == false)
		ReportError("File '%s' couldn't be opened", filename.c_str());

	// write sliding windows
	detwins = ssfSldWins->GetAllDetWins();
	for (int i = 0; i < detwins->size(); ++i) {
		detwin = detwins->at(i);
		outfile 
			<< detwin.x0 << " " 
			<< detwin.y0 << " " 
			<< detwin.w  << " " 
			<< detwin.h  << " " 
			<< detwin.response << "\n";
	}
	outfile.close();

	delete detwins;
}



void ModDetResults::Writing() {
FileManager fm;
string filename;
size_t position = 0;
SSFStorage storage;
size_t frameID;
SSFSamples *ssfSamples;
SSFSlidingWindow *ssfSldWins;

	// check if the directory exists
	if (CheckExistDirectory(dirName) == false)
		ReportError("Input '%s' is not a directory", dirName.c_str());

	// read samples from the SM
	if (dataType == "SAMPLES") {

		// perform detection results for every frame
		while ((frameID = glb_sharedMem.RetrieveFrameIDbyPosition(inputModID, SSF_DATA_SAMPLES, position++)) != SM_INVALID_ELEMENT) {

			ReportStatus("[FrameID: %d] writing detection results at position '%d", frameID, position);

			// retrieve the filename image
			filename = glb_sharedMem.GetFrameFilename(frameID);

			// load results
			ssfSamples = glb_sharedMem.GetFrameSamples(inputModID, frameID);

			// write detection results
			WriteDetectionResults(filename, ssfSamples); 
		} 
	}


	// read Sliding Windows from the Shared Memory
	else if (dataType == "SLDWINDOWS") {

		while ((frameID = glb_sharedMem.RetrieveFrameIDbyPosition(inputModID, SSF_DATA_SLDWINDOWS, position++)) != SM_INVALID_ELEMENT) {

			ReportStatus("[FrameID: %d] writing detection results at position '%d", frameID, position);

			// retrieve the filename image
			filename = glb_sharedMem.GetFrameFilename(frameID);

			// load results
			ssfSldWins = glb_sharedMem.GetSlidingWindows(inputModID, frameID);

			// write detection results
			WriteDetectionResults(filename, ssfSldWins); 
		} 

	}

	// unknown option
	else {
		ReportError("Invalid dataType '%s'", dataType);
	}
}

//================================================================================
// READING OPERATION
//================================================================================

vector<DetWin> *ModDetResults::LoadDetectionResults(string filename) {
vector<DetWin> *detwins;
DetWin detwin;
InputType inputType;
ifstream infile;
float resp;
char str[1024];

	detwins = new vector<DetWin>();

	inputType = FileParts(filename);

	if (fileSuffix.size() == 0)
		filename = dirName + FILESEP + inputType.name + ".txt";
	else
		filename = dirName + FILESEP + inputType.name + "." + fileSuffix + ".txt";


	infile.open(filename.c_str());

	// if couldn't open file
	if (infile.good() == false) {
		ReportError("File '%s' couldn't be opened", filename.c_str());
	}

	// read every line format: x0, y0, w, h, response
	while (infile.good()) {
		infile.getline (str,1024);
		if (strlen(str) == 0)
			continue;
		sscanf(str, "%d%d%d%d%f", &detwin.x0, &detwin.y0, &detwin.w, &detwin.h, &resp);
		detwin.response = resp;

		// allocate memory for new detwins
		detwins->push_back(detwin);
	}
	
	infile.close();


	return detwins;
}




void ModDetResults::Reading() {
FileManager fm;
size_t format;
string filename;
vector<DetWin> *detwins;
size_t position = 0;
SSFStorage storage;
SSFDetwins *ssfdetwins;
SSFSlidingWindow *sldwin;
size_t frameID;

	ReportStatus("Default operation is deprecated. Update your parameters file to set the desired operation");

	// check if the type is correct
	format = fm.GetInputType(dirName);
	if (format != FILETYPE_DIR)
		ReportError("Input '%s' is not a directory", dirName.c_str());

	// load files
	fm.SetInputFile(dirName, "*.txt");

	// perform detection results for every frame
	while ((frameID = glb_sharedMem.RetrieveFrameIDbyPosition(inputModID, SSF_DATA_STRING, position++)) != SM_INVALID_ELEMENT) {

		ReportStatus("[FrameID: %d] reading detection results at position '%d", frameID, position);

		// retrieve the filename image
		filename = glb_sharedMem.GetFrameFilename(frameID);

		// load the results
		detwins = LoadDetectionResults(filename);

		// create structure for detection windows
		ssfdetwins = new SSFDetwins();
		ssfdetwins->SetDetwins(detwins);

		sldwin = new SSFSlidingWindow();
		sldwin->AddDetwins(ssfdetwins, NULL);

		// set to the shared memory
		glb_sharedMem.SetSlidingWindows(frameID, sldwin);
	} 

}

// execute module
void ModDetResults::Execute() {

    // write detection results
    if (operation == "write") 
		Writing();

    // read detection results
	else if (operation == "read") 
		Reading();

    // operation does not exist
	else 
        ReportError("Operation %s does not exist!", this->operation.c_str());
}
