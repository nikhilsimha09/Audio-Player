/*
 * CSoundFile.cpp
 */

#include <iostream>
#include <string.h>
using namespace std;

#include "CSoundFile.h"
#include "CASDDEXception.h"

CSoundFile::CSoundFile(const char* path, const int mode)
: CFile(path, mode)
{
		m_pSFile=NULL;
		memset(&m_sfinfo,0,sizeof(m_sfinfo));
		m_sfinfo.format=SF_FORMAT_WAV|SF_FORMAT_FLOAT;
}

CSoundFile::~CSoundFile()
{
	close();
}

void CSoundFile::open()
{
	int mode;
	if(((m_mode & FILE_READ) != 0) && (((m_mode & FILE_WRITE) != 0) || ((m_mode & FILE_WRITEAPPEND) != 0)))mode=SFM_RDWR;
	else if(((m_mode & FILE_WRITE) != 0) || ((m_mode & FILE_WRITEAPPEND) != 0))mode=SFM_WRITE;
	else if((m_mode & FILE_READ) != 0)mode=SFM_READ;
	else throw CASDDException(SRC_File, FILE_E_UNKNOWNOPENMODE, getErrorTxt(FILE_E_UNKNOWNOPENMODE));

	m_pSFile= sf_open(m_path.c_str(), mode, &m_sfinfo);
	if(!m_pSFile)throw CASDDException(SRC_File, sf_error(m_pSFile), sf_strerror(m_pSFile));
}

void CSoundFile::openW(int numCh, int fs)
{
	if(numCh==0)throw CASDDException(SRC_File, FILE_E_SPECIAL, getErrorTxt(FILE_E_SPECIAL)+": number of channels must not be 0!");
	if(fs==0)throw CASDDException(SRC_File, FILE_E_SPECIAL, getErrorTxt(FILE_E_SPECIAL)+": sampling rate must not be 0!");
	m_sfinfo.samplerate=abs(fs);
	m_sfinfo.channels=abs(numCh);
	open();
}

void CSoundFile::close()
{
	if(m_pSFile != NULL)
		sf_close(m_pSFile);
	m_pSFile=NULL;
}

int CSoundFile::read(char* buf, int bufsize)
{
	if(m_pSFile == NULL)throw CASDDException(SRC_File, FILE_E_FILENOTOPEN,getErrorTxt(FILE_E_FILENOTOPEN));
	if(buf == NULL)throw CASDDException(SRC_File, FILE_E_NOBUFFER,getErrorTxt(FILE_E_NOBUFFER));
	if((m_mode & FILE_READ) == 0)throw CASDDException(SRC_File, FILE_E_CANTREAD,getErrorTxt(FILE_E_CANTREAD));

	int szread= sf_read_float(m_pSFile, (float*)buf, (int)(bufsize/sizeof(float)));
	return szread;
}

int CSoundFile::read(float* buf, int framesPerBlock) {
	return read((char*) buf, getNumChannels()*framesPerBlock*sizeof(float))/getNumChannels();
}

void CSoundFile::write(float* buf, int framesPerBuffer)
{
	if(m_pSFile == NULL)throw CASDDException(SRC_File, FILE_E_FILENOTOPEN,getErrorTxt(FILE_E_FILENOTOPEN));
	if(buf == NULL)throw CASDDException(SRC_File, FILE_E_NOBUFFER,getErrorTxt(FILE_E_NOBUFFER));
	if(((m_mode & FILE_WRITE) == 0) && ((m_mode & FILE_WRITEAPPEND) == 0))throw CASDDException(SRC_File, FILE_E_CANTWRITE,getErrorTxt(FILE_E_CANTWRITE));

	int szwrite= sf_writef_float(m_pSFile, buf, framesPerBuffer);
	if(szwrite != framesPerBuffer)
	{
		close();
		throw CASDDException(SRC_File, FILE_E_WRITE,getErrorTxt(FILE_E_WRITE));
	}
}

void CSoundFile::print(void)
{
	cout << "CSoundFile[" << hex << m_pSFile << dec << "]" << endl;
	cout << "Sound file: channels(" << m_sfinfo.channels << ") frames(" << m_sfinfo.frames;
	cout <<  ") fs("<< m_sfinfo.samplerate/1000. << "kHz)" << endl;
	cout << "Duration: " << m_sfinfo.frames/m_sfinfo.samplerate << "s" << endl;
	CFile::print();
}

void CSoundFile::rewind()
{
	if(m_pSFile == NULL)throw CASDDException(SRC_File, FILE_E_FILENOTOPEN,getErrorTxt(FILE_E_FILENOTOPEN));
	sf_seek(m_pSFile, 0, SEEK_SET);
}

int CSoundFile::getNumFrames() {
	return m_sfinfo.frames;
}

int CSoundFile::getSampleRate() {
	return m_sfinfo.samplerate;
}

int CSoundFile::getNumChannels() {
	return m_sfinfo.channels;
}

