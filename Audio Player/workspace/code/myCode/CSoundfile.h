/*
 * CSoundfile.h
 */

#ifndef SRC_CSOUNDFILE_H_
#define SRC_CSOUNDFILE_H_

#include "sndfile.h"
#include "CFile.h"

/**
 * Sound file handling class uses the open source sndfile library
 * see http://www.mega-nerd.com/libsndfile/api.html
 * (sndfile.h, libsndfile-1.lib,libsndfile-1.dll)
 *
 */
class CSoundFile : public CFile
{
private:
	/**
	 *\brief  special sound file pointer (is NULL, until the file has been opened successfully)
	 */
	SNDFILE* m_pSFile;
	/**
	 *\brief  structure with informations about the sound file
	 */
	SF_INFO m_sfinfo;

public:
	CSoundFile(const char * path, const int mode=FILE_UNKNOWN);
	~CSoundFile();
	/**
	 * opens a sound file (implements CFile interface)
	 */
	void open();
	/**
	 * opens a sound file for writing (adds a new behavior, because the
	 * number of channels and the sampling rate are needed to open a sound file for writing)
	 */
	void openW(int numCh, int fs);
	/**
	 * closes a file (implements CFile interface)
	 */
	void close();
	/**
	 * reads the content of the file (implements CFile interface)
	 */
	int read(char* buf, int bufsize);
	/**
	 * reads a block of frames as float type samples and returns the number of frames read
	 */
	int read(float* buf, int framesPerBlock);
	/**
	 * prints content of file on console
	 */
	void print(void);
	/**
	 * writes the content of the file
	 */
	void write(float* buf, int framesPerBuffer);
	/**
	 * rewinds an open sound file
	 */
	void rewind();
	/**
	 * methods to retrieve information about the sound file
	 */
	int getNumFrames();
	int getSampleRate();
	int getNumChannels();
};
#endif /* SRC_CSOUNDFILE_H_ */
