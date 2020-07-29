#ifndef _SSF_CORE_H_
#define _SSF_CORE_H_


#define SSF_METHOD_TYPE_FEATURE "SSFMethodType_Feature"
#define SSF_METHOD_TYPE_THREAD_MISC "SSFMethodType_ThreadMisc"
#define SSF_METHOD_TYPE_STRUCTURE "SSFMethodType_Structure"
#define SSF_METHOD_TYPE_INTMOD "SSFMethodType_InternalMod"
#define SSF_METHOD_TYPE_USERMOD "SSFMethodType_UserModule"

#define SSF_METHOD_INTERNALMOD_FES "SSFMethod_InternalMod_FES"
#define SSF_STRUCTURE_SM "SSFStructure_SM"
#define SSF_STRUCTURE_EXECCONTROL "SSFStructure_ExecControl"
#define SSF_STRUCTURE_SMManager "SSFStructure_SMManager"
#define SSF_METHOD_FES_MANAGER "FESMethod_Manager"
#define SSF_METHOD_FES_INSTANCE_CPU2D "FESMethod_InstanceCPU2D"
#define SSF_METHOD_FES_FEATURE_HOG "HOG" 
#define SSF_METHOD_FES_FEATURE_COOC "COOC" 
#define SSF_METHOD_FES_FEATURE_INTENSITY "Intensity" 


#define SSF_USERMOD_CLASS_Mod_Filtering "Filtering"
#define SSF_USERMOD_CLASS_Mod_Detection "Detection"
#define SSF_USERMOD_CLASS_Mod_IO "IO"
#define SSF_USERMOD_CLASS_Mod_Auxiliar "Auxiliar"
#define SSF_USERMOD_CLASS_Mod_BGS "BGS"
#define SSF_USERMOD_CLASS_Mod_Camera "Camera"
#define SSF_USERMOD_CLASS_Mod_Filtering "Filtering"
#define SSF_USERMOD_CLASS_Mod_Recognition "Recognition"
#define SSF_USERMOD_CLASS_Mod_Reidentification "Reidentification"

// data structures
#include "SMDataKey.h" 
#include "FeatBlockInfo.h"

// input parameters
#include "SSFParameters.h"

// information regarding a structure
#include "SSFInfo.h"

// thread launch
#include "SSFThreadsControl.h"

// base structure for methods in the SSF
#include "SSFStructure.h" 

// methods for modules, features and classifiers
#include "SSFMethod.h"
#include "SSFMethodThread.h" 
#include "SSFMethodNonThread.h" 
#include "SSFUserModule.h" 
#include "SSFInternalModule.h" 

// control for instantiated methods
#include "SSFMethodControl.h"

// execution control
#include "SSFExecutionControl.h"




extern SSFExecutionControl glb_SSFExecCtrl;

#endif