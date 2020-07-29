#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "files.h"
#include "ModFileList.h"


ModFileList instance("");

ModFileList::ModFileList(string modID) : SSFUserModule(modID) {

	/* set default values */
	loadGrayscale = false;
	rSize.width   = -1;
	rSize.height  = -1;

	/* Add parameters */
	inputParams.Add("inputName", &inputName,     "File name of the yml",                     true);
	inputParams.Add("width",     &rSize.width,   "Scale readed images to a specific width",  false);
	inputParams.Add("height",    &rSize.height,  "Scale readed images to a specific height", false);
	inputParams.Add("gray",      &loadGrayscale, "Read images as grayscale",                 false);

	/* module information */
	info.Description("Module to load an YML with results or object descriptions.");

	/* set input and output data */
	modParams.Provide(modID, SSF_DATA_FEED);			// feed that will be created
	modParams.Provide(modID, SSF_DATA_FRAME);			// frames that will be created
	modParams.Provide(modID, SSF_DATA_IMAGE);			// images that will be created
	modParams.Provide(modID, SSF_DATA_STRING);			// image filenames that will be created
	modParams.Provide(modID, SSF_DATA_SAMPLE);			// one sample that will be created
	modParams.Provide(modID, SSF_DATA_SAMPLES);			// sample set that will be created
	modParams.Provide(modID, SSF_DATA_OBJECT);			// object that will be created
	modParams.Provide(modID, SSF_DATA_TRACKLET);		// tracklets that will be created

	/* register the module */
	this->Register(this, modID);
}


ModFileList::~ModFileList() {
	// nothing
}



SSFMethod *ModFileList::Instantiate(string modID) {

	return new ModFileList(modID); 
}


void ModFileList::Setup() {


}


FileNode& ModFileList::ReadStorage(FileNode &node, string name, FileNode &value) {

	value = node[name];
	if (value.isNone())
		ReportError("Error processing file '%s': a node '%s' is missing somewhere.", inputName.c_str(), name.c_str());
	return value;
}

// execute module
void ModFileList::Execute() {

// temp vars
int i;
SMIndexType cframe;
ostringstream oss;
string path, objID, mask, object;

Mat cv_image;
Size cv_size;
VideoCapture cv_vcapture;
FileManager *fm = NULL;

SMIndexType frame;
SSFImage *img;

SSFRect rect;
SSFSample* smp;

unordered_map<SMIndexType, vector<SMIndexType> > frameIDsmps;
unordered_map<SMIndexType, vector<SMIndexType> >::iterator iframeIDsmps;
SMIndexType idSample;

vector<int> fileIDList;
vector<int>::iterator ifileIDList;

SSFTracklet *tracklet;

vector<SMIndexType> vecIDs;

vector<SMIndexType> objList;

SMIndexType fileID, srcID, feedID, ssfID, frameID; // ids
// --

// file storage vars
FileStorage      fs;
FileNode         node;
FileNodeIterator inode;
// --


// control maps
unordered_set<int> objectSet; // used to verify if samples for a folder or video was already generated
unordered_map<SMIndexType, vector<SSFImage*> > fileID2images; // map file source id (video or image) to ssf images
vector<SSFImage*> imgs;                                  // store images of a file (multiple images if file is a video)

unordered_map<SMIndexType, SSFSample*> fileID2samples;        // map file sample id to ssf samples

unordered_map<SMIndexType, SMIndexType> fileID2tracklets;          // map file tracklet id to ssf tracklet
// --


	// init: create feed and open file
	feedID = glb_sharedMem.CreateFeed(SSF_DATA_IMAGE);
	if (!fs.open(inputName, FileStorage::READ))
		ReportError("Input '%s' is not a valid file", inputName.c_str());



	// process images (if any)
	node = fs["images"];
	for( inode = node.begin(); inode != node.end(); ++inode ) {

		// load image
		path = (*inode)["file"];
		imgs.clear();
		ssfID = glb_sharedMem.CreateFrame(feedID);
		glb_sharedMem.SetFrameFilename(ssfID, path);

		cv_image = imread(path,
			(*inode)["gray"].isNone() ?
				(loadGrayscale ? 0 : -1) :
				((string)(*inode)["gray"]) == "true" ?
					(IMREAD_GRAYSCALE) :
					(IMREAD_UNCHANGED)
		);
		if( cv_image.empty() )
			ReportError("Error processing file '%s': can't read image %s", inputName.c_str(), path);

		// get size
		if( !(*inode)["height"].isNone() || !(*inode)["width"].isNone() ) {
			cv_size.width  = (*inode)["height"].isNone() ? -1 : (int) (*inode)["height"];
			cv_size.height = (*inode)["width"].isNone()  ? -1 : (int) (*inode)["width"];
		}
		else
			cv_size = rSize;

		// check if image needs to be resized
		if( (cv_size.width != -1) || (cv_size.height != -1) ) {

			if( cv_size.width == -1 )
				cv_size.width  = (int) ((cv_size.height / (float) cv_image.rows) * cv_image.cols);
			else if( cv_size.height == -1 )
				cv_size.height = (int) ((cv_size.width / (float) cv_image.cols) * cv_image.rows);
			resize(cv_image, cv_image, cv_size);
		}
		
		img = new SSFImage(cv_image);

		imgs.push_back(img);
		glb_sharedMem.SetFrameImage(ssfID, img);

		// link file id to ssf frame id
		fileID = (int)(*inode)["id"];
		if( fileID2images.count( fileID ) )
			ReportError("Error processing file '%s': duplicated image/video id %d", inputName.c_str(), fileID);
		fileID2images[ fileID ] = imgs;
	}



	// process folders (if any)
	fm = new FileManager();
	node = fs["folders"];
	for( inode = node.begin(); inode != node.end(); ++inode ) {

		// load folder
		path = (string) (*inode)["dir"];
		mask = (*inode)["mask"].isNone() ? "*.*" : (string) (*inode)["mask"];
		object = (*inode)["object"].isNone() ? "" : (string) (*inode)["object"];;

		if (fm->GetInputType(path) != FILETYPE_DIR) {
			delete fm;
			ReportError("Error processing file '%s': '%s' is not a folder with images", inputName.c_str(), path);
		}

		// load files
		fm->SetInputFile(path, mask);

		// set all images to the feed
		imgs.clear();
		for (i = 0; i < fm->GetNEntries(); i++) {

			// load image
			ssfID = glb_sharedMem.CreateFrame(feedID);
			glb_sharedMem.SetFrameFilename(ssfID, fm->RetrieveEntry(i));

			cv_image = imread(fm->RetrieveEntry(i),
				((string)(*inode)["gray"]) == "true" ?
					(IMREAD_GRAYSCALE) :
					(IMREAD_UNCHANGED)
			);
			if( cv_image.empty() ) {
				delete fm;
				ReportError("Error processing file '%s': can't read image %s in folder %s", inputName.c_str(), fm->RetrieveEntry(i), path);
			}

			// get size
			if( !(*inode)["height"].isNone() || !(*inode)["width"].isNone() ) {
				cv_size.width  = (*inode)["height"].isNone() ? -1 : (int) (*inode)["height"];
				cv_size.height = (*inode)["width"].isNone()  ? -1 : (int) (*inode)["width"];
			}
			else
				cv_size = rSize;

			// check if image needs to be resized
			if( (cv_size.width != -1) || (cv_size.height != -1) ) {

				if( cv_size.width == -1 )
					cv_size.width  = (int) ((cv_size.height / (float) cv_image.rows) * cv_image.cols);
				else if( cv_size.height == -1 )
					cv_size.height = (int) ((cv_size.width / (float) cv_image.cols) * cv_image.rows);
				resize(cv_image, cv_image, cv_size);
			}
		
			img = new SSFImage(cv_image);

			imgs.push_back(img);
			glb_sharedMem.SetFrameImage(ssfID, img);

			// create object for this image if object folder was informed
			if( !object.empty() ) {
				vecIDs.clear();
				// create a new sample
				smp = new SSFSample();
				rect.y0 = rect.x0 = 0;
				rect.h = img->GetNRows();
				rect.w = img->GetNCols();
				smp->SetPatch(rect);
				// set image sample
				vecIDs.push_back( glb_sharedMem.AddFrameSample(ssfID, smp) );
				glb_sharedMem.EndFrameSamples(ssfID);

				// create a new tracklet
				tracklet = new SSFTracklet();
				tracklet->SetSamples(vecIDs);
				vecIDs.clear();
				vecIDs.push_back( glb_sharedMem.SetTracklet(tracklet) );

				// create object
				glb_sharedMem.CreateObject(vecIDs, object);

			}
		}

		if( object.empty() ) {
			// link file id to ssf frame id
			fileID = (int)(*inode)["id"];
			if( fileID2images.count( fileID ) ) {
				delete fm;
				ReportError("Error processing file '%s': duplicated image/video id %d", inputName.c_str(), fileID);
			}
			fileID2images[ fileID ] = imgs;
		}
		else{
			// register as already generated
			objectSet.insert((int)(*inode)["id"]);
		}
	}
	delete fm;



	// process videos (if any)
	node = fs["videos"];
	for( inode = node.begin(); inode != node.end(); ++inode ) {

		// load video
		path = (*inode)["file"];

		cv_vcapture.open(path);
		if( !cv_vcapture.isOpened() )
			ReportError("Error processing file '%s': can't read video %s", inputName.c_str(), path);

		cframe = 0;
		imgs.clear();
		while(cv_vcapture.read(cv_image)) {

			ssfID = glb_sharedMem.CreateFrame(feedID);
			oss.str("");
			oss.clear();
			oss << path << "/" << cframe++;
			glb_sharedMem.SetFrameFilename(ssfID, oss.str());

			if( (*inode)["gray"].isNone() ) {
				if( loadGrayscale )
					cvtColor(cv_image, cv_image, CV_RGB2GRAY);
			}
			else {
				if( ((string)(*inode)["gray"]) == "true" )
					cvtColor(cv_image, cv_image, CV_RGB2GRAY);
			}

			// get size
			if( !(*inode)["height"].isNone() || !(*inode)["width"].isNone() ) {
				cv_size.width  = (*inode)["height"].isNone() ? -1 : (int) (*inode)["height"];
				cv_size.height = (*inode)["width"].isNone()  ? -1 : (int) (*inode)["width"];
			}
			else
				cv_size = rSize;

			// check if image needs to be resized
			if( (cv_size.width != -1) || (cv_size.height != -1) ) {

				if( cv_size.width == -1 )
					cv_size.width  = (int) ((cv_size.height / (float) cv_image.rows) * cv_image.cols);
				else if( cv_size.height == -1 )
					cv_size.height = (int) ((cv_size.width / (float) cv_image.cols) * cv_image.rows);
				resize(cv_image, cv_image, cv_size);
			}
		
			img = new SSFImage(cv_image);

			imgs.push_back(img);
			glb_sharedMem.SetFrameImage(ssfID, img);

			// create object for this image if object folder was informed
			if( !object.empty() ) {
				vecIDs.clear();
				// create a new sample
				smp = new SSFSample();
				rect.y0 = rect.x0 = 0;
				rect.h = img->GetNRows();
				rect.w = img->GetNCols();
				smp->SetPatch(rect);
				// set image sample
				vecIDs.push_back( glb_sharedMem.AddFrameSample(ssfID, smp) );
				glb_sharedMem.EndFrameSamples(ssfID);

				// create a new tracklet
				tracklet = new SSFTracklet();
				tracklet->SetSamples(vecIDs);
				vecIDs.clear();
				vecIDs.push_back( glb_sharedMem.SetTracklet(tracklet) );

				// create object
				glb_sharedMem.CreateObject(vecIDs, object);
			}
		}

		if( object.empty() ) {
			// link file id to ssf frame id
			fileID = (int)(*inode)["id"];
			if( fileID2images.count( fileID ) )
				ReportError("Error processing file '%s': duplicated image/video id %d", inputName.c_str(), fileID);
			fileID2images[ fileID ] = imgs;
		}
		else{
			// register as already generated
			objectSet.insert((int)(*inode)["id"]);
		}
	}



	// process samples
	node = fs["samples"];
	for( inode = node.begin(); inode != node.end(); ++inode ) {

		// read id, source and frame number
		fileID = (int)(*inode)["id"];
		srcID  = (int)(*inode)["source"];
		frame  = (*inode)["frame"].isNone() ? 0 : (int)(*inode)["frame"];
		// check if source already generated samples
		if( objectSet.find(srcID) != objectSet.end() ) {
			ReportError("Error processing file '%s': do not inform object and samples for a same folder or video (sample %d, source %d).",
				inputName.c_str(),
				fileID,
				srcID
			);
		}
		// check if source exists
		if( fileID2images.count(srcID) == 0 )
			ReportError("Error processing file '%s': missing image id %d referenced by sample %d",
				inputName.c_str(),
				srcID,
				fileID
			);
		imgs = fileID2images[srcID]; // retrieve file images
		// check if frame is valid
		if( frame >= (SMIndexType) imgs.size() )
			ReportError("Error processing file '%s': source id %d referenced by sample id %d doesn't have frame %d (it have %d frames)",
				inputName.c_str(),
				srcID,
				fileID,
				frame,
				imgs.size()
			);
		img = imgs[frame];

		// read patch
		rect.x0 = (*inode)["x0"].isNone() ? 0 : (int)(*inode)["x0"];
		rect.y0 = (*inode)["y0"].isNone() ? 0 : (int)(*inode)["y0"];
		rect.w  = (*inode)["w"].isNone()  ? img->GetNCols() - rect.x0 - 1 : (int)(*inode)["w"];
		rect.h  = (*inode)["h"].isNone()  ? img->GetNRows() - rect.y0 - 1 : (int)(*inode)["h"];

		// check patch size
		if( (rect.x0 + rect.w) >= img->GetNCols() || (rect.y0 + rect.h) >= img->GetNRows() )
			ReportError("Error processing file '%s': a super patch (id=%d) bigger than its image (id=%d) was found!",
				inputName.c_str(),
				fileID,
				srcID
			);

		// create a new sample
		smp = new SSFSample();
		smp->SetPatch(rect);

		// link id to ssf sample
		if( fileID2samples.count( fileID ) )
			ReportError("Error processing file '%s': duplicated sample id %d", inputName.c_str(), fileID);
		fileID2samples[ fileID ] = smp;

		// store sample
		frameID = img->GetFrameID();
		// creating new sample vector
		//if( frameIDsmps.count(frameID) == 0 )
		//	frameIDsmps[frameID] = new vector<SSFSample*>();

		//idSample = glb_sharedMem.SetFrameSample(frameID, smp);
		idSample = glb_sharedMem.AddFrameSample(frameID, smp);
		frameIDsmps[frameID].push_back(idSample);
	}
	// set all frame samples to the shared memory
	for( iframeIDsmps = frameIDsmps.begin(); iframeIDsmps != frameIDsmps.end(); ++iframeIDsmps ) {
		//glb_sharedMem.SetFrameSamples(iframeIDsmps->first, iframeIDsmps->second);
		glb_sharedMem.EndFrameSamples(iframeIDsmps->first);
	}



	// process tracklets
	node = fs["tracklets"];
	for( inode = node.begin(); inode != node.end(); ++inode ) {

		// read id and source
		fileID = (int)(*inode)["id"];
		(*inode)["samples"] >> fileIDList;
		vecIDs.resize(fileIDList.size());
		i = 0;
		for ( ifileIDList = fileIDList.begin(); ifileIDList != fileIDList.end(); ++ifileIDList, ++i ) {
			srcID = *ifileIDList;
			// check if is empty
			if( fileID2samples.count(srcID) == 0 )
				ReportError("Error processing file '%s': missing sample id %d referenced by tracklet %d",
					inputName.c_str(),
					srcID,
					fileID
				);
			vecIDs[i] = fileID2samples[srcID]->GetSampleID();
		}

		// create a new tracklet
		tracklet = new SSFTracklet();
		tracklet->SetSamples(vecIDs);
		ssfID = glb_sharedMem.SetTracklet(tracklet);

		// link id to ssf tracklet
		if( fileID2tracklets.count( fileID ) )
			ReportError("Error processing file '%s': duplicated sample id %d", inputName.c_str(), fileID);
		fileID2tracklets[ fileID ] = ssfID;
	}



	// process objects
	node = fs["objects"];
	for( inode = node.begin(); inode != node.end(); ++inode ) {

		// read id, source
		objID = (string)(*inode)["id"];
		(*inode)["tracklets"] >> fileIDList;
		vecIDs.resize(fileIDList.size());
		i = 0;
		for ( ifileIDList = fileIDList.begin(); ifileIDList != fileIDList.end(); ++ifileIDList, ++i ) {
			srcID = *ifileIDList;
			// check is empty
			if( fileID2tracklets.count(srcID) == 0 )
				ReportError("Error processing file '%s': missing sample id %d referenced by object %s",
					inputName.c_str(),
					srcID,
					objID
				);
			vecIDs[i] = fileID2tracklets[srcID];
		}

		// create a new object
		objList.push_back(
			glb_sharedMem.CreateObject(vecIDs, objID)
		);
	}

// debug reader
#if 0
SSFObject *db_obj;
SMIndexType pos;

vector<SMIndexType> db_tracklets;
vector<SMIndexType>::iterator db_itracklets;
SSFTracklet *db_tracklet;

vector<SMIndexType> db_samples;
vector<SMIndexType>::iterator db_isamples;
SSFSample *db_sample;

SMIndexType itracklet = 0;

	pos = 0;
	while ( (db_obj = glb_sharedMem.RetrieveObject(GetID(), pos++) ) != NULL ) {

		ReportStatus("Object %s is in images:\n", db_obj->GetObjID().c_str());
		db_tracklets = db_obj->GetTrackletIDs();
		for ( db_itracklets = db_tracklets.begin(); db_itracklets != db_tracklets.end(); ++db_itracklets ) {

			db_tracklet = glb_sharedMem.RetrieveTracklet(*db_itracklets);
			db_samples = db_tracklet->GetSamplesID();
			for ( db_isamples = db_samples.begin(); db_isamples != db_samples.end(); ++db_isamples ) {

				db_sample = glb_sharedMem.GetSample(*db_isamples);
				ReportStatus("\t- %s\n",
					glb_sharedMem.GetFrameFilename( db_sample->GetFrameID() ).c_str()
				);
			}
		}
	}
#endif
}
