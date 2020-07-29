
#include "headers.h"
#include "LAP.h" //Locking at people
#include "CVPedestrianDetection.h"

CVPedestrianDetection instance("");

/******
* class CVPedestrianDetection
******/
CVPedestrianDetection::CVPedestrianDetection(string modID) :  Module(modID) {

	/* Add parameters in file paramstest*/
	//modParams.Add("feedName1", &feedNameTrain, "desc", true); 

	/* set input and output data */
	modParams.Require(modID, SSF_DATA_IMAGE, &inputModID);	// input image
	modParams.Provide(modID, SSF_DATA_SAMPLES); // output mask
	modParams.Provide(modID, SSF_DATA_SAMPLE);	// output

	/* register the module */
	this->RegisterModule(GetModName(), GetModType(), this, modID);
}

void CVPedestrianDetection::Setup() {

	if (inputModID == "")
		ReportError("Input module with images has not been set (variable: inputModID)!");
}

Module *CVPedestrianDetection::Instantiate(string modID) {

	return new CVPedestrianDetection(modID); 
}

void CVPedestrianDetection::Execute() {

	size_t position=0;
	SMIndexType frameID, id;
	SSFImage *img;
	Mat data;	
	vector<SMIndexType> samples;
	//namedWindow("janela");

	//namedWindow(feedNameTrain);
	while ((frameID = glb_sharedMem.RetrieveFrameIDbyPosition(inputModID, SSF_DATA_IMAGE, position++)) != SM_INVALID_ELEMENT) {

		img = glb_sharedMem.GetFrameImage(frameID);
		data = img->RetrieveCopy();
		
		// Extract Hog
		HOGDescriptor hog;
		hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
		vector<Rect> found, found_filtered;

		// run the detector with default parameters. to get a higher hit-rate
		// (and more false alarms, respectively), decrease the hitThreshold and
		// groupThreshold (set groupThreshold to 0 to turn off the grouping completely).
		hog.detectMultiScale(data, found, 0, Size(8,8), Size(32,32), 1.05, 2);

		size_t i, j;

		for( i = 0; i < found.size(); i++ ){
			Rect r = found[i];
			for( j = 0; j < found.size(); j++ )
				if( j != i && (r & found[j]) == r)
					break;
			if( j == found.size() )
				found_filtered.push_back(r);
		}
		for( i = 0; i < found_filtered.size(); i++ ){
			Rect r = found_filtered[i];

			// the HOG detector returns slightly larger rectangles than the real objects.
			// so we slightly shrink the rectangles to get a nicer output.
			r.x += cvRound(r.width*0.1);
			r.width = cvRound(r.width*0.8);
			r.y += cvRound(r.height*0.07);
			r.height = cvRound(r.height*0.8);
			//Lembrar o vitor que aqui tem que cropar a imagem e mandar para memoria compartilhada
			rectangle(data, r.tl(), r.br(), cv::Scalar(0,255,0), 3);

			//crop image and sending for shared memory
			SSFRect ret(r.x,r.y,r.width,r.height);		
				
			SSFSample *sample = new SSFSample();
			sample->SetPatch(ret);
			
			id = glb_sharedMem.AddFrameSample(frameID, sample);
			//id = glb_sharedMem.SetFrameSample(frameID, sample); 
			/*
			FIXME FIXME FIXME
			void AddDataItem(SMIndexType smID); // adiciona sample de um a um

			void EndWriting(); // indica que a escrita está finalizada

			SMIndexType GetDataEntry(SMIndexType position); // retorna a entrada da posição position (arguarda até ser gerada) se o usuário setar EndWriting(), ela será liberada e retornará SM_INVALID_ELEMENT


			A função SSFSamples::vector<SMIndexType> GetSamples(); continua existindo, mas ela espera que o EndWriting() seja chamado para retornar dos dados, não retorna antes disso.

			A função void SetSamples(vector<SMIndexType> &samplesID); também existe, quando ela escreve os samples, o EndWriting() é chamado. 
			*/

			//samples.push_back(id);			
		}
		glb_sharedMem.EndFrameSamples(frameID);
		//glb_sharedMem.SetFrameSamples(frameID, samples);
		//imshow("janela", data);
	}
}


