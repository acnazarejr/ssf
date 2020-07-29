#include "headers.h"
#include "pls.h"



// Debug MASTLAB
// X = rand(110,50);
// y = rand(110,2);
// generateYMLMatrix(y, 'E:\Users\william\Documents\Dropbox\research\topics\framework\SSF\SSF\Y.yml');
// generateYMLMatrix(X, 'E:\Users\william\Documents\Dropbox\research\topics\framework\SSF\SSF\X.yml');
// text = fileread(yaml_file);
// X2 = YAML.load(text);
// W - reshape(X2.W.data, 10, 50)'
// Wstar - reshape(X2.Wstar.data, 10, 50)'
// Bstar - reshape(X2.Bstar.data, 2, 50)'



void PLS::runpls(SSFMatrix<FeatType> &X, SSFMatrix<FeatType> &Y, int nfactors) {
int i;
int kk;
float dt=0;
int maxsteps, step;
int nsamples, nfeatures;
SSFMatrix<FeatType> c, t, u, t0, Vcol, Vrow, w, p, C, U,  b_l, Yscaled;
SSFMatrix<FeatType> tmpM;

	// initially, clear current PLS model (if there is one)
	//ClearPLS();

	nsamples = X.rows;
	nfeatures = X.cols;

	if (X.rows != Y.rows)
		ReportError("Inconsistent number of rows for matrices X (%d) and Y (%d)", X.rows, Y.rows); 

	maxsteps = 100;

	Xmean = SSFMatrixMean(X);
	Xstd = SSFMatrixStd(X);
	SSFMatrixZscore(X, Xmean, Xstd);

       
	// Y
	//ymean = new Vector<float> (1);
	//ystd = new Vector<float> (1);
	//mean(Y, ymean);
	//std(Y, ymean, ystd);
	//zscore(Y, ymean, ystd);
	Ymean = SSFMatrixMean(Y);
	Ystd = SSFMatrixStd(Y);
	SSFMatrixZscore(Y, Ymean, Ystd);


	//Yscaled = Y->Copy();
	Yscaled = Y.clone();

	U.create(nsamples, nfactors); //U = new Matrix<float> (nsamples, nfactor);
	C.create(1, nfactors); // C = new Vector<float> (nfactor);
	T.create(nsamples, nfactors); //T = new Matrix<float> (nsamples, nfactor);
	P.create(nfeatures, nfactors); // P = new Matrix<float> (nfeatures, nfactor);
	W.create(nfeatures, nfactors); // W = new Matrix<float> (nfeatures, nfactor);
	b.create(1, nfactors);

	
	// compute square of the sum of X and Y
	//sumY2 = 0;
	//sumX2 = 0;
	
	//for (i = 0; i < Y->GetNElements(); i++) {
	//	sumY2 += (double) (Y->GetElement(i) * Y->GetElement(i));
	//}

	//for (i = 0; i < X->GetNCols(); i++) {
	//	for (j = 0; j < X->GetNRows(); j++) {
	//		sumX2 += (double) (X->GetElement(i, j) * X->GetElement(i, j));
	//	}
	//}

	//sumY2 = SSFMatrixSquare(Y);
	//sumX2 = SSFMatrixSquare(X);


	//printf("\rsumX: %5.3f, sumY: %5.3f", sumX2, sumY2);

	for (i = 0; i < nfactors; i++) {

		// normaliz(Y, t);
		t = SSFMatrixNormalize(Y.col(0));

		// CopyVector(t, u);
		u = t.clone();

		ReportStatus("Extracting PLS factor %d/%d", i + 1, nfactors);

		step = 0;
		do {

        	//t0 = CopyVector(t, rY);
			//CopyVector(t, t0);
			t0 = t.clone();

			// w=normaliz(Xres'*u)
			//tmp = MultiplyTransposeMatrixbyVector(X, rX, cX, u, rY);
        	//w = normaliz(tmp, cX);
			//free(tmp);
			//MultiplyTransposeMatrixbyVector(X, u, Vcol);
			//normaliz(Vcol, w);
			tmpM = X.t() * u;
			w = SSFMatrixNormalize(tmpM);


			// t=normaliz(Xres*w);
			//tmp = MultiplyMatrixbyVector(X, rX, cX, w, cX);
			//free(t);
			//t = normaliz(tmp, rX);
			//free(tmp);
			//MultiplyMatrixbyVector(X, w, Vrow);
			//normaliz(Vrow, t);
			tmpM = X * w;
			t = SSFMatrixNormalize(tmpM);



			// c = normaliz(Yres'*t); c is 1xrY
			//tmpscalar = MultiplyVectorTransposedbyVector(Y, rY, t);
			//c = normaliz(&tmpscalar, 1);
			//tmpscalar = MultiplyVectorTransposedbyVector(Y, t);
			//c = tmpscalar/tmpscalar;  //dummy step, because it normalizes a constant
			tmpM = Y.t() * t;
			c = SSFMatrixNormalize(tmpM);
			
			
			// u=Yres*c
			//free(u);
			//u = MultiplyVectorandScalar(Y, rY, c[0]);
			//MultiplyVectorandScalar(Y, c, u);
			u = Y * c;
			


			dt = 0;
            for (kk = 0; kk < nsamples; kk++) {
				dt += (t0(kk,0) - t(kk,0)) * (t0(kk,0) - t(kk,0));
			}
			if (cvIsNaN((double) dt))
				ReportError("Problem during PLS: NaN\n");
                       
 			step++;

			//ReportStatus("Latent Variable #%d, iteration #:%d", i, step);
			//disp(['Latent Variable #',int2str(l),'  Iteration #:',int2str(nstep)])

		} while (dt>0.000001 && step<maxsteps);
		
		// p=Xres'*t; p is cX by 1
		//p = MultiplyTransposeMatrixbyVector(X, rX, cX, t, rX);
		//MultiplyTransposeMatrixbyVector(X, t, p);
		p = X.t() * t;


		// b_l=((t'*t)^(-1))*(u'*t); // ||t|| = 1
		//b_l = MultiplyVectorTransposedbyVector(u, t);
		b_l = t.t() * t;
		b_l = b_l.inv() * (u.t() * t);

		// store matrices
		//b[i] = b_l;
		//b->SetElement(i, b_l);
		b[0][i] = b_l[0][0];
	

		//for (j = 0; j < nfeatures; j++) {
		//	P->SetValue(i, j, p->GetElement(j));
		//	W->SetValue(i, j, w->GetElement(j));
		//}
		p.copyTo(P.col(i));
		w.copyTo(W.col(i));


		//for (j = 0; j < cX; j++) {
	   // 	WMELEM(P, j, i, p[j], nfactor);
    	//	WMELEM(W, j, i, w[j], nfactor);
		//}

		//for (j = 0; j < nsamples; j++) {
		//	T->SetValue(i, j, t->GetElement(j));
		//	U->SetValue(i, j, u->GetElement(j));
		//}
		t.copyTo(T.col(i));
		u.copyTo(U.col(i));

		//for (j = 0; j < rX; j++) {
		//	WMELEM(T, j, i, t[j], nfactor);
    	//	WMELEM(U, j, i, u[j], nfactor);
		//}

		//c->SetElement(i, c);
		C[0][1] = c[0][0];
		

	    // deflation of X and Y
		// Xres=Xres-t*p';
		//SubtractFromMatrix(X, rX, cX, t, rX, p, cX);
		// SubtractFromMatrix(X, t, p);
		X = X - (t * p.t());
		

		// Yres=Yres-(b(l)*(t*c'));
		//SubtractFromVector(Y, rY, t, rX, c, 1, b[i]);
		//SubtractFromVector(Y, t, c, b->GetElement(i));
		Y = Y - (b[0][i] * (t * c.t()));   
	}



	//ComputeWstar();
	//Wstar=W*inv(P'*W);
	tmpM = P.t() * W;
	Wstar = W * tmpM.inv();

	//Bstar = Wstar*inv(T'*T)*T'*Y; 
	tmpM = (T.t() * T).inv();
	Bstar = Wstar * tmpM * T.t() * Yscaled;

	// set max number of factors
	this->nfactors = nfactors;
}

int PLS::GetNFactors() {

	return this->nfactors;
}

void PLS::Projection(const SSFMatrix<FeatType> &X, SSFMatrix<FeatType> &projX, int nfactors) {
SSFMatrix<FeatType> aux, aux2;
int i, y;

	if (nfactors > this->nfactors)
		ReportError("Maximum number of factors (%d) has been exceeded", this->nfactors);

	projX.create(X.rows, nfactors);

	for (y = 0; y < X.rows; y++) {
		aux = X.row(y);

		// zscore
		zdataV = aux - Xmean;
		zdataV /= Xstd;

		for (i = 0; i < nfactors; i++) {
			aux2 = Wstar.col(i);
			projX[y][i] = (FeatType) zdataV.dot(aux2.t());
		}
	}
}




void PLS::ProjectionBstar(const SSFMatrix<FeatType> &X, SSFMatrix<FeatType> &ret) {
SSFMatrix<FeatType> aux, tmp;
int y, i;

	ret.create(X.rows, Bstar.cols);

	for (y = 0; y < X.rows; y++) {
		aux = X.row(y);

		if (aux.cols != Xmean.cols)
			ReportError("Inconsistent data matrix");

		// zscore
		zdataV = aux - Xmean;
		zdataV /= Xstd;

		// X * Bstar .* Ydata.std) +  Ydata.mean;
		tmp = zdataV * Bstar;
		tmp = tmp.mul(Ystd) + Ymean;

		for (i = 0; i < tmp.cols; i++) {
			ret[y][i] = tmp[0][i];
		}
	}
}






void PLS::Save(SSFStorage &storage) {

	if (storage.isOpened() == false)
		ReportError("Invalid file storage!");

	storage << "PLS" << "{";
	storage << "nfactors" << nfactors;
	storage << "Xmean" << Xmean;
	storage << "Xstd" << Xstd;
	storage << "b" << b;
	storage << "T" << T;
	storage << "P" << P;
	storage << "W" << W;
	storage << "Wstar" << Wstar;
	storage << "Bstar" << Bstar;
	storage << "Ymean" << Ymean;
	storage << "Ystd" << Ystd;
	
	storage << "}";
}


void PLS::Load(const FileNode &node) {
FileNode n;

	n = node["PLS"];   

	n["nfactors"] >> nfactors;
	n["Xmean"] >> Xmean;
	n["Xstd"] >> Xstd;
	n["b"] >> b;
	n["T"] >> T;
	n["P"] >> P;
	n["W"] >> W;
	n["Wstar"] >> Wstar;
	n["Bstar"] >> Bstar;
	n["Ymean"] >> Ymean;
	n["Ystd"] >> Ystd;

	if (b.data == NULL)
		ReportError("Couldn't read 'b' from PLS model");
}


void PLS::Save(string filename) {
SSFStorage storage;

	storage.open(filename, SSF_STORAGE_WRITE);
	this->Save(storage);
	storage.release();
}


void PLS::Load(string filename) {
SSFStorage storage;

	storage.open(filename, SSF_STORAGE_READ);
	this->Load(storage.root());
	storage.release();
}


