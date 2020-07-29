#ifndef _SSFDETWINS_H_
#define _SSFDETWINS_H_
#include "DetWin.h"


class SSFDetwins {
SSFMutex mutex;
unordered_set<size_t> selectedDetwin;
vector<DetWin> *detwins;

public:
	SSFDetwins();	
	SSFDetwins(vector<DetWin> *detwins);
	~SSFDetwins();

	// set all detection windows, delete previous
	void SetDetwins(vector<DetWin> *detwins);

	// Retrieve all detection windows.
	vector<DetWin> *GetDetwins();

	//  Get the number of detection windows.
	size_t GetNumberofDetWins();

	// Select detection window according to its index.
	void SelectDetwin(size_t idx);

	// Set response for a detection window
	void SetResponse(size_t idx, float response);

	// Select a number n of detection windows randomly, with uniform distribution (if n >= number det wins, does not do anything).
	void SelectDetwinRandomly(size_t n);

	//  Retrieve only the selected detection windows
	vector<DetWin> *RetrieveSelected();

	// Add a detection window
	void AddDetWindow(DetWin detwin);
};


#endif