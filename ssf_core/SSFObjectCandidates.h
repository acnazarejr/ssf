#ifndef _SSF_OBJECTCANDIDATES_H_
#define _SSF_OBJECTCANDIDATES_H_
#include "SSFLAPData.h"



// class to store sample information
struct SSFObjectCandidate{
	SMIndexType objCandidate;
	FeatType confidence;
	bool operator<(SSFObjectCandidate &a){ return confidence > confidence; }
};

class SSFObjectCandidates : public SSFLAPData  {
friend class LAP;
SSFMutex mutex;
SMIndexType trackletID;
vector<SSFObjectCandidate> objCandidates;

	// save this sample
	void Save(SSFStorage &storage) { ; }

	// load a sample
	void Load(const FileNode &node) { ; }

public:
	SSFObjectCandidates();
	SSFObjectCandidates( SMIndexType trackletID );
	SSFObjectCandidates( SMIndexType trackletID, vector<SSFObjectCandidate> &objectCandidates );
	~SSFObjectCandidates();

	SMIndexType GetTrackletID();

	const vector<SSFObjectCandidate> *GetObjectCandidates();
};

#endif