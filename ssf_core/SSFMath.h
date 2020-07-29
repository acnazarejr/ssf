#ifndef _SSF_MATH_
#define _SSF_MATH_
#include "SSFRect.h" 




// compute the intersection between two rectangles
int SSFRectInt(SSFRect rectA, SSFRect rectB);



/* Matrix functions */

#define SSFMatrix Mat_


// compute the mean of the columns of the matrix
template <typename Type> 
SSFMatrix<Type> SSFMatrixMean(SSFMatrix<Type> &m) {
Mat_<Type> retm, aux;
Scalar value;
int x;
	
	retm.create(1, m.cols);

	for (x = 0; x < m.cols; x++) {
		aux = m.col(x);
		value = cv::mean(aux);
		retm[0][x] = (Type) value.val[0];
	}

	return retm;
}


// compute standard deviation if the std for a variable be smaller than eps, it will be set to 1
template <typename Type> 
SSFMatrix<Type> SSFMatrixStd(SSFMatrix<Type> &m, Type eps = 0.001) {
SSFMatrix<Type> retm, aux;
Scalar vmean, stddev;
int x;

	retm.create(1, m.cols);
	for (x = 0; x < m.cols; x++) {
		aux = m.col(x);
		cv::meanStdDev(aux, vmean, stddev);
		if ((Type) stddev.val[0] >= eps)
			retm[0][x] = (Type) stddev.val[0];
		else
			retm[0][x] = (Type) 1;
	}

	return retm;
}


// compute the square sum of the matrix
template <typename Type> 
double SSFMatrixSquare(SSFMatrix<Type> &m) {

	return m.dot(m);
}


// compute M matrix zscore
template <typename Type> 
void SSFMatrixZscore(SSFMatrix<Type> &M, SSFMatrix<Type> &mean, SSFMatrix<Type> &std) {
SSFMatrix<Type> aux;
int y;

	for (y = 0; y < M.rows; y++) {	
		aux = M.row(y);

		aux -= mean;
		aux /= std;
	}
}

template <typename Type> 
SSFMatrix<Type> SSFMatrixNormalize(SSFMatrix<Type> &M, int norm_type = NORM_L2) {
SSFMatrix<Type> aux;

	cv::normalize(M, aux, 1, 0, norm_type);

	return aux;
}

// compute QR decomposition
void QR_factor(SSFMatrix<FeatType> &A, SSFMatrix<FeatType> &Q, SSFMatrix<FeatType> &R);


#endif