#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "ModSplitYML.h"


ModSplitYML instance("");

/******
 * class ModSplitYML
 ******/
ModSplitYML::ModSplitYML(string modID) :  SSFUserModule(modID) {

	nTraining = 0;
	modelFileTrain = "Train_File.yml";
	modelFileTest = "Test_File.yml";

	/* Add parameters */
	inputParams.Add("modelFileTrain", &modelFileTrain, "File storing the model to save training samples", true);
	inputParams.Add("modelFileTest", &modelFileTest, "File storing the model to save testing samples", true);
	inputParams.Add("nTraining", &nTraining, "Number of training samples", false);

	/* module information */
	info.Description("Module to generate two ymls from one (??).");

	/* set input and output data */
	modParams.Require(modID, SSF_DATA_OBJECT, &InputMod, "");	// input image
		
	/* register the module */
	this->Register(this, modID);
}

ModSplitYML::~ModSplitYML() {

}

void ModSplitYML::Setup() {

	// check if the input modules have been set correctly
	if (modelFileTrain == "" && modelFileTest == "") 
		ReportError("modelFileTrain and modelFileTest must be set");

	// check if the parameters are set correctly
	if (nTraining <= 0)
		ReportError("Parameter 'nTraining' has to be set");

}

SSFMethod *ModSplitYML::Instantiate(string modID) {

	return new ModSplitYML(modID); 
}


void ModSplitYML::Execute() {

	SMIndexType position = 0;
	int nobject = 0;;
	SSFObject *db_obj;
	SSFTracklet *db_tracklet;
	SSFSample *db_sample;
	vector<SMIndexType> objList, db_tracklets, db_samples;
	vector<size_t>	index;
	vector<SMIndexType> indexTracklet, auxTrein, auxTest;
	vector<vector<SMIndexType> > trainTrack, testTrack;
	fstream fileTrain, fileTest;		
	vector<string> FrameFilename;

	// Read the object id in memory
	while((db_obj = glb_sharedMem.RetrieveObject(InputMod, position++)) != NULL) {

		// Number of the objects
		nobject++;

		ReportStatus("Object %s is in images:\n", db_obj->GetObjID().c_str());
	
		// Read the tracklets id in memory
		db_tracklets = db_obj->GetTrackletIDs();

		if(db_tracklets.size() < nTraining)
			ReportError("Ntraining must be less than the number of tracklets ");

		// Generate a random permutation of tracklets
		index = GenerateRandomPermutation((size_t)db_tracklets.size(), (size_t)nTraining);
		// sort index
		sort(index.begin(), index.begin()+index.size()); 

		// Read the tracklets id in memory
		db_tracklets = db_obj->GetTrackletIDs();

		indexTracklet.clear();

		for(size_t i = 0; i < db_tracklets.size(); i++) {

			// Read tracklets
			db_tracklet = glb_sharedMem.RetrieveTracklet(db_tracklets[i]);

			// Retrieve id of tracklets
			indexTracklet.push_back(db_tracklet->GetTrackletID());

			// Read the samples id in memory
			db_samples = db_tracklet->GetSamplesID();

			for(size_t j = 0; j < db_samples.size(); j++) {
				
				db_sample = glb_sharedMem.GetSample(db_samples[j]);

				// Retrieve the path of the frame
				FrameFilename.push_back(glb_sharedMem.GetFrameFilename(db_sample->GetFrameID()));	
		
			}
		}
		int b=0;
		auxTrein.clear();
		auxTest.clear();

		// Separate the training samples of test samples
		for(int c=0; c<db_tracklets.size(); c++)
		{
			if(index[b] == c)
			{
				size_t x = index[b];
				auxTrein.push_back(indexTracklet[x]);
				if(b < nTraining-1)
					b++;
			}
			else
			{
				auxTest.push_back( indexTracklet[c]);
			}
		}
		trainTrack.push_back(auxTrein);
		testTrack.push_back(auxTest);
	}

	// Save
	fileTrain.open(modelFileTrain, ios::out);
	fileTest.open(modelFileTest, ios::out);
	
	if (!fileTrain.is_open())
		ReportError("Invalid file file (%s)!", modelFileTrain.c_str());
	if (!fileTest.is_open())
		ReportError("Invalid file file (%s)!", modelFileTest.c_str());

	fileTrain << "%YAML:1.0" << endl << "images:" << endl;
	fileTest << "%YAML:1.0" << endl << "images:" << endl;

	for(int k=0; k<FrameFilename.size(); k++) {
		
		fileTrain << "  - { id: " << k+1 <<", file: " << FrameFilename[k] <<", gray: false }"<<endl;
		fileTest << "  - { id: " << k+1 <<", file: " << FrameFilename[k] <<", gray: false }"<<endl;
	}

	fileTrain << "samples:" << endl;
	fileTest << "samples:" << endl;
	for(int k=0; k<FrameFilename.size(); k++) {
		
		fileTrain << "  - { id: " << k+1 <<", source: " << k+1 <<" }"<<endl;
		fileTest << "  - { id: " << k+1 <<", source: " << k+1 <<" }"<<endl;
	}

	fileTrain << "tracklets:" << endl;
	fileTest << "tracklets:" << endl;
	for(int k=0; k<FrameFilename.size(); k++) {
		
		fileTrain << "  - { id: " << k+1 <<", samples: [" << k+1 <<"] }"<<endl;
		fileTest << "  - { id: " << k+1 <<", samples: [" << k+1 <<"] }"<<endl;
	}

	const String quote = "\"";
	fileTrain << "objects:" << endl;
	fileTest << "objects:" << endl;
	for(int k=0; k<nobject; k++) {

		fileTrain << "  - { id: "<< quote << k+1 << quote<<", tracklets: [";
		for(int x=0; x<nTraining; x++)
		{
			size_t valor = trainTrack[k][x];
			if(x == nTraining-1)
				fileTrain << trainTrack[k][x]; 
			else
				fileTrain << trainTrack[k][x] << ", "; 
		}
		fileTrain <<"] }"<<endl;

		fileTest << "  - { id: "<< quote << k+1 << quote<<", tracklets: [";
		for(int x=0; x<testTrack[k].size(); x++)
		{
			size_t valor = testTrack[k][x];
			if(x == testTrack[k].size()-1)
				fileTest << testTrack[k][x];
			else
				fileTest << testTrack[k][x] << ", ";
		}
		fileTest  <<"] }"<<endl;
	}

	fileTrain.close();
	fileTest.close();

}
