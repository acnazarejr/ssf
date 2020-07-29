#ifndef _SSF_LAP_DATA_H_
#define _SSF_LAP_DATA_H_
#include "SSFThreads.h" 
#include "SSFStorage.h"
#include "SSFData.h"

// Detwin structure
class SSFLAPData : public SSFData {
friend class LAP;
SMIndexType frameID;		// frame ID

protected:

	void SetFrameID(SMIndexType frameID);

public:

	SSFLAPData();

	SMIndexType GetFrameID();
};


#endif