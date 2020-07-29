#include "headers.h"
#include "SSFCore.h"
#include "ExtractionControl.h"
#include "LAP.h"
#include "SSFFeatures.h"
#include "SSFDict.h"
#include "SSFInvertedList.h"
#include "ModBowReid.h"
#include "FeatureBlocks.h" 
#include <ctime>  


ModBowReid instance("");

/******
 * class ModBowReid
 ******/
ModBowReid::ModBowReid(string modID) :  SSFUserModule(modID) {

	normalization = "false";
	executionType = "learning";
	setupDict = numberDict = 1;
	output = numberBlock = height = width = nCWs = setupBlock = nObject = -1;

	/* Add parameters */
	inputParams.Add("modelFile", &modelFile, "File storing the model to save or to load", true);
	inputParams.Add("featFile", &featFile, "File containing the feature setup ", true);
	inputParams.Add("FeatureFile", &FeatureFile, "File storing the extracted features", false);
	inputParams.Add("ResultFile", &ResultFile, "File to store the result of the bowreid module", false);
	inputParams.Add("featSetup", &featSetup, "Feature setup that will be considered (must be defined in file featFile)", true);

	inputParams.Add("executionType", &executionType, "Execute type (learning or testing)", true);
	inputParams.Add("normalization", &normalization, "Execute normalization", false);
	inputParams.Add("nCWs", &nCWs, "Number of codewords (only for learning)", false);
	inputParams.Add("nObject", &nObject, "Number of codewords (only for learning)", false);

	inputParams.Add("output", &output, "It indicates whether the output will be a list of probable subjects (multiple) or a (single) response", false);
	inputParams.Add("setupDict", &setupDict, "Configuration: 1 - single dictionary; 2 - a dictionary per block, 3 - multiple dictionaries", false);
	inputParams.Add("setupBlock", &setupBlock, "Configuration: 1 - Weighting block recognition, 2 - Weighting block Euclidean distance , 3 - Weighting block gaussian", false);
	inputParams.Add("numberDict", &numberDict, "The setting (setupDict = 3) is the number of dictionaries to be created", false);

	inputParams.Add("width", &width, "Resize all samples to the same width (default: sample width)", false);
	inputParams.Add("height", &height, "Resize all samples to the same height (default: sample height)", false);
	inputParams.Add("numberBlock", &numberBlock, "", false);
	
	/* module information */
	info.Description("Module to perform person re-identification using visual dictionaries and indexing structures.");

	/* set input and output data */
	modParams.Require(modID, SSF_DATA_OBJECT, &learnInputMod, "");	// tracklets for training samples
	modParams.Require(modID, SSF_DATA_OBJECTCANDIDATES, &testInputMod, ""); // object candidates for test images
	
	modParams.Provide(modID, SSF_DATA_OBJECTCANDIDATES);
	modParams.Provide(modID, SSF_DATA_OBJECT);

	/* register the module */
	this->Register(this, modID);	
}

ModBowReid::~ModBowReid() {


}

void ModBowReid::Setup() {

	// check if the input modules have been set correctly
	if (executionType != "learning" && executionType != "testing")
		ReportError("executionType must be either 'learning' or 'testing'");

	if (executionType == "learning" && learnInputMod == "")
		ReportError("learnInputMod must be set for learning");

	if (executionType == "testing" && testInputMod == "")
		ReportError("testInputMod must be set for testing");

	if (executionType == "testing" && modelFile == "") 
		ReportError("modelFile must be set for testing");

	if (executionType == "learning" && modelFile == "") 
		ReportError("modelFile must be set for learning");

	if (executionType == "testing" && ResultFile == "") 
		ReportError("executionType must be set for testing");

	if (executionType == "learning" && FeatureFile == "") 
		ReportError("FeatureFile must be set for learning");


	// check if the parameters are set correctly
	// learning
	if (executionType == "learning") {
		if (featFile == "")
			ReportError("Parameter 'featFile' has to be set");
		if (featSetup == "")
			ReportError("Parameter 'featSetup' has to be set");
		if (nCWs <= 0)
			ReportError("Parameter 'nCWs' has to be set");
		if (normalization != "true" && normalization != "false")
			ReportError("Parameter 'normalization' must be set correctly");
		if (FeatureFile == "")
			ReportError("Parameter 'FeatureFile' has to be set");
		if (setupDict != 1 && setupDict != 2 && setupDict != 3)
			ReportError("Parameter 'setupDict' has to be set correctly");
		if (setupBlock != 1 && setupBlock != 2 && setupBlock != 3 && setupBlock != -1)
			ReportError("Parameter 'setupBlock' has to be set correctly");
		if (setupDict == 3) {
			if(numberDict < 0)
			ReportError("Parameter 'numberDict' has to be set correctly");
		}
	}
	
	// testing
	else {
		if (featFile == "")
			ReportError("Parameter 'featFile' has to be set");
		if (featSetup == "")
			ReportError("Parameter 'featSetup' has to be set");
		if (normalization != "true" && normalization != "false")
			ReportError("Parameter 'normalization' must be set correctly");
		if (ResultFile == "")
			ReportError("Parameter 'ResultFile' has to be set");
		if (output == 0 || output < -1)
			ReportError("Parameter 'output' must not be zero or less than -1");
		if (setupDict != 1 && setupDict != 2 && setupDict != 3)
			ReportError("Parameter 'setupDict' has to be set correctly");
		if (setupBlock != 1 && setupBlock != 2 && setupBlock != 3 && setupBlock != -1)
			ReportError("Parameter 'setupBlock' has to be set correctly");
		if (nObject <= 0)
			ReportError("Parameter 'nObject' has to be set correctly");
		if (setupDict == 3) {
			if(numberDict < 0)
			ReportError("Parameter 'numberDict' has to be set correctly");
		}
	}
}

SSFMethod *ModBowReid::Instantiate(string modID) {

	return new ModBowReid(modID); 
}

void ModBowReid::Learning() {

	SSFFeatures *features;
	SSFFeatures feat_Object;
	SSFStorage storage, storage2;
	SSFTracklet *tracklet;
	SSFSample *sample;
	SSFObject *obj;
	vector<SMIndexType>::iterator iobjList, itracklets, isamples;
	vector<SMIndexType> objList, tracklets, samples;
	SSFMatrix<FeatType> auxMat;
	SMIndexType position = 0, trackletID;
	string objectID;
	int nobject = 0, control = 0;
	FeatureBlocks *blocks;
	SSFRect window;
	vector<SSFRect *> *blockSelected;
	vector<float> DistBlocks;
	float dist, sumDist=0;
	
	storage.open(FeatureFile, SSF_STORAGE_WRITE);

	if (storage.isOpened() == false)
		ReportError("Invalid file storage (%s)!", FeatureFile.c_str());

	storage << "BowReid_Features" << "{";

	// Read the object id in memory
	while((obj = glb_sharedMem.RetrieveObject(learnInputMod, position++)) != NULL) {

		ReportStatus("Object %s is in images:\n", obj->GetObjID().c_str());

		objectID = obj->GetObjID();

		object.push_back(objectID);

		tracklets = obj->GetTrackletIDs();
		ntracklets = (int)tracklets.size();

		// Reset SSFMatrix of the feature
		feat_Object.Reset();

		// Read tracklets
		for(itracklets = tracklets.begin(); itracklets != tracklets.end(); ++itracklets) {

			// get tracklet
			trackletID = *itracklets;
			tracklet = glb_sharedMem.RetrieveTracklet(trackletID);

			samples = tracklet->GetSamplesID();

			// Read the samples id in memory
			for(isamples = samples.begin(); isamples != samples.end(); ++isamples) {

				// Read sample
				sample = glb_sharedMem.GetSample(*isamples);
				ReportStatus("\t- %s\n", glb_sharedMem.GetFrameFilename(sample->GetFrameID()).c_str());

				// Extract and cropping features from a given sample
				glb_sharedMem.ExtractSampleFeatures(sample->GetSampleID(), featIdx, width, height);

				// Retrieves the extracted features
				features = sample->RetrieveFeatures();
				
				// Adds the feature handles in an array
				feat_bigmatrix.AddFeatures(features->RetrieveFeatures(false));
				feat_Object.AddFeatures(features->RetrieveFeatures(false));	

				///////////////////////////////////////////////////////////////////////////////////////////////
				// TROCAR QUANDO A FUNCAO QUE RETORNA A QUANTIDADES DE BLOCOS FOR IMPLEMENTADA
				///////////////////////////////////////////////////////////////////////////////////////////////

				// Retrieve the number of blocks in which the sample was partitioned
				if(control == 0) {
					feat_aux = feat_Object.RetrieveFeatures(false);
					nBlocks = feat_aux.rows;

					if(setupDict == 3) {
						if(numberDict > nBlocks)
							ReportError("numberDict the variable must be smaller");
					}

					if(setupBlock == 3) {
											
						// Retrieve coordinates of blocks
						window = sample->GetWindow();
						blocks = glb_featExtControl.RetriveActualBlocks(featIdx, window); 

						// Calculate Centroid of sample
						Point centroidSample(window.x0 + (window.w / 2), window.y0 + (window.h / 2));

						// Calculate Centroid of blocks
						blockSelected = blocks->RetrieveSelectedBlocks();
					
						for(int x=0; x<nBlocks; x++) {

							window = *blockSelected->at(x);
							Point centroidBlock(window.x0 + (window.w / 2), window.y0 + (window.h / 2));
					
							// Calculates the distance between the centroid of the block and the centroid of the sample
							dist = Distance(centroidBlock, centroidSample);
							DistBlocks.push_back(dist);
							sumDist = sumDist + dist;
						}
					}
				}
				control++;
			}	
		}

		// Retrieve extracted features (if singleRow == true, return a single row matrix)
		feat_aux = feat_Object.RetrieveFeatures(false);

		// Storing the features retrieved in a vector
		auxMat.create(0, feat_aux.cols);
		vector_feat.push_back(auxMat);
		vector_feat[nobject].push_back(feat_aux);

		storage << "Object"  + IntToString(nobject+1) << nobject+1;
		storage << "Feature" << feat_aux;

		nobject++;
	}

	storage << "}";
	storage.release();

	if(setupBlock == 3) {

		storage2.open("WeightBlocks.yml", SSF_STORAGE_WRITE);

		if (storage2.isOpened() == false)
			ReportError("Invalid file storage (%s)!", FeatureFile.c_str());

		storage2 << "Weight_Blocks" << "{";
		storage2 << "Data" << "[";

		// Calculates the weight of the blocks
		for(int x=0; x<nBlocks; x++) {

			double weight = ((DistBlocks[x]*100) / sumDist);
			storage2 << "{:" << "block" << x+1 << "weight" << weight << "}";
		}
		storage2 << "]" << "}";
		storage2.release();
	}

	if(setupDict == 1)
		LearningSingleDictionary(nobject);
	else if(setupDict == 2)
		LearningBlockDictionary(nobject);
	else if(setupDict == 3)
		LearningMultipleDictionary(nobject);
}

void ModBowReid::LearningSingleDictionary(int nobject) {
 
 	SSFStorage storage;
 	SSFDict *dict;
	vector<string> featID, featureID;
 	vector<double> distance, MinDist;
	SSFMatrix<FeatType> BigMatrix, BMmean, BMstd, zdataV, auxMat, feature, matrix;
 	static SSFMatrix<FeatType> aux, aux1;
 	data Data;

	storage.open("BigMatrix.yml", SSF_STORAGE_WRITE);
	if (storage.isOpened() == false)
		ReportError("Invalid file storage!");
	storage << "Features" << "{";
	storage << "BigMatrix" << BigMatrix;
	storage << "}";
	storage.release();

	storage.open(modelFile, SSF_STORAGE_WRITE);

	if (storage.isOpened() == false)
		ReportError("Invalid file storage (%s)!", modelFile.c_str());

	storage << "BowReid_Model" << "{";

	// Create SSFMatrix <FeatType> BigMatrix to build the dictionary 
 	BigMatrix = feat_bigmatrix.RetrieveFeatures(false);

	// Normalization
	if(normalization == "true") {

		BMmean = SSFMatrixMean(BigMatrix);
		BMstd = SSFMatrixStd(BigMatrix);
		SSFMatrixZscore(BigMatrix, BMmean, BMstd);

		storage << "BMmean" << BMmean;
		storage << "BMstd" << BMstd ;
	}

	storage << "nBlocks" << nBlocks;

	feature.create(nobject*ntracklets, feat_aux.cols);	

 	// Create the dictionary
 	Dict = new SSFDict();
 	Dict->BuildDictionary(BigMatrix, nCWs);
	
	// Creates an inverted list for each block
	for(int x=0; x<nBlocks; x++) {
		
		dict = Dict->Clone();

		storage << "Block"  + IntToString(x+1) << "{";
		ReportStatus("\t- creates inverted list %d\n", x+1);

		int subject = 0;
		for(int i=0; i<nobject;i++) {

			matrix = vector_feat[i];
			
			// zscore normalization 
			if(normalization == "true"){

				if(ntracklets > 1) {
					for(int b=1; b<ntracklets;b++) {
						aux1 = matrix.row(x+(nBlocks*b));
						zdataV = aux1 - BMmean;
						zdataV /= BMstd;
						zdataV.copyTo(feature.row(subject+(nobject*b)));
					}
				}
				else {
					aux = matrix.row(x);
					zdataV = aux - BMmean;
					zdataV /= BMstd;
					zdataV.copyTo(feature.row(subject));
				}

			}
			else{
				if(ntracklets > 1) {
					for(int b=1; b<ntracklets;b++) {
						matrix.row(x).copyTo(feature.row(subject+(nobject*b)));
					}
				}
				else
					matrix.row(x).copyTo(feature.row(subject));
			}

			// Creates featId
			featID.push_back(object[subject]);
			subject++;
		}
		
		if(ntracklets > 1) {
			for(int q=0; q<ntracklets; q++) {
				for(int p=0; p<nobject; p++) {
					featureID.push_back(featID[p]);
				}
			}
		}

		// Create inverted list
 		list = new SSFInvertedList();
		if(ntracklets > 1)
			list->CreatList(feature, featureID, nCWs, dict);
		else
			list->CreatList(feature, featID, nCWs, dict);
 		list->Sort();
 		list->Save(storage);
 
 		featureID.clear();
		featID.clear();
 		storage << "}";
 		delete list;
 	}
 	delete Dict;
 
 	storage << "}";
 	storage.release();
}

void ModBowReid::LearningBlockDictionary(int nobject) {

	SSFStorage storage;
	vector<string> featID, featureID;
	SSFMatrix<FeatType> BigMatrix, BMmean, BMstd, zdataV, auxMat, feature, matrix;
	static SSFMatrix<FeatType> aux, aux1;

	storage.open("BigMatrix.yml", SSF_STORAGE_WRITE);
	if (storage.isOpened() == false)
		ReportError("Invalid file storage!");

	storage1.open(modelFile, SSF_STORAGE_WRITE);
	if (storage1.isOpened() == false)
		ReportError("Invalid file storage (%s)!", modelFile.c_str());

	storage1 << "BowReid_Model" << "{";
	storage1 << "nBlocks" << nBlocks;
	
	// Creates a dictionary for each block
	for(int x=0; x<nBlocks; x++) {

		storage1 << "Block"  + IntToString(x+1) << "{";
		BigMatrix.release();

		// Create SSFMatrix <FeatType> BigMatrix to build the dictionary
		for(int i=0; i<nobject;i++) {

			matrix = vector_feat[i];
			BigMatrix.push_back(matrix.row(x));
		}
		storage << "Features Block" + IntToString(x+1) << "{";
		storage << "BigMatrix" << BigMatrix;
		storage << "}";

		// Normalization
		if(normalization == "true") {

			BMmean = SSFMatrixMean(BigMatrix);
			BMstd = SSFMatrixStd(BigMatrix);
			SSFMatrixZscore(BigMatrix, BMmean, BMstd);

			storage1 << "BMmean" << BMmean;
			storage1 << "BMstd" << BMstd ;
		}

		feature.create(nobject*ntracklets, feat_aux.cols);	

		// Create the dictionary
		Dict = new SSFDict();
		Dict->BuildDictionary(BigMatrix, nCWs);

		// Creates an inverted list for each block
		ReportStatus("\t- creates inverted list %d\n", x+1);

		int subject = 0;
		for(int i=0; i<nobject;i++) {

			matrix = vector_feat[i];

			// zscore normalization 
			if(normalization == "true"){
				if(ntracklets > 1) {
					for(int b=1; b<ntracklets;b++) {
						aux1 = matrix.row(x+(nBlocks*b));
						zdataV = aux1 - BMmean;
						zdataV /= BMstd;
						zdataV.copyTo(feature.row(subject+(nobject*b)));
					}
				}
				else {
					aux = matrix.row(x);
					zdataV = aux - BMmean;
					zdataV /= BMstd;
					zdataV.copyTo(feature.row(subject));
				}
			}
			else{
				if(ntracklets > 1) {
					for(int b=1; b<ntracklets;b++) {
						matrix.row(x).copyTo(feature.row(subject+(nobject*b)));
					}
				}
				else
					matrix.row(x).copyTo(feature.row(subject));
			}

			// Creates featId
			featID.push_back(object[subject]);
			subject++;
		}

		if(ntracklets > 1) {
			for(int q=0; q<ntracklets; q++) {
				for(int p=0; p<nobject; p++) {
					featureID.push_back(featID[p]);
				}
			}
		}

		// Create inverted list
 		list = new SSFInvertedList();
		if(ntracklets > 1)
			list->CreatList(feature, featureID, nCWs, Dict);
		else
			list->CreatList(feature, featID, nCWs, Dict);
		list->Sort();
		list->Save(storage1);

		featureID.clear();
		featID.clear();
		storage1 << "}";

		delete list;
	}
	storage1 << "}";
	storage1.release();
	storage.release();
}

void ModBowReid::LearningMultipleDictionary(int nobject) {

	SSFStorage storage;
	SSFDict *dict;
	vector<string> featID, featureID;
	SSFMatrix<FeatType> BigMatrix, BMmean, BMstd, zdataV, auxMat, feature, matrix;
	static SSFMatrix<FeatType> aux, aux1;

	// Create SSFMatrix <FeatType> BigMatrix to build the dictionary 
	BigMatrix = feat_bigmatrix.RetrieveFeatures(false);

	storage.open("BigMatrix.yml", SSF_STORAGE_WRITE);
	if (storage.isOpened() == false)
		ReportError("Invalid file storage!");
	storage << "Features" << "{";
	storage << "BigMatrix" << BigMatrix;
	storage << "}";
	storage.release();

	storage.open(modelFile, SSF_STORAGE_WRITE);

	if (storage.isOpened() == false)
		ReportError("Invalid file storage (%s)!", modelFile.c_str());

	storage << "BowReid_Model" << "{";

	// Normalization
	if(normalization == "true") {

		BMmean = SSFMatrixMean(BigMatrix);
		BMstd = SSFMatrixStd(BigMatrix);
		SSFMatrixZscore(BigMatrix, BMmean, BMstd);

		storage << "BMmean" << BMmean;
		storage << "BMstd" << BMstd ;
	}

	storage << "nBlocks" << nBlocks;

	feature.create(nobject*ntracklets, feat_aux.cols);

	for(int a=0; a<numberDict; a++) {

		storage << "Dict" + IntToString(a+1) << "{";

		// Create the dictionary
		Dict = new SSFDict();
		Dict->BuildDictionary(BigMatrix, nCWs);

		// Creates an inverted list for each block
		for(int x=0; x<nBlocks; x++) {

			dict = Dict->Clone();

			storage << "Block"  + IntToString(x+1) << "{";
			ReportStatus("\t- creates inverted list %d\n", x+1);

			int subject = 0;
			for(int i=0; i<nobject;i++) {

				matrix = vector_feat[i];

				// zscore normalization 
				if(normalization == "true"){
					if(ntracklets > 1) {
						for(int b=1; b<ntracklets;b++) {
							aux1 = matrix.row(x+(nBlocks*b));
							zdataV = aux1 - BMmean;
							zdataV /= BMstd;
							zdataV.copyTo(feature.row(subject+(nobject*b)));
						}
					}
					else {
						aux = matrix.row(x);
						zdataV = aux - BMmean;
						zdataV /= BMstd;
						zdataV.copyTo(feature.row(subject));
					}
				}
				else{
					if(ntracklets > 1) {
						for(int b=1; b<ntracklets;b++) {
							matrix.row(x).copyTo(feature.row(subject+(nobject*b)));
						}
					}
					else
						matrix.row(x).copyTo(feature.row(subject));
				}

				// Creates featId
				featID.push_back(object[subject]);
				subject++;
			}

			if(ntracklets > 1) {
				for(int q=0; q<ntracklets; q++) {
					for(int p=0; p<nobject; p++) {
						featureID.push_back(featID[p]);
					}
				}
			}

			// Create inverted list
 			list = new SSFInvertedList();
			if(ntracklets > 1)
				list->CreatList(feature, featureID, nCWs, dict);
			else
				list->CreatList(feature, featID, nCWs, dict);
			list->Sort();
			list->Save(storage);

			featureID.clear();
			featID.clear();
			storage << "}";
			delete list;
		}
		storage << "}";
		delete Dict;
	}
	
	storage << "}";
	storage.release();
}

void ModBowReid::Testing() {

	SSFFeatures *features;
	SSFFeatures feat_Object;
	SSFObjectCandidates *objCandidates;
	SSFObjectCandidates *Candidates;
	SSFTracklet *tracklet;
	SSFSample *sample;
	SSFStorage storage, storage3;
	SMIndexType trackletID, position = 0;
	vector<SMIndexType> objList, samples;
	vector<SMIndexType>::iterator isamples;
	FileNode node, n, n1, node1, n2, n3;
	SSFMatrix<FeatType> feat_aux, BMmean, BMstd, auxMat;
	int idx = 0;

	storage.open(modelFile, SSF_STORAGE_READ);
	if (storage.isOpened() == false)
		ReportError("Invalid file storage (%s)!", modelFile.c_str());

	if(setupDict == 1 || setupDict == 3) {
		node = storage.root();
		n = node["BowReid_Model"];
		n["BMmean"] >> BMmean;
		n["BMstd"] >> BMstd ;
		n["nBlocks"] >> nBlocks;
	
	}
	else if(setupDict == 2) {
	
		node = storage.root();
		n = node["BowReid_Model"];
		n["nBlocks"] >> nBlocks;
	}

	storage1.open(ResultFile, SSF_STORAGE_WRITE);
	if (storage1.isOpened() == false)
		ReportError("Invalid file storage!");

	storage1 << "Result_BowReid" << "{";
	storage1 << "nBlocks" << nBlocks;

	if(setupBlock == 3) {

		storage3.open("WeightBlocks.yml", SSF_STORAGE_READ);
		if (storage3.isOpened() == false)
			ReportError("Invalid file storage!");

		node1 = storage3.root();
		n2 = node1["Weight_Blocks"];
		n3 = n2["Data"];
		FileNodeIterator it = n3.begin(), it_end = n3.end();

		for( ; it != it_end; ++it)
		{
			int s = (int)(*it)["block"];
			WeightBlock.push_back((double)(*it)["weight"]);
		}
		storage3.release();
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// se objcandidates estiver vazio considera todos se nao considera os que estao dentro
	///////////////////////////////////////////////////////////////////////////////////////////////

	// Read object Candidates id in memory
	while ((Candidates = glb_sharedMem.GetObjectCandidates(testInputMod, position++)) != NULL ) {

		// get tracklet
		trackletID = Candidates->GetTrackletID();
		tracklet = glb_sharedMem.RetrieveTracklet(trackletID);
		tracklets.clear();
		tracklets.push_back(trackletID);

		// Reset SSFMatrix of the feature
		feat_Object.Reset();
		objList.clear();

		// Read the samples id in memory
		samples = tracklet->GetSamplesID();

		for(isamples = samples.begin(); isamples != samples.end(); ++isamples) {

			// Read sample
			sample = glb_sharedMem.GetSample(*isamples);
			ReportStatus("\t- %s\n", glb_sharedMem.GetFrameFilename( sample->GetFrameID()).c_str());

			// Extract and cropping features from a given sample
			glb_sharedMem.ExtractSampleFeatures(sample->GetSampleID(), featIdx, width, height);

			// Retrieves the extracted features
			features = sample->RetrieveFeatures();

			// Adds the feature handles in an array
			feat_Object.AddFeatures(features->RetrieveFeatures(false));
		}	

		// Retrieve extracted features (if singleRow == true, return a single row matrix)
		feat_aux = feat_Object.RetrieveFeatures(false);

		// Storing the features retrieved in a vector
		auxMat.create(0, feat_aux.cols);
		vector_feat.push_back(auxMat);
		vector_feat[0].push_back(feat_aux);

		if(setupDict == 1)
			TestingSingleDictionary(idx, n, BMmean, BMstd, numberBlock);
		else if(setupDict == 2)
			TestingBlockDictionary(idx, n, BMmean, BMstd, numberBlock);
		else if(setupDict == 3) {
			for(int a=0; a<numberDict; a++) {

				n1 = n["Dict" + IntToString(a+1)];
				storage1 << "Dict " + IntToString(a+1) << "{" ;
				TestingSingleDictionary(idx, n1, BMmean, BMstd, numberBlock);
				storage1 << "}";
			}
		}

		// Multimapping
		for (map<string, double>::iterator it = mapping.begin() ; it != mapping.end(); it++) {
			
			if(setupBlock == 2) {
				double val = (it->second) / map_aux[it->first];
				multimap.insert(pair<double, string>(val, it->first));
			}
			else
				multimap.insert(pair<double, string>(it->second, it->first));
		}

		// write candidates
		WriteCandidates(idx);

		// create candidates
		objCandidates = new SSFObjectCandidates(trackletID, candidates);

		// write to shared mem.
		glb_sharedMem.SetObjectCandidates(objCandidates);

		candidates.clear();
		vector_feat.clear();
		mapping.clear();
		multimap.clear();
		idx++;
	}

	PrintInvertedList();

	storage1 << "}";
	storage.release();
	storage1.release();

	for(int i=0; i<SubjectList.size(); i++)
		delete SubjectList[i];
}

void ModBowReid::TestingSingleDictionary(int idx, const FileNode &n, SSFMatrix<FeatType> BMmean, SSFMatrix<FeatType> BMstd, int block) {

	SSFMatrix<FeatType> zdataV, matrix;
	static SSFMatrix<FeatType> aux;
	vector<data > *result;
	FileNode n2;
	int tam;

	storage1 << "Sample " + IntToString(idx+1) << "{" ;

	if(setupBlock == 1)
		nBlocks = 1;

	for(int x=0; x<nBlocks; x++) {

		if(setupBlock == 1)
			n2 = n["Block" + IntToString(block+1)];
		else
			n2 = n["Block" + IntToString(x+1)];
		
		list = new SSFInvertedList();

		// Load InvertedList
		list->Load(n2);

		if(setupBlock == 1)
			storage1 << "Block " + IntToString(block+1) << "{" ;
		else
			storage1 << "Block " + IntToString(x+1) << "{" ;

		matrix = vector_feat[0];

		if(setupBlock == 1)
			aux = matrix.row(block);
		else
			aux = matrix.row(x);

		// zscore normalization 
		if(normalization == "true") {
				
			zdataV = aux - BMmean;
			zdataV /= BMstd;

			result = list->GetTestFeat(zdataV);
		}
		else {
			if(setupBlock == 1)
				aux = matrix.row(block);
			else
				aux = matrix.row(x);

			result = list->GetTestFeat(aux);
		}

		tam = (int)result->size();
		storage1 << "size" << tam << "Data" << "[";
			
		// Mapping
		Mapping(result, x);

		storage1 << "]" << "}";
		delete list;
	}
	storage1 << "}";
		
}

void ModBowReid::TestingBlockDictionary(int idx, const FileNode &n, SSFMatrix<FeatType> BMmean, SSFMatrix<FeatType> BMstd, int block) {

	SSFMatrix<FeatType> zdataV, matrix;
	static SSFMatrix<FeatType> aux;
	vector<data > *result;
	FileNode n1;
	int tam;

	if(setupBlock == 1)
		nBlocks = 1;

	for(int x=0; x<nBlocks; x++) {

		storage1 << "Dict " + IntToString(x+1) << "{" ;

		n1 = n["Block" + IntToString(x+1)];
		n1["BMmean"] >> BMmean;
		n1["BMstd"] >> BMstd ;
			
		list = new SSFInvertedList();

		// Load InvertedList
		list->Load(n1);

		storage1 << "List " + IntToString(idx+1) << "{" ;

		matrix = vector_feat[0];

		if(setupBlock == 1)
			aux = matrix.row(block);
		else
			aux = matrix.row(x);

		// zscore normalization 
		if(normalization == "true") {
				
			zdataV = aux - BMmean;
			zdataV /= BMstd;

			result = list->GetTestFeat(zdataV);
		}
		else {
			if(setupBlock == 1)
				aux = matrix.row(block);
			else
				aux = matrix.row(x);

			result = list->GetTestFeat(aux);
		}

		tam = (int)result->size();
		storage1 << "size" << tam << "Data" << "[";
			
		// Mapping
		Mapping(result, x);

		storage1 << "]" << "}" << "} ";
		delete list;
	}
}

void ModBowReid::Mapping(vector<data > *result, int block) {

	vector<data >::iterator it;
	map<string, double>::iterator itmap;
	
	if(setupBlock == 3) {
		for (it = result->begin() ; it != result->end(); it++) {
			data objID = *it;
			storage1 << "{:" << "id" << objID.ID << "dist" << objID.Dist << "}";

			// Mapping the result to the id of tracklet
			itmap = mapping.find(objID.ID);
			if(itmap != mapping.end()) 
				mapping[objID.ID] = mapping[objID.ID] + WeightBlock[block];
			else
				mapping.insert(pair<string, double>(objID.ID, WeightBlock[block]));
		}
	}
	else if(setupBlock == 2) {
		for (it = result->begin() ; it != result->end(); it++) {
			data objID = *it;
			storage1 << "{:" << "id" << objID.ID << "dist" << objID.Dist << "}";

			// Mapping the result to the id of tracklet
			itmap = mapping.find(objID.ID);
			double val = 1.0 - objID.Dist;
			if(itmap != mapping.end()) {
				mapping[objID.ID] = mapping[objID.ID] + val;
				map_aux[objID.ID]++;
			}
			else {
				mapping.insert(pair<string, double>(objID.ID, val));
				map_aux.insert(pair<string, int>(objID.ID, 1));
			}
		}
	}
	else {
		for (it = result->begin() ; it != result->end(); it++) 
		{
			data objID = *it;
			storage1 << "{:" << "id" << objID.ID << "dist" << objID.Dist << "}";

			// Mapping the result to the id of tracklet
			itmap = mapping.find(objID.ID);
			if(itmap != mapping.end())
				mapping[objID.ID]++;
			else
				mapping.insert(pair<string, double>(objID.ID, 1.0));
		}
	}
}

void ModBowReid::WriteCandidates(int idx) {

	vector<string> *v;
	vector<size_t> index;
	std::multimap<double, string>::reverse_iterator it;

	v = new vector<string>;
	SubjectList.push_back(v);

	int idy = 0;

	// Results contains all possible subject
	if(output == -1) {
		for (it = multimap.rbegin() ; it != multimap.rend(); it++) {
			
			string str = it->second;
			double conf = it->first;
			candidate.objCandidate = glb_sharedMem.CreateObject(tracklets, str);
			candidate.confidence = (FeatType)(conf);
			candidates.push_back(candidate);

			SubjectList[idx]->push_back(it->second);
		}
	} 
	else if(output > multimap.size()) {
		
		for (it = multimap.rbegin() ; it != multimap.rend(); it++) {

			string str = it->second;
			double conf = it->first;
			candidate.objCandidate = glb_sharedMem.CreateObject(tracklets, str);
			candidate.confidence = (FeatType)(conf);
			candidates.push_back(candidate);
				
			SubjectList[idx]->push_back(it->second);
		}

		index = GenerateRandomPermutation((size_t)nObject, (size_t)nObject);
		int tam = (int)(output - multimap.size());
		for(int k=0; k<tam; k++) {
			
			// Convert int to string
			int aux = (int)index[k];
			ostringstream convert;			// stream used for the conversion
			convert << aux;				 // insert the textual representation of 'Number' in the characters in the stream

			candidate.objCandidate = glb_sharedMem.CreateObject(tracklets, convert.str());
			candidate.confidence = (FeatType)(0);
			candidates.push_back(candidate);

			SubjectList[idx]->push_back(convert.str());
		}
	} // Results contains n possible subjec
	else {
		for (it = multimap.rbegin() ; it != multimap.rend(); it++) {
				
			if(idy < output) {

				string str = it->second;
				double conf = it->first;
				candidate.objCandidate = glb_sharedMem.CreateObject(tracklets, str);
				candidate.confidence = (FeatType)(conf);
				candidates.push_back(candidate);
				
				SubjectList[idx]->push_back(it->second);
			}
			else
				break;
			idy++;
		}
	}
}

void ModBowReid::PrintInvertedList() {

	SSFStorage storage;
	vector<string >::iterator it;

	storage.open("Result_Candidates_Object.yml", SSF_STORAGE_WRITE);

	if (storage.isOpened() == false)
		ReportError("Invalid file storage!");
	
	// print Inverted List for each list
	int idx = 1;
	for(int i=0; i<SubjectList.size(); i++)
	{
		// new level
		storage << "Object " + IntToString(idx++) << "{";
		storage << "nCandidates " << (int) SubjectList[i]->size() << "id" << "[:";
		for (it = SubjectList[i]->begin(); it != SubjectList[i]->end(); it++) 
		{
			string p = *it;
			storage << p ;
		}
		// return the level
		storage << "]" << "}";
	}

	storage.release();
}

float ModBowReid::Distance(CvPoint pt1, CvPoint pt2) {

	int dx = pt2.x - pt1.x;
	int dy = pt2.y - pt1.y;
	return cvSqrt((float)(dx*dx + dy*dy));
}

void ModBowReid::Execute() {

	// setup feature extraction methods
	featIdxMap = glb_featExtControl.Setup(featFile);
	
	// obtain the feature index that will be used to perform the feature extraction
	featIdx = glb_featExtControl.RetrieveSetupIdx(featIdxMap[featSetup]);

	// seed
	srand (unsigned ( std::time(0) ));

	if (executionType == "learning") 
		Learning();
	else if (executionType == "testing") 
		Testing();	
	else 
		ReportError("Invalid executionType '%s'", executionType.c_str());
}
