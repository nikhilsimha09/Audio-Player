/*
 * CSoundCollectionDB.h
 */

#ifndef SRC_CSOUNDCOLLECTIONDB_H_
#define SRC_CSOUNDCOLLECTIONDB_H_
#include <string>
using namespace std;
#include "CDatabase.h"

class CSoundCollectionDB: public CDatabase
{
private:
	char m_path[DB_TEXTLEN];
	char m_name[DB_TEXTLEN];
	int m_sid;
	int m_fs;
	int m_numChan;

public:
	CSoundCollectionDB();
	bool selectAllSounds();
	int selectNumSounds();
	int selectFs(int soundID);
	bool selectSoundData(int soundID); // Lab04 Task 1
	bool insertSound(string path,string name,int fs,int numChan);
	string getPath();
	string getName();
	int getID();
	int getFs();
	int getNumChan();
};

#endif /* SRC_CSOUNDCOLLECTIONDB_H_ */
