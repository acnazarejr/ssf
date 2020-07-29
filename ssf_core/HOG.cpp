#include "HOG.h"

#define PI 3.14159265358979323846
#define EPS 0.00000001

HOG::HOG(){
	string input_module_positive;
	string input_module_negative;

	nbins = 9;	
	inputParams.Add("nBins", &nbins, "Number of bins of the histogram ", true);

	cell_rows = 2;
	inputParams.Add("cell_rows", &cell_rows, "Number of vertical cells ", true);
	cell_cols = 2;
	inputParams.Add("cell_cols", &cell_cols, "Number of horizontal cells", true);
		
	clipping = 0.0f;
	inputParams.Add("clipping", &clipping, "clipping treshold", false);	
	
	normalization_type = "";
	inputParams.Add("normalization_type", &normalization_type, "Normalization type, L1 or L2", false);		
}


HOG::~HOG(){
	delete[] integral_mat;//free array of integral_mat
}

void HOG::Setup(string ID, AllParameters *params){
/* Set input parameters */
	inputParams.SetParameters(ID, params);
	integral_mat = new SSFMatrix<FeatType>[nbins];

	ncells = cell_rows*cell_cols;
}


int HOG::GetNFeatures(SSFRect &rect){ //ncell*nbins
	return this->ncells*this->nbins;
}


void HOG::ExtractFeatures(int x0, int y0, int x1, int y1, SSFMatrix<FeatType> &m) {
	bool is_verbose = false;
	ofstream normalizationReport;
	if(is_verbose){
		normalizationReport.open("normalization_report.txt",ofstream::out | ofstream::app);
	}
	FeatType sum = 0;
	m.convertTo(m, CV_32F);
	float cell_width = (float)(x1-x0)/(cell_cols);
	float cell_height = (float)(y1-y0)/(cell_rows);
	for(int direction = 0; direction< nbins;direction++){
		/*This block of code extracts features for each cell:*/	
		////for each cell, we extract a vector of features, using integral image histogram of magnitude
		sum=0;
		int cell_counter = 0;
		for(int celly_counter=0; celly_counter < this->cell_rows; celly_counter++){			
			for(int cellx_counter=0; cellx_counter < this->cell_cols; cellx_counter++){					
				float cell_x0 = x0 + cell_width * cellx_counter;
				float cell_x1 = x0 + cell_width * (cellx_counter + 1);
				float cell_y0 = y0 + cell_height * (celly_counter );
				float cell_y1 = y0 + cell_height * (celly_counter + 1);
				sum = integral_mat[direction](cell_y1, cell_x1) + integral_mat[direction](cell_y0, cell_x0) - integral_mat[direction](cell_y1, cell_x0) - integral_mat[direction](cell_y0, cell_x1);		
				m(0, direction + (nbins * cell_counter)) = sum;
				if(is_verbose){
					normalizationReport<<"sum is ["<<sum<<"]"<<endl<<endl;
					//normalizationReport<<"\n\nNormalization Type: ["<<normalization_type<<"]\n\n"<<endl;
				}
				sum = 0;
				cell_counter++;
			}
		}
	/*Cells feature extraction done*/
	}

	/*Print Each cell*/
	if(is_verbose){
		ofstream cell_report;
		cell_report.open("cell_report.txt",ofstream::out);
		for(int cell = 0; cell<ncells;cell++){
			for(int bin = 0;bin<nbins;bin++){
				cell_report<<m(0,bin + nbins*cell)<<",";
			}
			cell_report<<endl;
		}
	}


	

	/*Normalization:*/
	//L1 divide by sum
	if(normalization_type == "L1"){
		int len = nbins*ncells;
		sum =0;
		for (int i = 0; i < len; i++) {		
			sum += m(0,i);
		}
		for (int i = 0; i < len; i++) {		
			m(0,i) = m(0,i)/sum;
			if(is_verbose){
				normalizationReport<<"L1 normalization ["<<m(0,i)<<"]"<<endl;
			}
		}
	}
	//L2 norm
	if(normalization_type == "L2"){
		SSFMatrix<FeatType> m_2;
		SSFMatrix<FeatType> m_t = m.t();
		m_2 = m*m_t;		

		int len = nbins*ncells;
		for (int i = 0; i < len; i++) {		
			m(0,i) = m(0,i)/sqrt(m_2(0,0) + EPS);
			if(is_verbose){
				normalizationReport<<"L2 normalization ["<<m(0,i)<<"]"<<endl;
			}
		}

		if(is_verbose){
			FeatType L2_sum = 0;
			for (int i = 0; i < len; i++) {		
				L2_sum += m(0,i)*m(0,i);				
			}
			ofstream L2_report;
			L2_report.open("L2_report.txt", ofstream::out );
			L2_report<<sqrt(L2_sum)<<endl;
		}
		
	}

	//Clipping 
	if(clipping > 0){
		int len = nbins * ncells;
		for (int i = 0; i < len; i++) {		
			m(0,i) = MAX(m(0,i),clipping);
		}
	}
}

void HOG::SetImage(Mat &img) {
	img.convertTo(img, CV_32F);
	SSFMatrix<FeatType> *gradient_direction = new SSFMatrix<FeatType>[nbins];	
	for(int i =0;i<nbins;i++){
		gradient_direction[i] = Mat::zeros(Size(img.cols, img.rows ), CV_32F);
	}

	bool is_verbose = false;
	ofstream integralMatReport;
	if(is_verbose){
		integralMatReport.open("integralMat.txt", std::ofstream::out);
	}

	computeGradient(img, gradient_direction);
	for(int bin =0; bin < nbins; bin++){
		integral_mat[bin] = Mat::zeros(Size(img.cols + 1, img.rows + 1), CV_32F);				
		integral(gradient_direction[bin], integral_mat[bin], CV_32F);

		if(is_verbose){
			int i,j;
		
			int rows = integral_mat[bin].rows;
			int cols = integral_mat[bin].cols;
			for(i=0; i<rows; i++){
				integralMatReport<<"[";
				for(j=0;j<cols;j++){						
					integralMatReport<<integral_mat[bin](i,j)<<",";
				}
				integralMatReport<<"]"<<endl;				
			}
			integralMatReport<<"\n\n"<<endl;
		}

	}
	if(is_verbose){
		integralMatReport<<endl;
		integralMatReport.close();
	}
	
	delete[] gradient_direction;
}

void HOG::computeGradient(Mat inp, SSFMatrix<FeatType> *gradient_direction){
	bool is_verbose = false;
	ofstream gradientReport;
	FeatType gradient_magnitude = 0;
	if(is_verbose){
		
		gradientReport.open("gradient_report.txt",ofstream::out);
	}
	SSFMatrix<float> dxmask(3, 1, DataType<float>::type);
	SSFMatrix<float> dymask(1, 3, DataType<float>::type);

	
	dxmask.at<float>(0,0) = -1;
	dxmask.at<float>(1,0) = 0;
	dxmask.at<float>(2,0) = 1;
	
	dymask.at<float>(0,0) = -1;
	dymask.at<float>(0,1) = 0;
	dymask.at<float>(0,2) = 1;

	
	
	float v[3], tanv;
	int x = 0;
	int y = 0;

	SSFMatrix<FeatType> channels_x[3];
	SSFMatrix<FeatType> channels_y[3];

	cv::split(inp, channels_x);
	cv::split(inp, channels_y);
	if(is_verbose){
	ofstream channels_x_report;
	channels_x_report.open("channels_x_report.txt",ofstream::out);

		int rows = channels_x[0].rows;
		int cols = channels_x[0].cols;
		for(int color = 0; color<3;color++){
			channels_x_report<<"COLOR "<<color<<endl;
			for(int i =0;i<rows;i++){
				for(int j=0;j<cols;j++){
					channels_x_report<<channels_x[color](i,j)<<",";
				}
					channels_x_report<<endl;
			}
			channels_x_report<<endl;
		}
		channels_x_report.close();
	}

	if(is_verbose){
	ofstream channels_y_report;
	channels_y_report.open("channels_y_report.txt",ofstream::out);

		int rows = channels_y[0].rows;
		int cols = channels_y[0].cols;
		for(int color = 0; color<3;color++){
			channels_y_report<<"COLOR "<<color<<endl;
			for(int i =0;i<rows;i++){
				for(int j=0;j<cols;j++){
					channels_y_report<<channels_y[color](i,j)<<",";
				}
					channels_y_report<<endl;
			}
			channels_y_report<<endl;
		}
		channels_y_report.close();
	}

	for(int color =0;color<3;color++){
		filter2D(channels_x[color], channels_x[color], -1, dxmask, Point(0,0), 0.0, 4);
		filter2D(channels_y[color], channels_y[color], -1, dymask, Point(0,0), 0.0, 4);
	}

	if(is_verbose){
	ofstream channels_x_report;
	channels_x_report.open("channels_x_filter_report.txt",ofstream::out);

		int rows = channels_x[0].rows;
		int cols = channels_x[0].cols;
		for(int color = 0; color<3;color++){
			channels_x_report<<"COLOR "<<color<<endl;
			for(int j =0;j<rows;j++){
				for(int i=0;i<cols;i++){
					channels_x_report<<channels_x[color](j,i)<<",";
				}
					channels_x_report<<endl;
			}
			channels_x_report<<endl;
		}
		channels_x_report.close();
	}

	if(is_verbose){
	ofstream channels_y_report;
	channels_y_report.open("channels_y_filter_report.txt",ofstream::out);

		int rows = channels_y[0].rows;
		int cols = channels_y[0].cols;
		for(int color = 0; color<3;color++){
			channels_y_report<<"COLOR "<<color<<endl;
			for(int j =0;j<rows;j++){
				for(int i=0;i<cols;i++){
					channels_y_report<<channels_y[color](j,i)<<",";
				}
					channels_y_report<<endl;
			}
			channels_y_report<<endl;
		}
		channels_y_report.close();
	}

			

	/*
	* Compute Gradient Magnitudes and Orientations:
	*/	
	/*commented unused lines:
	*SSFMatrix<float> gradient_magnitude(inp.rows, inp.cols);
	*SSFMatrix<bool> gradient_direction[9];
	*SSFMatrix<short> channel_picked;
	*/		
	for (y = 0; y < inp.rows; y++) {
	if(is_verbose){
		gradientReport<<" [";
	}
		for (x = 0; x < inp.cols; x++) {//		
			for(int color = 0; color<3;color++){
				v[color] = channels_x[color].at<float>(y,x) * channels_x[color].at<float>(y,x) +
					channels_y[color].at<float>(y,x)* channels_y[color].at<float>(y,x);
			}		
			if (v[0] > v[1]) {
				if (v[0] > v[2]) {
					tanv = atan2((float) channels_y[0].at<float>(y,x), (float) channels_x[0].at<float>(y,x));
					gradient_magnitude = sqrt(v[0]);
					//channel_picked.at<short>(x,y) = 0;
				}
				else {
					tanv = atan2((float) channels_y[2].at<float>(y,x), (float) channels_x[2].at<float>(y,x));
					gradient_magnitude = sqrt(v[2]);
					//channel_picked.at<short>(x,y) = 2;					
				}
			}
			else {
				if (v[1] > v[2]) {
					tanv = atan2((float) channels_y[1].at<float>(y,x), (float) channels_x[1].at<float>(y,x));
					gradient_magnitude = sqrt(v[1]);
					//channel_picked.at<short>(x,y) = 1;
				}
				else {
					tanv = atan2((float) channels_y[2].at<float>(y,x), (float) channels_x[2].at<float>(y,x));
					gradient_magnitude = sqrt(v[2]);
					//channel_picked.at<short>(x,y) = 2;
				}
			}

			if (tanv < 0){
				tanv += (float) PI;
			}
			// set in the matrix
			int bin = floor(tanv * this->nbins / (PI + 1e-6));
			gradient_direction[bin].at<FeatType>(y,x) = gradient_magnitude;
			if(is_verbose){
				gradientReport<<gradient_direction[bin].at<FeatType>(y,x)<<",";
			}
		}
		if(is_verbose){
			gradientReport<<"]"<<endl;
		}
	}
	if(is_verbose){
		gradientReport<<endl;
		gradientReport.close();
	}
}

HOG *HOG::New() { 

	return new HOG(); 
}