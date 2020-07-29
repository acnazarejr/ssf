// SSF.cpp : Defines the entry point for the console application.
//
#include "headers.h"
#include "SSFCore.h"
#include "structures.h"
#include "shared_memory.h"
#include "LAP.h"
#include "ExtractionControl.h"
#include "SSFGarbageCollector.h"
#include "SSFTest.h"
#include "SSFParameters.h"


// global variables
LAP glb_sharedMem;
FeatureExtControl glb_featExtControl;
SSFGarbageCollector glb_SSFGarbageCollector;
std::default_random_engine glb_RandonGenerator;
SSFExecutionControl glb_SSFExecCtrl;
TimeProfile glb_TP;


int main(int argc, char* argv[]) {


	/* initialization of the random number generator */
	InitializeRandomNumberGenerator();	

	glb_TP.Startx();

	/* inicialize execution */
	glb_SSFExecCtrl.Initialize("params_0.0.3_Display.yml", "control");
	//glb_SSFExecCtrl.Initialize("params_0.0.3_DetTesting.yml", "control");
	//glb_SSFExecCtrl.Initialize("params_0.0.3_MultipleOutputs.yml", "control");

	// save used parameters
	glb_SSFExecCtrl.SaveMethodsParameters("allParameters.yml");

	// wait until modules to end
	glb_SSFExecCtrl.WaitModulesToEnd();
	
	glb_TP.Stopx();

	glb_TP.Printx();

	return 0;
}