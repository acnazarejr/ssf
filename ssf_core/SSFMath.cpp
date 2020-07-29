#include "headers.h"
#include "ap.h"
#include "reflections.h" 
#include "qr.h"
#include "SSFData.h"
#include "SSFRect.h"
#include "SSFMath.h"





int SSFRectInt(SSFRect rectA, SSFRect rectB) {
int leftA, bottomA, rightA, topA;
int leftB, bottomB, rightB, topB;

	leftA = rectA.x0;
	bottomA = rectA.y0;
	rightA = leftA + rectA.w;
	topA = bottomA + rectA.h;

	leftB = rectB.x0;
	bottomB = rectB.y0;
	rightB = leftB + rectB.w;
	topB = bottomB + rectB.h;

	return (max(0, min(rightA, rightB) - max(leftA, leftB))) * (max(0, min(topA, topB) - max(bottomA, bottomB)));
}






void QR_factor(SSFMatrix<FeatType> &A, SSFMatrix<FeatType> &Q, SSFMatrix<FeatType> &R) {
ap::real_2d_array a, q, r;
ap::real_1d_array tau;
int x, y;


	a.setbounds(0, A.rows - 1, 0, A.cols);

	for (y = 0; y < A.rows; y++) {
		for (x = 0; x < A.cols; x++) {
			a(y, x) = A(y, x);
		}
	}


	rmatrixqr(a, A.rows, A.cols, tau);
	
	rmatrixqrunpackq(a, A.rows, A.cols, tau, A.cols, q);

	rmatrixqrunpackr(a, A.rows, A.cols, r);

	R.release();
	R.create(r.gethighbound(2) + 1, r.gethighbound(2) + 1);

	for (y = 0; y < R.rows; y++) {
		for (x = 0; x < R.cols; x++) {
			R(y,x) = (FeatType) r(y, x);
		}
	}

	Q.release();
	Q.create(q.gethighbound(1) + 1, q.gethighbound(2) + 1);

	for (y = 0; y < Q.rows; y++) {
		for (x = 0; x < Q.cols; x++) {
			Q(y,x) = (FeatType) q(y, x);
		}
	}
}