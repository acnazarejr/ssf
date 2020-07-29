#include "headers.h"
#include "SSFCore.h"
#include "ExtractionControl.h"
#include "LAP.h"
#include "pls.h"
#include "QDA.h"
#include "files.h"
#include "ModDetRandomFiltering.h"


ModDetRandomFiltering instance("");

/******
 * class ModDetRandomFiltering
 ******/
ModDetRandomFiltering::ModDetRandomFiltering(string modID) :  SSFUserModule(modID) {

	pls = NULL;
	nfactors = -1;
	threshold = -10000.0f;
    strideX_ = -1;
    strideY_ = -1;
    detWindowHeight_ = -1;
    detWindowWidth_  = -1;

	executionType = "testing";

    /* Sample generator parameters */
	inputParams.Add("detWindowHeight", &detWindowHeight_, "Detection window's height", false);
	inputParams.Add("detWindowWidth", &detWindowWidth_, "Detection window's width", false);
    inputParams.Add("strideX", &strideX_, "Displacement value in x-axis for sample generation", false);
    inputParams.Add("strideY", &strideY_, "Displacement value in y-axis for sample generation", false);

	/* Add parameters */
	inputParams.Add("modelFile", &modelFile, "File storing the model to save or to load", true);
	inputParams.Add("executionType", &executionType, "Execute type (learning or testing)", true);
	inputParams.Add("featFile", &featFile, "File containing the feature setup for detection (only for learning)", false);
	inputParams.Add("featSetup", &featSetup, "Feature setup that will be considered (must be defined in file featFile", false);
	inputParams.Add("nfactors", &nfactors, "Number of PLS factors (only for learning)", false);
	inputParams.Add("threshold", &threshold, "Threshold for this detector (only for testing)", false);

	/* module information */
	info.Description("Implementation of the random filtering detector.");

	/* set input and output data */
	modParams.Require(modID, SSF_DATA_IMAGE,      &posInputMod,    "positive");	// images for positive samples
	modParams.Require(modID, SSF_DATA_SLDWINDOWS, &groundTruthMod, "groundTruth");	// sliding windows for positive samples
	modParams.Require(modID, SSF_DATA_SLDWINDOWS, &testInputMod,   "test");		// sliding windows for test images
	//modParams.Require(modID, SSF_DATA_SLDWINDOWS, &negInputMod, "negative");	// sliding windows for negative samples

	modParams.Provide(modID, SSF_DATA_SLDWINDOWS);

	/* register the module */
	this->Register(this, modID);
}

ModDetRandomFiltering::~ModDetRandomFiltering() {

	if (pls != NULL)
		delete pls;
}


void ModDetRandomFiltering::Setup() {

	// check if the input modules have been set correctly
	if (executionType != "learning" && executionType != "testing")
		ReportError("executionType must be either 'learning' or 'testing'");

	else if (executionType == "learning" && (posInputMod == "" || groundTruthMod == ""))
		ReportError("posInputMod and groundTruthMod must be set for learning");

	else if (executionType == "testing" && testInputMod == "")
		ReportError("testInputMod must be set for testing");

	// check if the parameters are set correctly
	// learning
	if (executionType == "learning") {
		if (featFile == "")
			ReportError("Parameter 'featFile' has to be set");
		if (featSetup == "")
			ReportError("Parameter 'featSetup' has to be set");
		if (nfactors <= 0)
			ReportError("Parameter 'nfactors' has to be set");
		if (detWindowHeight_ <= 0 || detWindowWidth_ <= 0)
            ReportError("Parameters 'detWindowHeight' and 'detWindowWidth' have to be set");
        if (strideX_ <= -1 || strideY_ <= -1)
            ReportError("Parameters 'strideX' and 'strideY' have to be set");
	}

	// testing
	else {
		if (featFile == "")
			ReportError("Parameter 'featFile' has to be set");
		if (featSetup == "")
			ReportError("Parameter 'featSetup' has to be set");
	}
}


SSFMethod *ModDetRandomFiltering::Instantiate(string modID) {

	return new ModDetRandomFiltering(modID); 
}


void ModDetRandomFiltering::Execute() {
size_t frameID;
size_t position = 0;
string filename;
SSFImage *img;
SSFSlidingWindow *sldWins;
vector<DetWin> *detwins;
// feature extraction
SSFFeatures ssfFeats;
SSFMatrix<FeatType> dataY;

    // learning
    if (executionType == "learning") {
		// for each subwindow 
		position = 0;
		while ((frameID = glb_sharedMem.RetrieveFrameIDbyPosition(posInputMod, SSF_DATA_IMAGE, position++)) != SM_INVALID_ELEMENT) {

			ReportStatus("[frameID: %d] Learning for images at position %d", frameID, position);

			// retrieve image properties
			img = glb_sharedMem.GetFrameImage(frameID);
			filename = glb_sharedMem.GetFrameFilename(frameID);

			// retrieve sliding windows for frameID
			sldWins = glb_sharedMem.GetSlidingWindows(groundTruthMod, frameID);

			// retrieve all detwins
			detwins = sldWins->GetAllDetWins();

			// generate training samples
			cv::Mat gtStrides; 
			vector<SSFImage*> ssfImages;
			GenerateTrainingSamples(img, detwins, ssfImages, gtStrides);

			// extract features
			SSFFeatures currSsfFeats;
			SSFMatrix<FeatType> currDataY;
			FeatureExtraction(ssfImages, gtStrides, currSsfFeats, currDataY);

			// push features into this matrix
			if (currSsfFeats.RetrieveFeatures(false).rows != 0) {
				ssfFeats.AddFeatures( currSsfFeats.RetrieveFeatures(false) );
				dataY.push_back( currDataY );
			}

			// delete allocated memory
			for (int i = 0; i < ssfImages.size(); ++i)
				delete ssfImages[i];
            delete detwins;
		}

		Learning( ssfFeats, dataY );
	}

    // testing
	else if (executionType == "testing") { 

		size_t scaleID;
        SSFSlidingWindow *outputSldWins;
		vector<int> featIdxs;
		SSFDetwins *ssfDetwins, *outputDetwins;
		SSFFeatures ssfFeatSample;
        SSFRect rect;
		SSFMatrix<FeatType> lowD, responses, allResponses;
		int responsePosID = 0;		// index of the positive response
		FeatureIndex featIdx;
		vector<size_t> allIds;

		// setup feature extraction
		featIdx = featExt.Setup(featFile);

		// obtain the feature index that will be used to perform the feature extraction
		//featSetupIdx = featExt.RetrieveSetupIdx(featIdx[featSetup]);
		allIds = featIdx.GetAllIDs();
		for (int i = 0; i < allIds.size(); i++)
			featIdxs.push_back(featExt.RetrieveSetupIdx(allIds[i]));

		// load saved detection model
		this->Load();

		position = 0;
		while ((frameID = glb_sharedMem.RetrieveFrameIDbyPosition(testInputMod, SSF_DATA_SLDWINDOWS, position++)) != SM_INVALID_ELEMENT) {

			// set sliding windows
			outputSldWins = new SSFSlidingWindow();

			// retrieve sliding windows for frameID
			sldWins = glb_sharedMem.GetSlidingWindows(testInputMod, frameID);

			// load images for the input sliding windows
			sldWins->LoadImages(glb_sharedMem.GetFrameImage(frameID));

            // retrieve image
			img = glb_sharedMem.GetFrameImage(frameID);

			ReportStatus("[frameID: %d] Detecting for %d scales and %d detection windows)", frameID, sldWins->GetNEntries(), sldWins->GetNumberofWindows());

			// run for each scale
			for (scaleID = 0; scaleID < sldWins->GetNEntries(); scaleID++) {

				// set image for this scale
				featExt.SetImage(featIdxs, sldWins->GetSSFImage(scaleID));

                // image dimensions of this scale
				int imgCols = sldWins->GetSSFImage(scaleID)->GetNCols();
				int imgRows = sldWins->GetSSFImage(scaleID)->GetNRows();

				// retrieve sliding windows of this scale
				ssfDetwins = sldWins->GetSSFDetwins(scaleID);

				// extract features for each detection window
				detwins = ssfDetwins->GetDetwins();

				// create output SSFDetwins
				outputDetwins = new SSFDetwins();

				for (int i = 0; i < detwins->size(); i++) {
					ssfFeatSample.Reset();
					rect = detwins->at(i).GetSSFRect();

					featExt.ExtractFeatures(featIdxs, rect, ssfFeatSample);  

					SSFMatrix<FeatType> regression, retProj;
					regression.create(0,2);
                    retProj.create(0,2);

					//ssfFeats.AddFeatures(ssfFeatSample.RetrieveFeatures(true));

					// extract features for the positive samples	
					pls->ProjectionBstar(ssfFeatSample.RetrieveFeatures(true), retProj);
					//regression.push_back(retProj);

					DetWin detwin = detwins->at(i);

                    // check if it doesn't exceeds the image boundaries
					if ((detwin.x0 + retProj(0,0) >= FLT_EPSILON) && (imgCols - 1 - (detwin.x0 + retProj(0,0) + detwin.w)) >= FLT_EPSILON)
                        detwin.x0 += retProj(0,0);
                    if ((detwin.y0 + retProj(0,1) >= FLT_EPSILON) && (imgRows - 1 - (detwin.y0 + retProj(0,1) + detwin.h)) >= FLT_EPSILON)
                        detwin.y0 += retProj(0,1);

                    // add detwins
					outputDetwins->AddDetWindow(detwin);
				}

				// only create this guy if outputDetwins->Size() > 0, otherwise do not propagate to the "next stage"
				if (outputDetwins->GetNumberofDetWins() > 0) 
					//ReportStatus("WARNING: clone scaleInfo");
						outputSldWins->AddDetwins(outputDetwins, sldWins->GetScaleInfo(scaleID));
			}

			// release the images for the input sliding windws
			sldWins->ReleaseImages();

			// set output sliding windows
			glb_sharedMem.SetSlidingWindows(frameID, outputSldWins);
		}

//		Mat regression = Testing( filename, testSamples, testDataY );
//		// saves ground truth
//		InputType inputType = FileParts( filename );
//		string resultsName = "results/" + inputType.name + ".yml";

//		FileStorage fsRegression(resultsName, FileStorage::WRITE);
//		fsRegression << "regression" << regression;
//		fsRegression << "groundTruth" << testDataY;

//		float mseX = RootMeanSquaredError( regression.colRange( Range(0,1) ), testDataY.colRange( Range(0,1) ) );
//		float mseY = RootMeanSquaredError( regression.colRange( Range(1,2) ), testDataY.colRange( Range(1,2) ) );

//		fsRegression << "mseX" << mseX;
//		fsRegression << "mseY" << mseY;

//		fsRegression.release();
	}
	else {
		ReportError("Invalid executionType '%s'", executionType.c_str());
	}
}

////////////////////////////////////////////////////////////////////////////////
//                GENERATE TRAINING SAMPLES AND AUXILIAR METHODS                   
////////////////////////////////////////////////////////////////////////////////

void ModDetRandomFiltering::GenerateTrainingSamples(SSFImage *img, vector<DetWin> *detwins, vector<SSFImage*> &ssfSamples, Mat &deltas) {
int i, j;
vector<Rect> samplesRect; // used to save in a archive

    Mat data = img->RetrieveCopy();

	// maximum values a window can reach for each ground truth sample
	int xMax = 0, yMax = 0; 
	DetWin detwin; 
	for (i = 0; i < detwins->size(); ++i) {
		detwin = detwins->at(i);
		xMax = 65; //max( detwin.w, xMax );
		yMax = 129; //max( detwin.h, yMax );
	}

	// subwindows criteria: only the ones without close persons
    vector<bool> areSamplesClose( detwins->size(), false ); // filled with boolean values. close: 1, far: 0
	for (i = 0; i < detwins->size(); ++i) {
        for (j = 1; j < detwins->size(); ++j) {
            if (i == j) 
                continue;
			bool areThey = AreSamplesClose( detwins->at(i), detwins->at(j), (float) xMax );
            areSamplesClose[i] = areThey || areSamplesClose[i];
            areSamplesClose[j] = areThey || areSamplesClose[j];
		}
	}

    // compute ground truth once
	DetWin canonDetwin(0, 0, detWindowWidth_, detWindowHeight_, 0.0);
	Mat deltasSample = ComputeGroundTruth(canonDetwin, 0.5, strideX_, strideY_); // one canonical set of strides because all windows have the same size

	// for each ground truth detwin
	for (i = 0; i < detwins->size(); ++i) {

        if (areSamplesClose[i] == true)
            continue;

		DetWin detwin = detwins->at(i); 
		deltas.push_back( deltasSample ); // push canonical samples 

        // resize image once
        double fx = (double) detWindowWidth_  / detwin.w;
        double fy = (double) detWindowHeight_ / detwin.h;

		// rescale and add padding to image
		Mat rsPaddedImg;
        cv::resize(data, rsPaddedImg, Size(), fx, fy);
		cv::copyMakeBorder( rsPaddedImg, rsPaddedImg, yMax, yMax, xMax, xMax, BORDER_REPLICATE); // src, dst, top, bottom,  left, right, borderType, value

		for (j = 0; j < deltasSample.rows; ++j) { 
			DetWin displacedWin(detwin);

            // resize detwin to resized image space
            displacedWin.x0 = (int) floor( displacedWin.x0 * fx );
            displacedWin.y0 = (int) floor( displacedWin.y0 * fy );

            // displaces window by a stride x and y, respectively
			displacedWin.x0 += deltasSample.at<int>(j,0) + xMax;
			displacedWin.y0 += deltasSample.at<int>(j,1) + yMax;

			displacedWin.w = detWindowWidth_;
            displacedWin.h = detWindowHeight_;

			assert(displacedWin.x0 >= 0 && displacedWin.y0 >= 0);
			assert(displacedWin.x0 + detWindowWidth_  < rsPaddedImg.cols);
			assert(displacedWin.y0 + detWindowHeight_ < rsPaddedImg.rows);

			Rect window(displacedWin.x0, displacedWin.y0, detWindowWidth_, detWindowHeight_);
            Mat sample = rsPaddedImg(window);

            samplesRect.push_back( window ); // save samples

            ssfSamples.push_back( new SSFImage(sample) );
		}
	} // end for (i = 0; i < detwins->size() ... for each ground truth detwin

	//  multiplied by -1. 
	// this function displaces window by +strideX, +strideY. to return to the original position,
	// one must sum -strideX,-strideY
    deltas *= -1;
} // end GenerateTrainingSamples(.) ... 

Mat ModDetRandomFiltering::ComputeGroundTruth(DetWin gtWin, float threshold, int strideX, int strideY) {
int deltaY = 0;
int deltaX = 0;
vector<Point> deltas;
vector<DetWin> outDetwins;

    for (deltaY = 0; ; deltaY += strideY) {
        // generate the new sample based on this deltaY
        deltaX = 0;
		DetWin baseWin( gtWin.x0 + deltaX, gtWin.y0 + deltaY, gtWin.w, gtWin.h, 0.0 );

        // jaccard coefficient for this base y-window
        float yMetric = JaccardCoefficient(gtWin, baseWin);
		if (yMetric - threshold < FLT_EPSILON) 
            break;

        // if using this y is ok, try varying x 
		for (deltaX = 0; ; deltaX += strideX) {
            DetWin displacedWin( gtWin.x0 + deltaX, gtWin.y0 + deltaY, gtWin.w, gtWin.h, 0.0 );
            float a0 = JaccardCoefficient(gtWin, displacedWin);    
            if (a0 - threshold < FLT_EPSILON)
                break;
			else { 
                deltas.push_back(Point(deltaX, deltaY));
                outDetwins.push_back( displacedWin );
			}
		}
	}
    // save into a matrix file
    cv::Mat deltasMat(deltas);
	deltasMat = deltasMat.reshape(1, 0);

    // create permutations of these matrix
    cv::Mat drPairs = deltasMat.clone(); // down right
    cv::Mat dlPairs = deltasMat.clone(); // down left
    cv::Mat ulPairs = deltasMat.clone(); // upper left

	drPairs.col(1) *= -1;
	dlPairs.col(0) *= -1; dlPairs.col(1) *= -1;
	ulPairs.col(0) *= -1;
    
    // insert only unique rows
    Mat allPairs;
    allPairs.push_back( deltasMat );
    allPairs.push_back( drPairs );
    allPairs.push_back( dlPairs );
    allPairs.push_back( ulPairs );

    // keep only unique rows
    Mat uniqueStrides; 
    uniqueStrides.push_back( allPairs.row(0) );
    for (int i = 1; i < allPairs.rows; ++i) {
        int isInside = false;
		for (int j = 0; j < uniqueStrides.rows; ++j) {
			int count = 0;
			for (int k = 0; k < uniqueStrides.cols; ++k) // checks by element of 
				if(allPairs.at<int>(i,k) == uniqueStrides.at<int>(j,k)) 
					++count;
			if (count == 2) {
				isInside = true;
				break;
			}	
		}
        if (isInside == false) uniqueStrides.push_back( allPairs.row(i) );
	}

	return uniqueStrides;  
}


// Auxiliar method of GenerateTrainingSamples(.)
float ModDetRandomFiltering::JaccardCoefficient(DetWin &gtWin, DetWin &detwin) {

	Rect gtRect( gtWin.GetSSFRect().GetOpencvRect() );
	Rect winRect( detwin.GetSSFRect().GetOpencvRect() );
    
    cv::Rect intersect = gtRect & winRect;
    if (intersect.area() == 0) 
        return 0;

	float areaUnion = (float) (detwin.w * detwin.h) + (gtWin.w * gtWin.h) - intersect.area();
    return (intersect.area() / areaUnion); 
}

// Auxiliar method of GenerateTrainingSamples(.)
bool ModDetRandomFiltering::AreSamplesClose(DetWin &a, DetWin &b, float maxDistance) {
float centroidAx, centroidBx;
float centroidAy, centroidBy;
float distance;

    centroidAx = (float) (a.x0 + a.w) / 2; centroidAy = (float) (a.y0 + a.h) / 2;
    centroidBx = (float) (b.x0 + b.w) / 2; centroidBy = (float) (b.y0 + b.h) / 2;

    float x = centroidAx - centroidBx; 
    float y = centroidAy - centroidBy;

    distance = sqrt( x*x + y*y );

    if (distance - maxDistance <= FLT_EPSILON)
        return true;
	else
		return false;
}

float ModDetRandomFiltering::RootMeanSquaredError(Mat Yhat, Mat Y) {
    
    assert(Yhat.rows == Y.rows);

	Yhat.convertTo(Yhat, CV_32F);
    Y.convertTo(Y, CV_32F);

    //Mat err = Yhat - Y;
    //Mat errSquared = err.mul( err );
    //float mse = cv::sum( errSquared )[0] / Y.rows;
    //mse = sqrtf( mse );

    // sanity check
    float mse = 0;
    for (int i = 0; i < Y.rows; ++i) {
        float diff = Yhat.at<float>(i) - Y.at<float>(i);
        mse += diff * diff;
	}
	mse = (float) mse / Y.rows;
    mse = sqrtf( mse );

    return mse;
}

////////////////////////////////////////////////////////////////////////////////
//                           REGRESSION LEARNING                              // 
////////////////////////////////////////////////////////////////////////////////
void ModDetRandomFiltering::Learning(SSFFeatures &ssfFeats, SSFMatrix<FeatType> &dataY) {
SSFMatrix<FeatType> retProj;

	// execute PLS
	pls = new PLS();
	pls->runpls(ssfFeats.RetrieveFeatures(false), dataY, nfactors); 

	pls->ProjectionBstar(ssfFeats.RetrieveFeatures(false), retProj);

    // TODO check training error using MSE
	// save model
	this->Save();
}


void ModDetRandomFiltering::FeatureExtraction(vector<SSFImage*> ssfSamples, Mat gtStrides, SSFFeatures &ssfFeats, SSFMatrix<FeatType> &dataY) {
FeatureIndex featIdx;
SSFRect rect;
SSFFeatures ssfFeatSample;
SSFMatrix<FeatType> lowPos, lowNeg;
vector<int> featIdxs;
vector<size_t> allIds;
DataStartClassification startClassification;
size_t position, i;

    // convert delta matrix to FLOAT
    gtStrides.convertTo(gtStrides, CV_32FC1);

	// initialize Y matrix (this matrix will have Nx2, when N is the number of feature samples (positive ones)
	dataY.create(0, 2);

	// setup feature extraction
	featIdx = featExt.Setup(featFile);

	// obtain the feature index that will be used to perform the feature extraction
	//featSetupIdx = featExt.RetrieveSetupIdx(featIdx[featSetup]);
	allIds = featIdx.GetAllIDs();
	for (i = 0; i < allIds.size(); i++)
		featIdxs.push_back(featExt.RetrieveSetupIdx(allIds[i]));

	// extract features for the positive samples	
	position = 0;
	for (int sampleIter = 0; sampleIter < ssfSamples.size(); ++sampleIter) {
        SSFImage *sampleImg = ssfSamples[sampleIter];

		// set image for this scale
		featExt.SetImage(featIdxs, sampleImg);

		ssfFeatSample.Reset();
		SSFRect rect(0, 0, sampleImg->GetNCols(), sampleImg->GetNRows());
		featExt.ExtractFeatures(featIdxs, rect, ssfFeatSample);  

        // add features
		ssfFeats.AddFeatures(ssfFeatSample.RetrieveFeatures(true));
		dataY.push_back( gtStrides.row(sampleIter) );

		//ReportStatus("Extracting positive features [sample: %d]", sampleIter + 1);
	}
}

////////////////////////////////////////////////////////////////////////////////
//                         REGRESSION TESTING
////////////////////////////////////////////////////////////////////////////////
Mat ModDetRandomFiltering::Testing(string filename, SSFFeatures &ssfFeats, SSFMatrix<FeatType> &deltas) {
SSFSlidingWindow *outputSldWins;
SSFDetwins *outputDetwins;
FeatureIndex featIdx;
SSFFeatures ssfFeatSample;
DetWin detwin;
SSFMatrix<FeatType> lowD, responses, allResponses;
size_t i;
vector<size_t> allIds;
vector<int> featIdxs;

	// setup feature extraction
	featIdx = featExt.Setup(featFile);

	// obtain the feature index that will be used to perform the feature extraction
	//featSetupIdx = featExt.RetrieveSetupIdx(featIdx[featSetup]);
	allIds = featIdx.GetAllIDs();
	for (i = 0; i < allIds.size(); i++)
		featIdxs.push_back(featExt.RetrieveSetupIdx(allIds[i]));

	// load saved detection model
	this->Load();

	SSFMatrix<FeatType> regression, retProj;
	regression.create(0,2);

	// extract features for the positive samples	
	pls->ProjectionBstar(ssfFeats.RetrieveFeatures(false), retProj);
	regression.push_back(retProj);

#if 0 
	int nsamples = ssfFeats.RetrieveFeatures(false).rows;
	for (int sampleIter = 0; sampleIter < nsamples; ++sampleIter) {
        ReportStatus("[sample: %d] Detecting for images", sampleIter);

		// set sliding windows
		outputSldWins = new SSFSlidingWindow();

		// create output SSFDetwins
		outputDetwins = new SSFDetwins();

		ssfFeatSample.Reset();

		// project to the low dimension
		regression.push_back( pls->ProjectionBstar(ssfFeatSample.RetrieveFeatures(true)) );
		auto answer = deltas.row(sampleIter);
		// set image to the new sliding window
		//outputSldWins->AddDetwins(outputDetwins, sldWins->GetScaleInfo(scaleID), sldWins->GetSSFImage(scaleID)->Clone());
	}
#endif

	//ReportStatus("[frameID: %d] Detecting for images at position %d", frameID, position);

    // TODO set sliding window into the shared memory
	//glb_sharedMem.SetSlidingWindows(frameID, outputSldWins);

    return regression;
}


void ModDetRandomFiltering::Save() {
SSFStorage storage;

	storage.open(modelFile, SSF_STORAGE_WRITE);
	if (storage.isOpened() == false)
		ReportError("Invalid file storage (%s)!", modelFile.c_str());

	storage << "DetRandomFiltering" << "{";

	// save pls model
	pls->Save(storage);

	// save feature setup

	storage << "}";
	storage.release();
}



void ModDetRandomFiltering::Load() {
DataStartClassification startClassification;
SSFStorage storage;
FileNode n, node;

	storage.open(modelFile, SSF_STORAGE_READ);
	if (storage.isOpened() == false)
		ReportError("Invalid file storage (%s)!", modelFile.c_str());

	node = storage.root();

	n = node["DetRandomFiltering"];

	// load pls model
	pls = new PLS();
	pls->Load(n);

	// load feature setup

	storage.release();
}