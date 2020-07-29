#include "headers.h"
#include "co_occurrence_general.h"

bool custom_isnan(double var)
{
    volatile double d = var;
    return d != d;
}

void co_occurrence_general::coocanglexNewNewNoFeatSelection(SSFMatrix<FeatType> &m, int tones, SSFMatrix<FeatType> &mout) {

	mout(0,0) = f1_asm (m, tones);
	if (custom_isnan(mout(0,0)) == true)
		mout(0,0) = 0.0f;

	mout(0,1) = f2_contrast(m, tones);
	if (custom_isnan(mout(0,1)) == true)
		mout(0,1) = 0.0f;

	mout(0,2) = f3_corr(m, tones);
	if (custom_isnan(mout(0,2)) == true)
		mout(0,2) = 0.0f;

	mout(0,3) = f4_var(m, tones);
	if (custom_isnan(mout(0,3)) == true)
		mout(0,3) = 0.0f;

	mout(0,4) = f5_idm(m, tones);
	if (custom_isnan(mout(0,4)) == true)
		mout(0,4) = 0.0f;

	mout(0,5) = f6_savg(m, tones);
	if (custom_isnan(mout(0,5)) == true)
		mout(0,5) = 0.0f;

	mout(0,6) = f7_svar(m, tones, mout(0,5));
	if (custom_isnan(mout(0,6)) == true)
		mout(0,6) = 0.0f;

	mout(0,7) = f8_sentropy(m, tones);
	if (custom_isnan(mout(0,7)) == true)
		mout(0,7) = 0.0f;

	mout(0,8) = f9_entropy(m, tones);
	if (custom_isnan(mout(0,8)) == true)
		mout(0,8) = 0.0f;

	mout(0,9) = f10_dvar(m, tones);	
	if (custom_isnan(mout(0,9)) == true)
		mout(0,9) = 0.0f;

	mout(0,10) = f11_dentropy(m, tones);
	if (custom_isnan(mout(0,10)) == true)
		mout(0,10) = 0.0f;

	mout(0,11) = f15_directionality(m, tones);
	if (custom_isnan(mout(0,11)) == true)
		mout(0,11) = 0.0f;
}





float co_occurrence_general::f1_asm (float **P, int Ng) {
  int i, j;
  float sum = 0;

  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      sum += P[i][j] * P[i][j];

  return sum;

  /*
   * The angular second-moment feature (ASM) f1 is a measure of homogeneity
   * of the image. In a homogeneous image, there are very few dominant
   * gray-tone transitions. Hence the P matrix for such an image will have
   * fewer entries of large magnitude.
   */
}



float co_occurrence_general::f1_asm (SSFMatrix<FeatType> &P, int Ng) {
  int i, j;
  float sum = 0;

  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      sum += P(i,j) * P(i,j);

  return sum;

  /*
   * The angular second-moment feature (ASM) f1 is a measure of homogeneity
   * of the image. In a homogeneous image, there are very few dominant
   * gray-tone transitions. Hence the P matrix for such an image will have
   * fewer entries of large magnitude.
   */
}



float co_occurrence_general::f2_contrast (float **P, int Ng)

/* Contrast */
{
  int i, j, n;
  float sum = 0, bigsum = 0;

  for (n = 0; n < Ng; ++n)
  {
    for (i = 0; i < Ng; ++i)
      for (j = 0; j < Ng; ++j)
	if ((i - j) == n || (j - i) == n)
	  sum += P[i][j];
    bigsum += n * n * sum;

    sum = 0;
  }
  return bigsum;

  /*
   * The contrast feature is a difference moment of the P matrix and is a
   * measure of the contrast or the amount of local variations present in an
   * image.
   */
}


float co_occurrence_general::f2_contrast (SSFMatrix<FeatType> &P, int Ng)

/* Contrast */
{
  int i, j, n;
  float sum = 0, bigsum = 0;

  for (n = 0; n < Ng; ++n)
  {
    for (i = 0; i < Ng; ++i)
      for (j = 0; j < Ng; ++j)
	if ((i - j) == n || (j - i) == n)
	  sum += P(i,j);
    bigsum += n * n * sum;

    sum = 0;
  }
  return bigsum;

  /*
   * The contrast feature is a difference moment of the P matrix and is a
   * measure of the contrast or the amount of local variations present in an
   * image.
   */
}





float co_occurrence_general::f3_corr (float **P, int Ng)


/* Correlation */
{
  int i, j;
  float sum_sqrx = 0, sum_sqry = 0, tmp, *px;
  float meanx =0 , meany = 0 , stddevx, stddevy;

  px = cvector (0, Ng);
  for (i = 0; i < Ng; ++i)
    px[i] = 0;

  /*
   * px[i] is the (i-1)th entry in the marginal probability matrix obtained
   * by summing the rows of p[i][j]
   */
  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      px[i] += P[i][j];


  /* Now calculate the means and standard deviations of px and py */
  /*- fix supplied by J. Michael Christensen, 21 Jun 1991 */
  /*- further modified by James Darrell McCauley, 16 Aug 1991 
   *     after realizing that meanx=meany and stddevx=stddevy
   */
  for (i = 0; i < Ng; ++i)
  {
    meanx += px[i]*i;
    sum_sqrx += px[i]*i*i;
  }
  meany = meanx;
  sum_sqry = sum_sqrx;
  stddevx = sqrt (sum_sqrx - (meanx * meanx));
  stddevx = stddevx + (float)0.0001;
  stddevy = stddevx;

  /* Finally, the correlation ... */
  for (tmp = 0, i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      tmp += i*j*P[i][j];

  free(px);
	
  return (tmp - meanx * meany) / (stddevx * stddevy);
  /*
   * This correlation feature is a measure of gray-tone linear-dependencies
   * in the image.
   */
}




float co_occurrence_general::f3_corr (SSFMatrix<FeatType> &P, int Ng)


/* Correlation */
{
  int i, j;
  float sum_sqrx = 0, sum_sqry = 0, tmp, *px;
  float meanx =0 , meany = 0 , stddevx, stddevy;

  px = cvector (0, Ng);
  for (i = 0; i < Ng; ++i)
    px[i] = 0;

  /*
   * px[i] is the (i-1)th entry in the marginal probability matrix obtained
   * by summing the rows of p[i][j]
   */
  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      px[i] += P(i,j);


  /* Now calculate the means and standard deviations of px and py */
  /*- fix supplied by J. Michael Christensen, 21 Jun 1991 */
  /*- further modified by James Darrell McCauley, 16 Aug 1991 
   *     after realizing that meanx=meany and stddevx=stddevy
   */
  for (i = 0; i < Ng; ++i)
  {
    meanx += px[i]*i;
    sum_sqrx += px[i]*i*i;
  }
  meany = meanx;
  sum_sqry = sum_sqrx;
  stddevx = sqrt (sum_sqrx - (meanx * meanx));
  stddevx = stddevx + (float)0.0001;
  stddevy = stddevx;

  /* Finally, the correlation ... */
  for (tmp = 0, i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      tmp += i*j*P(i,j);

  free(px);
	
  return (tmp - meanx * meany) / (stddevx * stddevy);
  /*
   * This correlation feature is a measure of gray-tone linear-dependencies
   * in the image.
   */
}






float co_occurrence_general::f4_var (float **P, int Ng)

/* Sum of Squares: Variance */
{
  int i, j;
  float mean = 0, var = 0;

  /*- Corrected by James Darrell McCauley, 16 Aug 1991
   *  calculates the mean intensity level instead of the mean of
   *  cooccurrence matrix elements 
   */
  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      mean += i * P[i][j];

  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      var += (i + 1 - mean) * (i + 1 - mean) * P[i][j];

  return var;
}





float co_occurrence_general::f4_var (SSFMatrix<FeatType> &P, int Ng)

/* Sum of Squares: Variance */
{
  int i, j;
  float mean = 0, var = 0;

  /*- Corrected by James Darrell McCauley, 16 Aug 1991
   *  calculates the mean intensity level instead of the mean of
   *  cooccurrence matrix elements 
   */
  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      mean += i * P(i,j);

  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      var += (i + 1 - mean) * (i + 1 - mean) * P(i,j);

  return var;
}








float co_occurrence_general::f5_idm (float **P, int Ng)


/* Inverse Difference Moment */
{
  int i, j;
  float idm = 0;

  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      idm += P[i][j] / (1 + (i - j) * (i - j));

  return idm;
}



float co_occurrence_general::f5_idm (SSFMatrix<FeatType> &P, int Ng)
/* Inverse Difference Moment */
{
  int i, j;
  float idm = 0;

  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      idm += P(i,j) / (1 + (i - j) * (i - j));

  return idm;
}




float co_occurrence_general::f6_savg (float **P, int Ng)

/* Sum Average */
{
  int i, j;
  float savg = 0;

  for (i = 0; i <= 2 * Ng; i++)
    Pxpy[i] = 0;


  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j) 
      Pxpy[i + j + 2] += P[i][j];
  

  for (i = 2; i <= 2 * Ng; ++i) {
    savg += i * Pxpy[i]; 
  }

  return savg;
}




float co_occurrence_general::f6_savg (SSFMatrix<FeatType> &P, int Ng)

/* Sum Average */
{
  int i, j;
  float savg = 0;

  for (i = 0; i <= 2 * Ng; i++)
    Pxpy[i] = 0;


  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j) 
      Pxpy[i + j + 2] += P(i,j);
  

  for (i = 2; i <= 2 * Ng; ++i) {
    savg += i * Pxpy[i]; 
  }

  return savg;
}




float co_occurrence_general::f7_svar (float **P, int Ng, float S)


/* Sum Variance */
{
  int i, j;
  float var = 0;

  for (i = 0; i <= 2 * Ng; i++)
    Pxpy[i] = 0;

  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      Pxpy[i + j + 2] += P[i][j];

  for (i = 2; i <= 2 * Ng; ++i)
    var += (i - S) * (i - S) * Pxpy[i];

  return var;
}



float co_occurrence_general::f7_svar (SSFMatrix<FeatType> &P, int Ng, float S)


/* Sum Variance */
{
  int i, j;
  float var = 0;

  for (i = 0; i <= 2 * Ng; i++)
    Pxpy[i] = 0;

  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      Pxpy[i + j + 2] += P(i,j);

  for (i = 2; i <= 2 * Ng; ++i)
    var += (i - S) * (i - S) * Pxpy[i];

  return var;
}





float co_occurrence_general::f8_sentropy (float **P, int Ng)


/* Sum Entropy */
{
  int i, j;
  float sentropy = 0;

  for (i = 0; i <= 2 * Ng; i++)
    Pxpy[i] = 0;

  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      Pxpy[i + j + 2] += P[i][j];

  for (i = 2; i <= 2 * Ng; ++i)
    sentropy -= Pxpy[i] * log10 (Pxpy[i] + (float)0.00001);

  return sentropy;
}





float co_occurrence_general::f8_sentropy (SSFMatrix<FeatType> &P, int Ng)


/* Sum Entropy */
{
  int i, j;
  float sentropy = 0;

  for (i = 0; i <= 2 * Ng; i++)
    Pxpy[i] = 0;

  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      Pxpy[i + j + 2] += P(i,j);

  for (i = 2; i <= 2 * Ng; ++i)
    sentropy -= Pxpy[i] * log10 (Pxpy[i] + (float)0.00001);

  return sentropy;
}






float co_occurrence_general::f9_entropy (float **P, int Ng)
/* Entropy */
{
  int i, j;
  float entropy = 0;

  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      entropy += P[i][j] * log10 (P[i][j] + (float)0.000001);

  return -entropy;
}



float co_occurrence_general::f9_entropy (SSFMatrix<FeatType> &P, int Ng)
/* Entropy */
{
  int i, j;
  float entropy = 0;

  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      entropy += P(i,j) * log10 (P(i,j) + (float)0.000001);

  return -entropy;
}




float co_occurrence_general::f10_dvar (float **P, int Ng)
/* Difference Variance */
{
  int i, j, tmp;
  float sum = 0, sum_sqr = 0, var = 0;

  for (i = 0; i < 2 * Ng; i++)
    Pxpy[i] = 0;

  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      Pxpy[abs (i - j)] += P[i][j];

  /* Now calculate the variance of Pxpy (Px-y) */
  for (i = 0; i < Ng; ++i)
  {
    sum += Pxpy[i];
    sum_sqr += Pxpy[i] * Pxpy[i];
  }
  tmp = Ng * Ng;
  var = ((tmp * sum_sqr) - (sum * sum)) / (tmp * tmp);

  return var;
}




float co_occurrence_general::f10_dvar (SSFMatrix<FeatType> &P, int Ng)
/* Difference Variance */
{
  int i, j, tmp;
  float sum = 0, sum_sqr = 0, var = 0;

  for (i = 0; i < 2 * Ng; i++)
    Pxpy[i] = 0;

  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      Pxpy[abs (i - j)] += P(i,j);

  /* Now calculate the variance of Pxpy (Px-y) */
  for (i = 0; i < Ng; ++i)
  {
    sum += Pxpy[i];
    sum_sqr += Pxpy[i] * Pxpy[i];
  }
  tmp = Ng * Ng;
  var = ((tmp * sum_sqr) - (sum * sum)) / (tmp * tmp);

  return var;
}




float co_occurrence_general::f11_dentropy (float **P, int Ng)

/* Difference Entropy */
{
  int i, j;
  float sum = 0;

  for (i = 0; i < 2 * Ng; i++)
    Pxpy[i] = 0;

  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      Pxpy[abs (i - j)] += P[i][j];

  for (i = 0; i < Ng; ++i)
    sum += Pxpy[i] * log10 (Pxpy[i] + (float)0.00001);

  return -sum;
}


float co_occurrence_general::f11_dentropy (SSFMatrix<FeatType> &P, int Ng)

/* Difference Entropy */
{
  int i, j;
  float sum = 0;

  for (i = 0; i < 2 * Ng; i++)
    Pxpy[i] = 0;

  for (i = 0; i < Ng; ++i)
    for (j = 0; j < Ng; ++j)
      Pxpy[abs (i - j)] += P(i,j);

  for (i = 0; i < Ng; ++i)
    sum += Pxpy[i] * log10 (Pxpy[i] + (float)0.00001);

  return -sum;
}





float co_occurrence_general::f15_directionality (float **P, int Ng) {
int i;
float sum = 0;


  for (i = 0; i < Ng; ++i) {
      sum += P[i][i];
  }

  return sum;
}



float co_occurrence_general::f15_directionality (SSFMatrix<FeatType> &P, int Ng) {
int i;
float sum = 0;


  for (i = 0; i < Ng; ++i) {
      sum += P(i,i);
  }

  return sum;
}




float *co_occurrence_general::cvector (int nl, int nh) {
  float *v;

  v = (float *) malloc ((unsigned) (nh - nl + 1) * sizeof (float));
  if (!v)
    ReportError("Memory allocation failure\n");

  return v - nl;
}





co_occurrence_general::co_occurrence_general() {

}



co_occurrence_general::~co_occurrence_general() {

}




void co_occurrence_general::Initialize(int nbins, int distance) {
	
	this->nbins = nbins;
	this->distance = distance;

	M0.create(this->nbins, this->nbins);
	M45.create(this->nbins, this->nbins);
	M90.create(this->nbins, this->nbins);
	M135.create(this->nbins, this->nbins);
}
	


int co_occurrence_general::GetNFeatures() {

	return 4 * FEATURESCG;
}


// compute features without considering feature selection
void  co_occurrence_general::ExtractFeatures(int x0, int y0, int x1, int y1, SSFMatrix<int> &img, SSFMatrix<FeatType> &m) {
float R0, R45, R90, R135;
int d, x, y;
int rows, cols, row, col;
int xis, yp;
int tones;
int idxfeat;
SSFMatrix<FeatType> mTmp;

    /* Inicializacoes */
	cols = x1;
	rows = y1;
	d = this->distance;
	
    /* Determine the number of different gray scales (not maxval) */
	tones = this->nbins;
	yp = 0; xis = 0;

	/* Zera as matrizes */
	M0.zeros(this->nbins, this->nbins);
	M90.zeros(this->nbins, this->nbins);
	M45.zeros(this->nbins, this->nbins);
	M135.zeros(this->nbins, this->nbins);

	R0 = R45 = R135 = R90 = 0.0f;

	for (row = y0; row <= y1; row++){
		for (col = x0; col <= x1; col++){

			/* Calcula as matrizes */
			x = img(row,col);
	              
            if (col + d <= cols){
				y = img(row, col + d);
				M0(y,x)++;
				M0(x,y)++;
				R0++;
            }

            if (row + d <= rows){
				y = img(row + d, col);
				M90(y,x)++;
				M90(x,y)++;
				R90++;
            }

            if (row + d <= rows && col - d >= x0){
				y = img(row + d, col - d);
				//y = direct->GetElement(col - d, row + d);
				M45(y,x)++;
				M45(x,y)++;
				R45++;
            }

            if (row + d <= rows && col + d <= cols){
				//y = direct->GetElement(col + d, row + d);
				y = img(row + d, col + d);
				M135(y,x)++;
				M135(x,y)++;
				R135++;
            }
		}
	}

	/* Normalize gray-tone spatial dependence matrix */
	// Normalize
	M0 = M0/R0;
	M45 = M45/R45;
	M90 = M90/R90;
	M135 = M135/R135;

	idxfeat = 0;

	mTmp = m.colRange(0, FEATURESCG);
	coocanglexNewNewNoFeatSelection(M0, tones, mTmp);
	idxfeat += FEATURESCG;

	mTmp = m.colRange(idxfeat, idxfeat + FEATURESCG);
	coocanglexNewNewNoFeatSelection(M45, tones, mTmp);
	idxfeat += FEATURESCG;

	mTmp = m.colRange(idxfeat, idxfeat + FEATURESCG);
	coocanglexNewNewNoFeatSelection(M90, tones, mTmp);
	idxfeat += FEATURESCG;

	mTmp = m.colRange(idxfeat, idxfeat + FEATURESCG);
	coocanglexNewNewNoFeatSelection(M135, tones, mTmp);
}