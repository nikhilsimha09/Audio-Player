/*
 * CSoundCollectionDB.cpp
 */

#include <iostream>
#include "CSoundCollectionDB.h"


CSoundCollectionDB::CSoundCollectionDB() {
	m_path[0]=0;
	m_name[0]=0;
	m_sid=0;
	m_fs=0;
	m_numChan=0;
}

bool CSoundCollectionDB::selectAllSounds() {
	string sSQL="select * from sounds;";
	if (true == _executeSQLStmt(sSQL, "selectAllSounds", true))
	{
		SQLBindCol( m_hStmt, 1, SQL_C_LONG, &m_sid, 0, &m_info );
		SQLBindCol( m_hStmt, 2, SQL_C_CHAR, &m_path, DB_TEXTLEN, &m_info );
		SQLBindCol( m_hStmt, 3, SQL_C_CHAR, &m_name, DB_TEXTLEN, &m_info );
		SQLBindCol( m_hStmt, 4, SQL_C_LONG, &m_fs, 0, &m_info );
		SQLBindCol( m_hStmt, 5, SQL_C_LONG, &m_numChan, 0, &m_info );
		return true;
	}
	else
	{
		return false;
	}
}

int CSoundCollectionDB::selectNumSounds() {
	int num;
	string sSQL="select count(*) from sounds;";
	if (true == _executeSQLStmt(sSQL, "selectNumSounds", true))
	{
		SQLBindCol( m_hStmt, 1, SQL_C_LONG, &num, 0, &m_info );

		bool bfetch=fetch(); // get the number of available sounds
		closeQuery();
		return bfetch ? num : -1;
	}
	return -1;
}

int CSoundCollectionDB::selectFs(int soundID) {
	int fs;
	string sSQL="select fs from sounds where soundfileid=" + to_string(soundID)+";";
	if (true == _executeSQLStmt(sSQL, "selectFs", true))
	{
		SQLBindCol( m_hStmt, 1, SQL_C_LONG, &fs, 0, &m_info );

		bool bfetch=fetch(); // get fs
		closeQuery();
		return bfetch ? fs : -1;
	}
	return -1;
}

bool CSoundCollectionDB::selectSoundData(int soundID)
{
	string sSQL;
	sSQL= "SELECT * FROM sounds WHERE soundfileid=" + to_string(soundID);
	if( true == _executeSQLStmt(sSQL, "selectSoundData data for a given sound id", true))
	{
		SQLBindCol( m_hStmt, 1, SQL_C_LONG, &m_sid, 0, &m_info );
		SQLBindCol( m_hStmt, 2, SQL_C_CHAR, &m_path, DB_TEXTLEN, &m_info );
		SQLBindCol( m_hStmt, 3, SQL_C_CHAR, &m_name, DB_TEXTLEN, &m_info );
		SQLBindCol( m_hStmt, 4, SQL_C_LONG, &m_fs, 0, &m_info );
		SQLBindCol( m_hStmt, 5, SQL_C_LONG, &m_numChan, 0, &m_info );
		bool bret= fetch();
		closeQuery();
		return bret;
	}
	else
		return false;
}

bool CSoundCollectionDB::insertSound(string path, string name, int fs,int numChan)
{
	unsigned int pos=0,pos1;
	string modpath;
	unsigned int pathlen=path.length();
	while((pos1=path.find("\\",pos))<pathlen)
	{
		modpath=modpath+path.substr(pos,pos1-pos)+"\\\\";
		pos=pos1+1;
	}
	modpath=modpath+path.substr(pos)+"\\\\";
	string sSQL="insert into sounds(folder,name,fs,numchan) values(\'" + modpath+"\',\'"+name+"\',"+to_string(fs)+","+to_string(numChan)+");";
	return _executeSQLStmt(sSQL, "insertSound", false);
}

string CSoundCollectionDB::getPath() {
	return m_path;
}

string CSoundCollectionDB::getName() {
	return m_name;
}

int CSoundCollectionDB::getFs() {
	return m_fs;
}

int CSoundCollectionDB::getID() {
	return m_sid;
}

int CSoundCollectionDB::getNumChan() {
	return m_numChan;
}

