#include "headers.h"
#include "SSFCore.h"
#include "FES.h"
#include "FESTemplateMethod.h"


/***
 * class SSFMethodFeatureExtraction
 ***/
SSFMethodFeatureExtraction::SSFMethodFeatureExtraction(string instanceID) : SSFMethodNonThread (instanceID) {

}



/***
 * class FeatureExtractionGPU
 ***/
FeatureExtractionGPU::FeatureExtractionGPU(string instanceID) : SSFMethodFeatureExtraction(instanceID) {

}


/***
 * class FeatureExtractionCPU
 ***/
FeatureExtractionCPU::FeatureExtractionCPU(string instanceID) : SSFMethodFeatureExtraction(instanceID) {

}



/***
 * class FeatureExtractionCPU2D
 ***/
FeatureExtractionCPU2D::FeatureExtractionCPU2D(string instanceID) :FeatureExtractionCPU (instanceID) {
	
}