
#include "headers.h"
#include "LAP.h" //Locking at people
#include "Tracking.h"

//#include <sys/time.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>

using namespace cv;
using namespace std;

//cv::KalmanFilter KF;
//cv::Mat_<float> measurement(2,1); 
//Mat_<float> state(4, 1); // (x, y, Vx, Vy)


Tracking instance("");

/******
* class Tracking
******/
Tracking::Tracking(string modID) :  Module(modID) {

	/* Add parameters in file paramstest*/
	//modParams.Add("feedName1", &feedNameTrain, "desc", true); 

	/* set input and output data */
	modParams.Require(modID,  SSF_DATA_IMAGE, &inputModID, "image");	// input image
	modParams.Require(modID,  SSF_DATA_SAMPLES, &inputModID, "samples");	// input samples
	modParams.Provide(modID, SSF_DATA_TRACKLET); // output mask
	modParams.Provide(modID, SSF_DATA_SAMPLES); // output samples


	/* register the module */
	this->RegisterModule(GetModName(), GetModType(), this, modID);
}

void Tracking::Setup() {

	if (inputModID == "")
		ReportError("Input module with images has not been set (variable: inModImg)!");
}

Module *Tracking::Instantiate(string modID) {

	return new Tracking(modID); 
}

#if 0
void initKalman(float x, float y)
{
	// Instantate Kalman Filter with
	// 4 dynamic parameters and 2 measurement parameters,
	// where my measurement is: 2D location of object,
	// and dynamic is: 2D location and 2D velocity.
	KF.init(4, 2, 0);

	measurement = Mat_<float>::zeros(2,1);
	measurement.at<float>(0, 0) = x;
	measurement.at<float>(1, 0) = y;

	KF.statePre.setTo(0);

	KF.statePre.at<float>(0, 0) = x;
	KF.statePre.at<float>(1, 0) = y;

	KF.statePost.setTo(0);
	KF.statePost.at<float>(0, 0) = x;
	KF.statePost.at<float>(1, 0) = y; 

	KF.transitionMatrix = (Mat_<float>(4, 4) << 1,0,1,0,   0,1,0,1,  0,0,1,0,  0,0,0,1); // Including velocity
	KF.measurementMatrix = (Mat_<float>(2, 4) << 1,0,0,0, 0,1,0,0);
	setIdentity(KF.processNoiseCov, Scalar::all(.0000005)); //adjust this for faster convergence - but higher noise
	setIdentity(KF.measurementNoiseCov, Scalar::all(0.02e-5));
	setIdentity(KF.errorCovPost, Scalar::all(.1));
}

Point kalmanPredict() 
{
	Mat prediction = KF.predict();
	Point predictPt(prediction.at<float>(0),prediction.at<float>(1));

	/*This prevents the filter from getting stuck in a state when no corrections are executed.*/
	KF.statePre.copyTo(KF.statePost);
	KF.errorCovPre.copyTo(KF.errorCovPost);

	return predictPt;
}
Point2f kalmanCorrect(float x, float y)
{
	measurement(0) = x;
	measurement(1) = y;
	Mat estimated = KF.correct(measurement);
	Point2f statePt(estimated.at<float>(0),estimated.at<float>(1));
	return statePt;
}
#endif

#if 1
//void Tracking::Execute() {

//    KalmanFilter KF(4, 2, 0);
//    Mat measurement = Mat::zeros(2, 1, CV_32F);

//    for(;;) {
//        setIdentity(KF.measurementMatrix);
//        setIdentity(KF.processNoiseCov, Scalar::all(.00005));
//        setIdentity(KF.measurementNoiseCov, Scalar::all(0.02)); // this is inverse to the tracking speed
//        setIdentity(KF.errorCovPost, Scalar::all(1));

//        for(;;)
//        {
//            Mat prediction = KF.predict();

//			float px = prediction.at<float>(0);
//			float py = prediction.at<float>(1);
//			circle( img, Point2f( px, py),  2, Scalar(20,200,20), 1); 

//            measurement.at<float>(0) = float(mouse.x);
//            measurement.at<float>(1) = float(mouse.y);
//            KF.correct(measurement);

//            circle( img, Point2f( mouse.x, mouse.y),1, Scalar(180,180,180), 1); 
//            imshow( "Kalman", img );
//        }
//    }
//}

void Tracking::Execute() {
	size_t frameID;
	SSFSamples *ssfSamples;
	vector<SMIndexType> samples;
	SSFSample *sample;
	float x = 0,y = 0;
	SSFImage *img;
	Mat dataOriginal;
	namedWindow("janela");

	//Pode pegar o ultimo frame processado?
	//Faria sentido caso a cada frame computado fosse chamado o kalman.
	//while ((frameID = glb_sharedMem.RetrieveLastProcessedFrameID(inputModID, SSF_DATA_SAMPLES, true)) != SM_INVALID_ELEMENT){

	frameID = glb_sharedMem.RetrieveLastProcessedFrameID(inputModID, SSF_DATA_SAMPLES, true);
	img = glb_sharedMem.GetFrameImage(frameID);
	dataOriginal = img->RetrieveCopy();

	//Pegue todas as samples do frameid x;
	ssfSamples = glb_sharedMem.GetFrameSamples(inputModID, frameID);

	//Pegue todos os indices das samples do frameid x; 
	samples = ssfSamples->GetSamples();
	Point2f s, p;

	for (int i = 0; i < samples.size(); i++) {
		sample = glb_sharedMem.GetSample(samples.at(i));

		// processing the current frame
		ReportStatus("[Feed: %d] Kalman for frameID '%d'", inputModID,  frameID);

		// Pegar a janela correspondente a sample
		SSFRect rectSample = sample->GetWindow();

		//Resolução inocente
		Point2f aux(rectSample.x0,rectSample.y0);

		KalmanFilter KF(4, 2, 0);
		Mat measurement = Mat::zeros(2, 1, CV_32F);

		setIdentity(KF.measurementMatrix);
		setIdentity(KF.processNoiseCov, Scalar::all(.00005));
		setIdentity(KF.measurementNoiseCov, Scalar::all(0.02)); // this is inverse to the tracking speed
		setIdentity(KF.errorCovPost, Scalar::all(1));

        for (;;) {
			Mat prediction = KF.predict();

			float px = prediction.at<float>(0);
			float py = prediction.at<float>(1);

			measurement.at<float>(0) = rectSample.x0;
			measurement.at<float>(1) = rectSample.y0;
			KF.correct(measurement);

			Point pt1, pt2, pt3;
			pt1.x = p.x;
			pt1.y = p.y;
			pt2.x = p.x + rectSample.w;
			pt2.y = p.y + rectSample.h;
			pt3.x = s.x;
			pt3.y = s.y;

            Mat data = dataOriginal.clone();

			rectangle(data, pt1, pt2, cv::Scalar(0,255,0), 5);
			rectangle(data, pt3, pt2, cv::Scalar(255,0,0), 1);

			imshow("janela", data);
            cv::waitKey(30);
            char code = (char)waitKey(10);
            if( code > 0 )
                break;
		}
	}
}
#endif


#if 0
void Tracking::Execute() {

	size_t frameID;
	SSFSamples *ssfSamples;
	vector<SMIndexType> *samples;
	SSFSample *sample;
	float x = 0,y = 0;
	SSFImage *img;
	Mat data;
	namedWindow("janela");

	//Pode pegar o ultimo frame processado?
	//Faria sentido caso a cada frame computado fosse chamado o kalman.
	//while ((frameID = glb_sharedMem.RetrieveLastProcessedFrameID(inputModID, SSF_DATA_SAMPLES, true)) != SM_INVALID_ELEMENT){
		
		frameID = glb_sharedMem.RetrieveLastProcessedFrameID(inputModID, SSF_DATA_SAMPLES, true);
		img = glb_sharedMem.GetFrameImage(frameID);
		data = img->RetrieveCopy();
		//Pegue todas as samples do frameid x;
		ssfSamples = glb_sharedMem.GetFrameSamples(inputModID, frameID);
		//Pegue todos os indices das samples do frameid x; 
		samples = ssfSamples->GetSamples();
		Point2f s, p;

		for (int i = 0; i < samples->size(); i++) {

			sample = glb_sharedMem.GetSample(samples->at(i));

			// processing the current frame
			ReportStatus("[Feed: %d] Kalman for frameID '%d'", inputModID,  frameID);

			// Pegar a janela correspondente a sample
			SSFRect rectSample = sample->GetWindow();

			//Resolução inocente
			Point2f aux(rectSample.x0,rectSample.y0);

			//x e y do pixel a ser rastreado
			initKalman(aux.x,aux.y);
			
			for (int j = 0; j < 10; j++) {

				//Prediz
				p = kalmanPredict();
				s = kalmanCorrect(p.x,p.y);


				if(s.x != p.x || s.y != p.y){
					cout << "kalman prediction: " << p.x << " " << p.y << endl;
					cout << "kalman corrected state: " << s.x << " " << s.y << endl;
				}


				//crop image and sending for shared memory
				SSFRect ret(p.x,p.y,rectSample.w,rectSample.h);		

				Point pt1, pt2, pt3;
				pt1.x = p.x;
				pt1.y = p.y;
				pt2.x = p.x + rectSample.w;
				pt2.y = p.y + rectSample.h;
				pt3.x = s.x;
				pt3.y = s.y;

				rectangle(data, pt1, pt2, cv::Scalar(0,255,0), 5);
				rectangle(data, pt3, pt2, cv::Scalar(255,0,0), 1);

				// Guardar o resultado
				imshow("janela", data);
				waitKey(30);
				aux = pt1;
				cout<<i;
			}
	
		}			
	//}
		
}
#endif 

#if 0
void Tracking::Execute() {

	size_t frameID;
	SSFSamples *ssfSamples;
	vector<SMIndexType> *samples;
	SSFSample *sample;
	float x = 0,y = 0;
	SSFImage *img;
	Mat data;
	namedWindow("janela");

	//Pode pegar o ultimo frame processado?
	//Faria sentido caso a cada frame computado fosse chamado o kalman.
	//while ((frameID = glb_sharedMem.RetrieveLastProcessedFrameID(inputModID, SSF_DATA_SAMPLES, true)) != SM_INVALID_ELEMENT){
		
		frameID = glb_sharedMem.RetrieveLastProcessedFrameID(inputModID, SSF_DATA_SAMPLES, true);
		img = glb_sharedMem.GetFrameImage(frameID);
		data = img->RetrieveCopy();
		//Pegue todas as samples do frameid x;
		ssfSamples = glb_sharedMem.GetFrameSamples(inputModID, frameID);
		//Pegue todos os indices das samples do frameid x; 
		samples = ssfSamples->GetSamples();
		Point2f s, p;

		for (int i = 0; i < samples->size(); i++) {

			sample = glb_sharedMem.GetSample(samples->at(i));

			// processing the current frame
			ReportStatus("[Feed: %d] Kalman for frameID '%d'", inputModID,  frameID);

			// Pegar a janela correspondente a sample
			SSFRect rectSample = sample->GetWindow();

			//Resolução inocente
			Point2f aux(rectSample.x0,rectSample.y0);

			//x e y do pixel a ser rastreado
			initKalman(aux.x,aux.y);
			
			for (int j = 0; j < 10; j++) {

				//Prediz
				p = kalmanPredict();
				s = kalmanCorrect(p.x,p.y);


				if(s.x != p.x || s.y != p.y){
					cout << "kalman prediction: " << p.x << " " << p.y << endl;
					cout << "kalman corrected state: " << s.x << " " << s.y << endl;
				}


				//crop image and sending for shared memory
				SSFRect ret(p.x,p.y,rectSample.w,rectSample.h);		

				Point pt1, pt2, pt3;
				pt1.x = p.x;
				pt1.y = p.y;
				pt2.x = p.x + rectSample.w;
				pt2.y = p.y + rectSample.h;
				pt3.x = s.x;
				pt3.y = s.y;

				rectangle(data, pt1, pt2, cv::Scalar(0,255,0), 5);
				rectangle(data, pt3, pt2, cv::Scalar(255,0,0), 1);

				// Guardar o resultado
				imshow("janela", data);
				waitKey(30);
				aux = pt1;
				cout<<i;
			}
	
		}			
	//}
		
}
#endif

//
//void Tracking::Execute() {
//
//    Mat img(500, 500, CV_8UC3);
//    KalmanFilter KF(2, 1, 0);
//    Mat state(2, 1, CV_32F); /* (phi, delta_phi) */
//    Mat processNoise(2, 1, CV_32F);
//    Mat measurement = Mat::zeros(1, 1, CV_32F);
//    char code = (char) -1;
//
//    for(;;)
//    {
//        randn( state, Scalar::all(0), Scalar::all(0.1) );
//        KF.transitionMatrix = *(Mat_<float>(2, 2) << 1, 1, 0, 1);
//
//        setIdentity(KF.measurementMatrix);
//        setIdentity(KF.processNoiseCov, Scalar::all(1e-5));
//        setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));
//        setIdentity(KF.errorCovPost, Scalar::all(1));
//
//        randn(KF.statePost, Scalar::all(0), Scalar::all(0.1));
//
//        for(;;)
//        {
//            Point2f center(img.cols*0.5f, img.rows*0.5f);
//            float R = img.cols/3.f;
//            double stateAngle = state.at<float>(0);
//            Point statePt = calcPoint(center, R, stateAngle);
//
//            Mat prediction = KF.predict();
//            double predictAngle = prediction.at<float>(0);
//            Point predictPt = calcPoint(center, R, predictAngle);
//
//            randn( measurement, Scalar::all(0), Scalar::all(KF.measurementNoiseCov.at<float>(0)));
//
//            // generate measurement
//            measurement += KF.measurementMatrix*state;
//
//            double measAngle = measurement.at<float>(0);
//            Point measPt = calcPoint(center, R, measAngle);
//
//            // plot points
//            #define drawCross( center, color, d )                                 \
//                line( img, Point( center.x - d, center.y - d ),                \
//                             Point( center.x + d, center.y + d ), color, 1, CV_AA, 0); \
//                line( img, Point( center.x + d, center.y - d ),                \
//                             Point( center.x - d, center.y + d ), color, 1, CV_AA, 0 )
//
//            img = Scalar::all(0);
//            drawCross( statePt, Scalar(255,255,255), 3 );
//            drawCross( measPt, Scalar(0,0,255), 3 );
//            drawCross( predictPt, Scalar(0,255,0), 3 );
//            line( img, statePt, measPt, Scalar(0,0,255), 3, CV_AA, 0 );
//            line( img, statePt, predictPt, Scalar(0,255,255), 3, CV_AA, 0 );
//
//            if(theRNG().uniform(0,4) != 0)
//                KF.correct(measurement);
//
//            randn( processNoise, Scalar(0), Scalar::all(sqrt(KF.processNoiseCov.at<float>(0, 0))));
//            state = KF.transitionMatrix*state + processNoise;
//
//            imshow( "Kalman", img );
//            code = (char)waitKey(100);
//
//            if( code > 0 )
//                break;
//        }
//        if( code == 27 || code == 'q' || code == 'Q' )
//            break;
//    }
//}

		
