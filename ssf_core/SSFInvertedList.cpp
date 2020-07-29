#include "headers.h"
#include "SSFInvertedList.h"

/***************************
 * class SSFInvertedList *
 ***************************/

SSFInvertedList::SSFInvertedList() {

	this->SetDataType(SSF_DATA_INVERTEDLIST);
}

SSFInvertedList::~SSFInvertedList() {

	for(int i=0; i<list.size(); i++)
		delete list[i];
	if(dict != NULL)
		delete dict;
}

void SSFInvertedList::CreatList(SSFMatrix<FeatType> feat, vector<string> featID, int ncws, SSFDict *Dict) {

	double dist;

	dict = Dict;

	nList = ncws;
	vector<data> *v;

	for(int i=0; i<nList; i++)
	{
		v = new vector<data>;
		list.push_back(v);
	}

	int featIDsize = (int)featID.size();

	if(feat.rows != featIDsize)
		ReportError("Feature e featureId have different size");

	for(int i=0; i<featIDsize; i++)
	{
		int index = dict->ComputeDistance(feat.row(i), dist);
		SetElement(featID[i], index, dist);
	}
}

void SSFInvertedList::SetElement(string featID, int index, double dist){   

	data Data;
	Data.ID = featID;
	Data.Dist = dist;
	list[index]->push_back(Data);
}

vector<data> *SSFInvertedList::GetListPos(int pos){

	return list[pos];
}

int SSFInvertedList::GetNList(){
	return nList;
}

vector<data> *SSFInvertedList::GetTestFeat(SSFMatrix<FeatType> feat){

	int index = dict->ComputeDistance(feat);
	return list[index];
}

void SSFInvertedList::Sort(){

	vector<data >::iterator it;
	map<double, string> mapping;
	vector<data> *v;
	vector<vector<data> *> NewList;
	map<double, string>::iterator itmap;
	data aux;
	double dist = 0, val;

	for(int i=0; i<nList; i++)
	{
		v = new vector<data>;
		NewList.push_back(v);
	}

	for(int i=0; i<list.size(); i++) {

		for (it = list[i]->begin() ; it != list[i]->end(); it++) {

			aux = *it;
			
			dist = dist + aux.Dist;
		}

		for (it = list[i]->begin() ; it != list[i]->end(); it++) {

			aux = *it;

			val = aux.Dist / dist;

			mapping.insert(pair<double, string>(val, aux.ID));
		}

		for (itmap = mapping.begin(); itmap != mapping.end(); itmap++) {

			aux.Dist = itmap->first;
			aux.ID = itmap->second;
			
			NewList[i]->push_back(aux);
		}
		mapping.clear();
	}

	for(int i=0; i<list.size(); i++)
		delete list[i];

	list = NewList;
}

void SSFInvertedList::Load(const FileNode &node){
	
	int nId;
	vector<data> *v;
	FileNode n, n2, n3, n4;
	
	n = node["Inverted_List"];
	n["nList"] >> nList;

	for(int i=0; i<nList; i++)
	{
		v = new vector<data>;
		list.push_back(v);
	}

	for(int i=0; i<nList; i++)
	{
		n2 = n["List" + IntToString(i)];
		n2["nId"] >> nId;
		n3 = n2["Data"];
		FileNodeIterator it = n3.begin(), it_end = n3.end();

		for( ; it != it_end; ++it)
		{
			data Data;
			Data.ID = (string)(*it)["id"];
			Data.Dist = (double)(*it)["dist"];
			
			list[i]->push_back(Data);
		}
	}
	dict = new SSFDict();
	dict->Load(node);
}

void SSFInvertedList::Save(SSFStorage &storage) {

	int idx = 0;
	vector<data>::iterator it;

	if (storage.isOpened() == false)
		ReportError("Invalid file storage!");

	storage << "Inverted_List" << "{";
	storage << "nList" << (int) list.size();

	// print Inverted List for each list
	for(int i=0; i<list.size(); i++)
	{
		// new level
		storage << "List" + IntToString(idx++) << "{";
		storage << "nId" << (int) list[i]->size();
		storage << "Data" << "[";
		for (it = list[i]->begin() ; it != list[i]->end(); it++) 
		{
			data p = *it;
			storage << "{:" << "id" << p.ID << "dist" << p.Dist << "}";
		}
		// return the level
		storage <<"]" << "}";
	}
	storage << "}";
	// save dictionary
	dict->Save(storage);
}

void SSFInvertedList::Load(string filename){
	
	SSFStorage storage;

	storage.open(filename, SSF_STORAGE_READ);
	this->Load(storage.root());
	storage.release();
}

void SSFInvertedList::Save(string filename) {
SSFStorage storage;

	storage.open(filename, SSF_STORAGE_WRITE);
	this->Save(storage);
	storage.release();
}