#ifndef _BowReid_H_
#define _BowReid_H_
#if 0
#include "headers.h"
#include "module.h"
#include "auxiliardefs.h"
#include "SSFFeatures.h"
#include "SSFDict.h"
#include "SSFInvertedList.h"
#include "LAP.h"
#endif

class ModBowReid : public SSFUserModule {
vector<vector<string> *>SubjectList;
vector<SSFObjectCandidate> candidates;
vector<SMIndexType> tracklets;
map<string, double> mapping;
map<string, int> map_aux;
multimap<double, string> multimap;
SSFObjectCandidate candidate;
SSFInvertedList *list;
FeatureIndex featIdxMap;
SSFStorage storage1;
SSFDict *Dict;
FeatureExtControl featExt;
vector<SSFMatrix<FeatType>> vector_feat;
SSFFeatures feat_bigmatrix;
SSFMatrix<FeatType> feat_aux;
vector<double> WeightBlock;
vector<string> object;
string executionType;
string learnInputMod;
string testInputMod;
string featFile;
string featSetup;
string normalization;
string FeatureFile;
string ResultFile;
string modelFile;
int setupDict;
int setupBlock;
int numberDict;
int numberBlock;
int nBlocks;
int nCWs;
int nObject;
int width, height;
int output;
int featIdx;
int ntracklets;

	void Learning();

	void LearningSingleDictionary(int nobject);

	void LearningBlockDictionary(int nobject);

	void LearningMultipleDictionary(int nobject);

	void Testing();

	void TestingSingleDictionary(int idx, const FileNode &n, SSFMatrix<FeatType> BMmean, SSFMatrix<FeatType> BMstd, int block);

	void TestingBlockDictionary(int idx, const FileNode &n, SSFMatrix<FeatType> BMmean, SSFMatrix<FeatType> BMstd, int block);

	void Mapping(vector<data > *result, int block);

	void WriteCandidates(int idx);

	void PrintInvertedList();

	float Distance(CvPoint pt1, CvPoint pt2);
	
public:
	ModBowReid(string modID);
	~ModBowReid();

	// return name of the module
	string GetName() { return "ModBowReid"; }

	// Return the type of the module  
	string GetClass() { return SSF_USERMOD_CLASS_Mod_Reidentification; }

	// retrieve structure version
	string GetVersion() { return "0.0.1"; }

	// function to generate a new instatiation of the detector
	SSFMethod *Instantiate(string modID);

	// function to check if the parameters are set correctly
	void Setup();

	// execute
	void Execute();
};




#endif