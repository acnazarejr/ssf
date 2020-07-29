#include "headers.h"
#include "ClassificationMethod.h"
#include "QDA.h"

#define QDAEPS 0.00000001

QDA::QDA(DataStartClassification dataStart) : ClassificationMethod(dataStart) {

	logPriori = -1;
	logDetSigma1 = -1;
	logDetSigma2 = -1;
}




// learn the classifier
void QDA::Learn() { 
set<string>::iterator it;
int i, j, n, nclasses, totalSamples;
map<string,int> nsamples;
vector<string> classIDs;
SSFMatrix<FeatType> mMean;				// mean of each class
vector<SSFMatrix<FeatType> > mclass;	// matrix containing samples of each class
SSFMatrix<FeatType> auxMat;
vector<FeatType> logDetSigma;
SSFMatrix<FeatType> Q, R, w;
vector<SSFMatrix<FeatType> > invs;		// inverses of Rk
vector<FeatType> prior;
int nfeats;

	// set number of classes
	nclasses = (int) labels.size();
	if (labels.size() != 2)
		ReportError("QDA classifier takes only two classes");

	// get number of samples and features
	totalSamples = (int) dataX.rows;
	nfeats = (int) dataX.cols;

	// compute the number of samples for each class
	for (it = labels.begin(); it != labels.end(); ++it) {
		nsamples[*it] = 0;
		classIDs.push_back(*it);
	}
	
	for (i = 0; i < (int) dataY.size(); i++) {
		nsamples[dataY[i]]++;
	}


	// create matrices containing samples of each class separately
	auxMat.create(0, nfeats);
	mclass.push_back(auxMat);
	auxMat.create(0, nfeats);
	mclass.push_back(auxMat);
	for (i = 0; i < totalSamples; i ++) {
		if (dataY[i] == classIDs[0])
			mclass[0].push_back(dataX.row(i));
		else
			mclass[1].push_back(dataX.row(i));
	}

	logDetSigma.assign(2, 0);

	// remove mean of each data matrix and compute QR decomposition and SVD to find logdetsigmas
	for (n = 0; n < nclasses; n++) {

		// compute mean of each class
		auxMat = SSFMatrixMean(mclass[n]);
		mMean.push_back(auxMat);

		/*
		% Stratified estimate of covariance.  Do not do pivoting, so that A
		% can be computed without unpermuting.  Instead use SVD to find rank
		% of R.
		[Q,Rk] = qr(training(gindex==k,:) - repmat(gmeans(k,:), gsize(k), 1), 0);
		Rk = Rk / sqrt(gsize(k) - 1); % SigmaHat = R'*R
		*/
		// remove mean
		for (i = 0; i < mclass[n].rows; i++) {
			mclass[n].row(i) = mclass[n].row(i) - mMean.row(n);
		}

		// run QR decomposition
		QR_factor(mclass[n], Q, R);

		for (i = 0; i < R.rows; i++) {
			for (j = 0; j < R.cols; j++) {
				R(i, j) = R(i, j) / sqrt((FeatType) mclass[n].rows - 1.0f);
		///		R->SetValue(j, i, R->GetElement(j, i) / sqrt((float)nsamples[n] - 1.0f));
			}
		}


		/*
		s = svd(Rk);
		if any(s <= max(gsize(k),d) * eps(max(s)))
			error('stats:classify:BadVariance',...
				  'The covariance matrix of each group in TRAINING must be positive definite.');
		end
		logDetSigma(k) = 2*sum(log(s)); % avoid over/underflow
		*/
		SVD svd(R);
		w = svd.w;

		// check consistency
		for (i = 0; i < w.rows; i++) {
			if (w(i,0) <= QDAEPS * .001) {
				ReportError("The covariance matrix of each group in TRAINING must be positive definite.\n");
				exit(2);
			}
		}

		for (i = 0; i < w.rows; i++) {
			logDetSigma[n] += log(w(i,0));
		}
		logDetSigma[n] = 2 * logDetSigma[n];

		// get inverse of R
		invert(R, auxMat, DECOMP_LU);
		invs.push_back(auxMat);
	}


	/*
		prior = ones(1, ngroups) / ngroups;
	 */
	// set priors
	prior.assign(2, (1.0f/(FeatType) nclasses));

	// set variables for training
	invR1 = invs[0];
	invR2 = invs[1];
	Gmeans = mMean;
	mMean.row(0).copyTo(Gmean1);
	mMean.row(1).copyTo(Gmean2);
	logPriori = prior[0];
	logDetSigma1 = logDetSigma[0];
	logDetSigma2 = logDetSigma[1];

	// clean memory
	mclass[0].release();
	mclass[1].release();
	mMean.release();
	auxMat.release();
	Q.release();
	R.release();
	w.release();
	invs[0].release();
	invs[1].release();
}





void QDA::Classify(const SSFMatrix<FeatType> &X, SSFMatrix<FeatType> &responses)  {
SSFMatrix<FeatType> auxMat, auxMat2;
FeatType v1, v2; 
int n;

	if (logPriori == -1)
		ReportError("load QDA training first");

	responses.create(X.rows, 2);

	for (n = 0; n < X.rows; n++) {
		///////////////
		// for class 1:
		// subtract mean, get v1
		//mat.SubtractVectorsSSE(projectedvector, Gmean1v, v1auxv, nfactors);
		auxMat = X.row(n) - Gmean1;

		// project v1 to inv of R1 get v2
		auxMat2 = auxMat * invR1;

		// dot product of v2 and v2 get v3 (square elements of v2 and add them up)
		auxMat2 = auxMat2 * auxMat2.t();
		v1 = auxMat2(0, 0);

		// Logprior1 - (.5*v3) + logDetSigma1;  get V1
		v1 = logPriori - (FeatType) 0.5 * (v1 + logDetSigma1);


		///////////////
		// for class 2:
		// subtract mean
		//mat.SubtractVectorsSSE(projectedvector, Gmean2v, v2auxv, nfactors);
		auxMat = X.row(n) - Gmean2;

		// project v1 to inv of R1 
		auxMat2 = auxMat * invR2;

		// dot product of v2 and v2  (square elements of v2 and add them up)
		auxMat2 = auxMat2 * auxMat2.t();
		v2 = auxMat2(0,0);

		// Logprior1 - (.5*v3) + logDetSigma1;  get V1
		v2 = logPriori - (float) 0.5 * (v2 + logDetSigma2);


		//////////////////
		// Compute the posterior probability
		// find max between V1 and V2, get maxD
		/*
		maxD = max(v1, v2);

		// P1 = exp(V1 - maxD)
		responses(n, 0) = exp(v1 - maxD);

		// P2 = exp(V2 - maxD)
		responses(n, 1) = exp(v2 - maxD);

		// add P1 and P2, get sumP
		sumP = responses(n, 0) + responses(n, 1);

		// P1 / sumP get posterior 1
		responses.row(n) /= sumP;

		responses(n, 0) = 1.0f - responses(n, 1);
		responses(n, 1) = 1.0f - responses(n, 0);
		v1 = responses(n, 0);
		v2 = responses(n, 1);
		*/

		responses(n, 0) = v1 - v2;
		responses(n, 1) = v2 - v1;
	}
}





void QDA::Save(SSFStorage &storage) {

	if (storage.isOpened() == false)
		ReportError("Invalid file storage!");


	storage << "QDA" << "{";
	Save_(storage);  // save parameters that are in the ClassificationMethod 
	storage << "logPriori" << logPriori;
	storage << "logDetSigma1" << logDetSigma1;
	storage << "logDetSigma2" << logDetSigma2;
	storage << "Gmeans" << Gmeans;
	storage << "Gmean1" << Gmean1;
	storage << "Gmean2" << Gmean2;
	storage << "invR1" << invR1;
	storage << "invR2" << invR2;
	storage << "}";
}



void QDA::Load(const FileNode &node, SSFStorage &storage) {
FileNode n;


	n = node["QDA"];   
	n["logPriori"] >> logPriori;
	n["logDetSigma1"] >> logDetSigma1;
	n["logDetSigma2"] >> logDetSigma2;
	n["Gmeans"] >> Gmeans;
	n["Gmean1"] >> Gmean1;
	n["Gmean2"] >> Gmean2;
	n["invR1"] >> invR1;
	n["invR2"] >> invR2;
	Load_(n);   // load parameters that are in the ClassificationMethod 
}


QDA *QDA::Instantiate(DataStartClassification dataStart) {

	return new QDA(dataStart); 
}