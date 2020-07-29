#include "headers.h"
#include "ExtractionMethod.h"
#include "FeatureMethods.h"
#include "IntensityFeature.h"
#include "LBPFeature.h"
#include "HistogramFeature.h"
#include "PredominanceFilter.h"
#include "HOG.h"


FeatureMethods::FeatureMethods() {
IntensityFeature *intensity;	
LBPFeature *lbp;
HistogramFeature *histogram;
PredominanceFilter *predominance;
HOG *hog;


	/* intensity feature */
	intensity = new IntensityFeature();
	methods.insert(pair<string, ExtractionMethod *>(intensity->GetID(), intensity));

	/* LBP feature */
	lbp = new LBPFeature();
	methods.insert(pair<string, ExtractionMethod *>(lbp->GetID(), lbp));

	/* Color Histogram feature */
	histogram = new HistogramFeature();
	methods.insert(pair<string, ExtractionMethod *>(histogram->GetID(), histogram));

	/* Predominance feature */
	predominance = new PredominanceFilter();
	methods.insert(pair<string, ExtractionMethod *>(predominance->GetID(), predominance));

	/* HOG feature*/
	hog = new HOG();
	methods.insert(pair<string, ExtractionMethod *>(hog->GetID(), hog));
}


ExtractionMethod *FeatureMethods::InstanciateMethod(string featName) {
unordered_map<string, ExtractionMethod *>::iterator it;

	it = methods.find(featName);

	if (it == methods.end()) 
		ReportError("Feature extraction method '%s' does not exist", featName.c_str());

	return it->second->New();
}
