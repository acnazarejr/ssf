#ifndef _SSF_OBJECT_H_
#define _SSF_OBJECT_H_
#include "SSFLAPData.h"



// class to store object information
class SSFObject : public SSFLAPData  {
friend class LAP;
SSFMutex mutex;
string objID;				// object identifier
vector<SMIndexType> tracksID;	// list of the tracklets for this object

	// set tracklet ID to this object
	void AddTrackletID(SMIndexType trackID);

	// set object ID
	void SetObjID(string objID);

	// save the object in the storage
	void Save(SSFStorage &storage);

	// load the object in the storage
	void Load(const FileNode &node);

public:
	SSFObject();

	// retrieve the track IDs inside this object
	vector<SMIndexType> GetTrackletIDs();

	// get object ID
	string GetObjID();

	// get index
	SMIndexType GetIndex();
};




#endif