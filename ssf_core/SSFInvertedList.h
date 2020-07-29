#ifndef SSF_INVERTED_LIST_H
#define SSF_INVERTED_LIST_H
#include "headers.h"
#include "SSFDict.h"

struct data {
 string ID;
 double Dist;
};

class SSFInvertedList : public SSFData{

protected:

vector<vector<data> *> list;			// inverted list
SSFDict* dict;							// dictionary
int nList;								// number codeword or number of indexes inverted list

	// Insert element in the inverted list
	void SetElement(string featID, int index, double dist);

public:

	// constructor and destrutor
	SSFInvertedList();
	~SSFInvertedList();

	// Return list of subject given a position in the list
	vector<data> *GetListPos(int pos);

	void CreatList(SSFMatrix<FeatType> feat, vector<string> featID, int ncws, SSFDict *Dict);

	// Return number of indexes inverted list
	int GetNList();

	// Return list of subject given a feature
	vector<data > *GetTestFeat(SSFMatrix<FeatType> feat);

	// Sort the inverted list
	void Sort();
	
	// Load Inverted List
	void Load(string filename);
	void Load(const FileNode &node);

	// Save Inverted List
	void Save(string filename);
	void Save(SSFStorage &storage);

};


#endif




	

