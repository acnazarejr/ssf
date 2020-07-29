#include "headers.h"
#include "structures.h"
#include "shared_memory.h"
#include "ExtractionControl.h"
#include "parameters.h"
#include "ClassificationMethod.h"
#include "pls_oaa.h"
#include "pls.h"
#include "QDA.h" 
#include "SSFImage.h" 
#include "SSFFeatures.h" 
#include "SSFDict.h"
#include "SSFInvertedList.h"
#include "ModDetectionEvaluation.h"
#include "SSFTest.h"



void SSFTest::SSFTest_PLS() {
//ModParameters modPa;
FeatureParams featParam;
string feed;
SSFStorage storage, storage2;	
SSFMatrix<FeatType> m, m2;
SSFMatrix<FeatType> std, mean, zscore, maux;
SSFMatrix<FeatType> X, X2, Y;
PLS pls, pls2;

	storage.open("X.yml", SSF_STORAGE_READ);
	storage["m"] >> X;
	storage.release();


	storage.open("Y.yml", SSF_STORAGE_READ);
	storage["m"] >> Y;
	storage.release();

	storage.open("test3.yml", SSF_STORAGE_WRITE);
	storage << "X" << X;
	storage << "Y" << Y;
	storage.release();

	pls.runpls(X, Y, 10);
	pls.Save("pls.yml");

	pls2.Load("pls.yml");

	pls2.Save("pls2.yml");

	// load a "clean" version of X
	storage.open("X.yml", SSF_STORAGE_READ);
	storage["m"] >> X;
	storage.release();

	SSFMatrix<FeatType> retProj;
	pls2.Projection(X, retProj, 10);

	storage.open("projectionWstar.yml", SSF_STORAGE_WRITE);
	storage << "retProj" << retProj;
	storage.release();


	pls2.ProjectionBstar(X, retProj);

	storage.open("projectionBstar.yml", SSF_STORAGE_WRITE);
	storage << "retProj" << retProj;
	storage.release();

}



void SSFTest::SSFTest_PLS_OAA() {
SSFStorage storage, storage3, storage4;
PLS_OAA *plsOAA2, *plsOAA;
SSFMatrix<FeatType> responses; 
DataStartClassification dataStart;
SSFMatrix<FeatType> X, X2, Y;

	storage.open("X.yml", SSF_STORAGE_READ);
	storage["m"] >> X;
	storage.release();

	storage.open("X2.yml", SSF_STORAGE_READ);
	storage["m"] >> X2;
	storage.release();

	dataStart.ID = "teste";
	plsOAA = new PLS_OAA(dataStart);

	plsOAA->AddSamples(X, IntToString(1));
	plsOAA->AddSamples(X2, IntToString(2));
	plsOAA->AddSamples(X, IntToString(1));
	plsOAA->Learn();

	storage3.open("plsOAA.yml", SSF_STORAGE_WRITE);
	plsOAA->Save(storage3);
	storage3.release();

	plsOAA2 = new PLS_OAA(dataStart);
	storage4.open("plsOAA.yml", SSF_STORAGE_READ);
	plsOAA2->Load(storage4.root(), storage);
	storage4.release();

	storage4.open("plsOAA2.yml", SSF_STORAGE_WRITE);
	plsOAA2->Save(storage4);
	storage4.release();

	plsOAA2->Classify(X,responses);

	storage4.open("testPLS.yml", SSF_STORAGE_WRITE);
	storage4 << "resposes" << responses;
	storage4.release();
}





void SSFTest::SSFTest_FeatureExtraction() {
SSFFeatures ssfFeats;
FeatureParams featParam;
int featIdx;
//ModParameters modPa;
SSFRect window;
SSFImage *img;
FeatureIndex featIdxMap;
string feed;
//Mat img2;
SSFMatrix<FeatType> m, m3;
vector<int> featIdxs;
	

	// setup feature extraction methods
	featIdxMap = glb_featExtControl.Setup("ParamsFeat.txt");
	
	// open image
	img = new SSFImage("image_00000000_0.png");
	//img2 = img->RetrieveCopy();

	// set image patch from which the feature will be extracted
	window.SetRect(0, 0, img->GetNCols(), img->GetNRows());

	SSFStorage storage;
	storage.open("a.yml", SSF_STORAGE_WRITE);

	
	// extract using feat2
	featIdx = glb_featExtControl.RetrieveSetupIdx(featIdxMap["feat2"]);
	featIdxs.push_back(featIdx);
	glb_featExtControl.SetImage(featIdx, img);
	glb_featExtControl.ExtractFeatures(featIdx, window, NULL, ssfFeats); 

	m3 = ssfFeats.RetrieveFeatures(true);

	storage << "m3" << m3;

	ssfFeats.Reset();


	// extract using feat1
	featIdx = glb_featExtControl.RetrieveSetupIdx(featIdxMap["feat1"]);
	featIdxs.push_back(featIdx);
	glb_featExtControl.SetImage(featIdx, img);
	glb_featExtControl.ExtractFeatures(featIdx, window, NULL, ssfFeats); 

	m = ssfFeats.RetrieveFeatures(true);

	storage << "m2" << m;
	

	glb_featExtControl.ExtractFeatures(featIdxs, window, ssfFeats);
	m = ssfFeats.RetrieveFeatures(true);

	storage << "m4" << m;

	storage.release();
}

void SSFTest::SSFTest_FeatureExtractionHOG(){
SSFFeatures ssfFeats;
FeatureParams featParam;
int featIdx;
//ModParameters modPa;
SSFRect window;
SSFImage *img;
FeatureIndex featIdxMap;
string feed;
SSFMatrix<FeatType> m, m3;
vector<int> featIdxs;
	

	// setup feature extraction methods
	featIdxMap = glb_featExtControl.Setup("ParamsFeatHOG.txt");
	
	// open image
	img = new SSFImage("HOG_teste.jpg");

	// set image patch from which the feature will be extracted
	window.SetRect(0, 0, img->GetNCols(), img->GetNRows());

	SSFStorage storage;
	storage.open("a.yml", SSF_STORAGE_WRITE);

	// extract using feat2
	featIdx = glb_featExtControl.RetrieveSetupIdx(featIdxMap["feat1"]);
	featIdxs.push_back(featIdx);
	glb_featExtControl.SetImage(featIdx, img);
	glb_featExtControl.ExtractFeatures(featIdx, window, NULL, ssfFeats); 

	m3 = ssfFeats.RetrieveFeatures(true);

	storage << "m3" << m3;

	ssfFeats.Reset();


	storage.release();
}



void SSFTest::SSFTest_QDA() {
SSFStorage storage, storage3, storage4;
QDA *qda, *qda2;
SSFMatrix<FeatType> responses; 
DataStartClassification dataStart;
SSFMatrix<FeatType> X, X2, Y;

	storage.open("X.yml", SSF_STORAGE_READ);
	storage["m"] >> X;
	storage.release();

	storage.open("X2.yml", SSF_STORAGE_READ);
	storage["m"] >> X2;
	storage.release();

	dataStart.ID = "teste";
	qda = new QDA(dataStart);

	qda->AddSamples(X, "positive");
	qda->AddSamples(X2, "negative");
	qda->AddSamples(X, "positive");
	qda->Learn();

	storage3.open("QDA.yml", SSF_STORAGE_WRITE);
	qda->Save(storage3);
	storage3.release();

	qda2 = new QDA(dataStart);
	storage4.open("QDA.yml", SSF_STORAGE_READ);
	qda2->Load(storage4.root(), storage);
	storage4.release();

	storage4.open("QDA2.yml", SSF_STORAGE_WRITE);
	qda2->Save(storage4);
	storage4.release();

	qda2->Classify(X, responses);

	storage4.open("testQDA.yml", SSF_STORAGE_WRITE);
	storage4 << "resposes" << responses;
	storage4.release();

	qda2->Classify(X2, responses);

	storage4.open("testQDA2.yml", SSF_STORAGE_WRITE);
	storage4 << "resposes" << responses;
	storage4.release();

	vector<string> ids;
	ids = qda2->RetrieveClassIDs();

	// test in matlab
	// data = read_yml('../X.yml');
	// X = reshape(data.m.data, [50 110])';
	// data = read_yml('../X2.yml');
	// X2 = reshape(data.m.data, [50 110])';
	// [class,err,POSTERIOR,logp,coeff] = classify(X, [X; X2; X], [ones(110,1); 1+ones(110,1); ones(110,1)]);
	// data = read_yml('../testQDA.yml');
	// data.resposes.data
}


void SSFTest::SSFImage_Test() {
SSFStorage storage, storage2;
SSFImage *img, *img2;
SSFRect window;

	img = new SSFImage("image_00000000_0.png");

	// encoding
	storage.open("png.yml", SSF_STORAGE_WRITE);
	img->Save(storage);
	storage.release();
	delete img;

	// decoding
	storage.open("png.yml", SSF_STORAGE_READ);
	img = new SSFImage(storage.root());
	storage.release();
	img->Save("img2.png");

	// crop
	window.x0 = 0;
	window.y0 = 0;
	window.w = 200;
	window.h = 200;
	img2 = img->RetrievePatch(window);
	img2->Save("crop.png");
}



void SSFTest::SSFDet_Evaluation() {
ModDetectionEvaluation deteval("nothing");
vector<FeatType> posResp;
vector<FeatType> negResp;

	posResp.push_back(0.1f);
	posResp.push_back(0.3f);
	posResp.push_back(0.4f);
	posResp.push_back(0.5f);
	posResp.push_back(0.6f);
	posResp.push_back(0.7f);
	posResp.push_back(0.7f);
	posResp.push_back(0.8f);
	posResp.push_back(0.81f);


	negResp.push_back(0.1f);
	negResp.push_back(0.2f);
	negResp.push_back(0.3f);
	negResp.push_back(0.2f);
	negResp.push_back(0.4f);
	negResp.push_back(0.3f);
	negResp.push_back(0.1f);
	negResp.push_back(0.38f);
	negResp.push_back(0.181f);


	deteval.EvaluateFPPW(posResp, negResp);

}


void SSFTest::SSFTest_SVM()  {
SSFStorage storage, storage2;
FileNode n, node;
int num_files = 5;
int img_area = 4*3;
Mat training_mat(num_files,img_area,CV_32FC1);
string imgname = "image_00000000_0.png";
Mat img_mat = imread(imgname,0); // I used 0 for greyscale


int ii = 0; // Current column in training_mat
for (int i = 0; i<4; i++) {
    for (int j = 0; j < 3; j++) {
        training_mat.at<float>(0,ii++) = img_mat.at<uchar>(i,j);
    }
}
Mat labels(num_files,1,CV_32FC1);
labels.at<float>(0,0) = 0;
labels.at<float>(1,0) = 0;
labels.at<float>(2,0) = 0;
labels.at<float>(3,0) = 1;
labels.at<float>(4,0) = 1;

CvSVMParams params;
params.svm_type = CvSVM::C_SVC;
params.kernel_type = CvSVM::LINEAR;
params.gamma = 3;

CvSVM svm, svm2;
svm.train(training_mat, labels, Mat(), Mat(), params);



	storage.open("svm_filename.yml", SSF_STORAGE_WRITE);
	storage << "teste" << params.gamma;
	svm.write(*storage, "SVM");
	storage.release();

	storage2.open("svm_filename.yml", SSF_STORAGE_READ);
	node = storage2.root();
	n = node["SVM"];
	svm2.read(*storage2, *n);
	storage2.release();
	svm2.save("svm_filename2.yml");
}