#ifndef _STORAGE_H_
#define _STORAGE_H_


#define SSF_STORAGE_WRITE FileStorage::WRITE
#define SSF_STORAGE_READ FileStorage::READ


class SSFStorage : public FileStorage {
//FileStorage fs;			// openCV struct for storage

public:
	//SSFStorage();

	// release and save what is pending
	//void Release();

	// Load storage	//void Load(const string& filename, int flags);


	//void WriteMatrix(Mat &m, string name);
};


#endif