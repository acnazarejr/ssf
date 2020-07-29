#ifndef CO_OCCURRENCEGENERAL_H
#define CO_OCCURRENCEGENERAL_H


#define COOCALLDIRS

#define FEATURESCG 12
#define SIGN(x,y) ((y)<0 ? -fabs(x) : fabs(x))
#define SWAP(a,b) {y=(a);(a)=(b);(b)=y;}

#define PGM_MAXMAXVAL 257

#define RADIX 2.0
#define EPSILON 0.000000001
#define COOC_MAX_MEAS 14
#define COOC_MAX_ANGLES 4
#undef MEAN





typedef unsigned short gray;


class co_occurrence_general {

	// auxiliar variables
	float Pxpy[2 * PGM_MAXMAXVAL];
	int nbins;
	int distance;


	SSFMatrix<FeatType> M0, M45, M90, M135;


	float **matrix (int nrl, int nrh, int ncl, int nch);

	float *cvector (int nl, int nh);

	float f1_asm (float **P, int Ng);
	float f1_asm (SSFMatrix<FeatType> &P, int Ng);

	float f2_contrast (float **P, int Ng);
	float f2_contrast (SSFMatrix<FeatType> &P, int Ng);

	float f3_corr (float **P, int Ng);
	float f3_corr (SSFMatrix<FeatType> &P, int Ng);

	float f4_var (float **P, int Ng);
	float f4_var (SSFMatrix<FeatType> &P, int Ng);

	float f5_idm (float **P, int Ng);
	float f5_idm (SSFMatrix<FeatType> &P, int Ng);

	float f6_savg (float **P, int Ng);
	float f6_savg (SSFMatrix<FeatType> &P, int Ng);

	float f7_svar (float **P, int Ng, float S);
	float f7_svar (SSFMatrix<FeatType> &P, int Ng, float S);

	float f8_sentropy (float **P, int Ng);
	float f8_sentropy (SSFMatrix<FeatType> &P, int Ng);

	float f9_entropy (float **P, int Ng);
	float f9_entropy (SSFMatrix<FeatType> &P, int Ng);

	float f10_dvar (float **P, int Ng);
	float f10_dvar (SSFMatrix<FeatType> &P, int Ng);

	float f11_dentropy (float **P, int Ng);
	float f11_dentropy (SSFMatrix<FeatType> &P, int Ng);

	float f15_directionality (float **P, int Ng);
	float f15_directionality (SSFMatrix<FeatType> &P, int Ng);

	// compute features without considering feature selection
	void coocanglexNewNewNoFeatSelection(SSFMatrix<FeatType> &m, int tones, SSFMatrix<FeatType> &mout);

public:
	
	co_occurrence_general();
	~co_occurrence_general();

	// initialize method
	void Initialize(int nbins, int distance);

	// co-occurrence without feature selection
	void ExtractFeatures(int x0, int y0, int x1, int y1, SSFMatrix<int> &img, SSFMatrix<FeatType> &m);

	int GetNFeatures();
};

#endif