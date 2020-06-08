/*
 * File.h
 *
 *  Created on: 14.11.2019
 *      Author: A. Wirth
 */

/**
 * \file CFile.h
 *
 * \brief header file for class CFile (differs slightly from the class used within the lesson!)
 *
 * \see CFile
 */
#ifndef FILE_H_
#define FILE_H_

#include <string>
using namespace std;

/**
 * \brief base class for files
 *
 *\brief   this class defines an interface for all file classes
 *\brief   the following methods are pure virtual that is, they are not implemented
 *\brief   in the base class and each derived class has to implement these methods
 */
/**
 * \brief available file open modes (may be combined logically to express e.g. ReadWrite mode)
 */
enum ASDD_FILEMODES{FILE_UNKNOWN=0x00, FILE_READ=0x01, FILE_WRITE=0x02, FILE_WRITEAPPEND=0x04};
/**
 * \brief Error Codes
 */
enum ASDD_FILEERRORS{FILE_E_UNKNOWNOPENMODE,FILE_E_NOFILE,FILE_E_FILENOTOPEN,FILE_E_NOBUFFER,FILE_E_READ,FILE_E_CANTREAD,FILE_E_CANTWRITE,FILE_E_WRITE,FILE_E_SPECIAL};

class CFile {
protected:
	/**
	 *\brief file open mode
	 */
	unsigned char m_mode;
	/**
	 *\brief file name with absolute or relative path
	 */
	string m_path;

public:
	/**
	 *\brief constructor sets the complete file path and the open mode
	 *\param path [in] complete relative or absolute file path and name
	 *\param mode [in] file open mode may be a logical combination of ASDD_FILEMODES
	 */
	CFile(const char* path=NULL, const int mode=FILE_UNKNOWN);
	virtual ~CFile();

	/**
	 *\brief opens a file
	 */
	virtual void open()=0;
	/**
	 *\brief closes a file
	 */
	virtual void close()=0;
	/**
	 *\brief  reads the content of the file into a buffer
	 *\brief  returns the number of elements read
	 *\param buf[in] - address of a buffer to store the data
	 *\param bufsize[in] - size of the buffer in bytes
	 */
	virtual int read(char* buf, int bufsize)=0;

	/**
	 *\brief   prints the content of the file on console
	 *\brief   is not pure virtual that is, it is implemented in this class
	 *\brief   and may be used or overwritten by the derived classes
	 */
	virtual void print(void);

protected:
	/**
	 *\brief converts the file error codes into textual messages
	 */
	string getErrorTxt(ASDD_FILEERRORS e);
};

/**
 *\brief   class to read and write files without any format
 *\brief   derived from CFile
 *\brief   implements the interface for the operations open, close and read
 *\brief   implements an additional method for writing files
 *\brief   overloads the basic classes print method
 */
class CRawFile : public CFile
{
protected:
	/**
	 *\brief  File pointer (is NULL, until the file has been opened successfully)
	 */
	FILE* m_pFile;
public:
	CRawFile(const char * path, const int mode=FILE_UNKNOWN);
	virtual ~CRawFile();

	/**
	 *\brief opens a file
	 */
	virtual void open();
	/**
	 *\brief closes a file
	 */
	virtual void close();
	/**
	 *\brief reads the content of the file (implements the interface)
	 */
	virtual int read(char* buf, int bufsize);
	/**
	 *\brief  writes the content of the buffer into a file
	 *\brief  returns the number of elements read
	 *\param buf[in] - address of a buffer to store the data
	 *\param bufsize[in] - size of the buffer in bytes
	 */
	virtual void write(char* buf, int bufsize);
	/**
	 *\brief  prints content of file on console
	 */
	virtual void print(void);
};

/**
 * class to read files with the ASDD filter file format
 * derived from CRawFile
 * inherits the interface for the operations open, close and read from CRawFile
 * implements additional methods for parsing filter files
 * overloads the basic classes print method
 */
class CFilterFile : public CRawFile
{
private:
	string m_filterType;	// type of the filter (e.g. lowpass, shelving, delay, ...)
	string m_filterSubType;	// subtype of the filter (e.g. butter, feedback, ...)
	string m_filterInfo;	// free textual filter description
	int m_order;			// filter order
	float m_delay;			// delay in seconds
	float* m_b;				// filter numerator coefficients
	int m_blen;		// number of filter numerator coefficients
	float* m_a;				// filter denominator coefficients
	int m_alen;		// number of filter denominator coefficients
	int m_fs;				// sampling frequency

public:
	CFilterFile(int fs, const char * path, const int mode=FILE_UNKNOWN);
	~CFilterFile();
	/**
	 * opens a file
	 */
	void open();
	/**
	 * closes a file
	 */
	void close();
	/**
	 * reads the content of the file (implements the interface)
	 */
	int read(char* buf, int bufsize);
	/**
	 * prints content of the filter file on console
	 */
	void print(void);
	/**
	 * retrieve information from the filter file
	 */
	static void getFs(string path, int* fs, int& numFs);	// get the sample rates for which the file contains coefficients
	int getOrder();						// filter order
	float getDelay();					// filter delay
	string getFilterType();				// type of the filter
	string getFilterSubType();			// sub-type of the filter
	string getFilterInfo();				// free textual filter description
	float* getBCoeffs();				// filter numerator coefficients
	int getNumBCoeffs();				// number of filter numerator coefficients
	float* getACoeffs();				// filter denominator coefficients
	int getNumACoeffs();				// number of filter denominator coefficients
};

#endif /* FILE_H_ */
