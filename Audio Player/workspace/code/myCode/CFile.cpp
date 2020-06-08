/*
 * File.cpp
 */
#include <iostream>
#include <string.h>
using namespace std;

#include "CFile.h"
#include "CASDDException.h"

/**
 * Constructor
 */
CFile::CFile(const char* path, int mode) {
	m_path=path;
	if((mode==FILE_UNKNOWN) || ((mode!=FILE_READ) && (mode!=FILE_WRITE) && (mode!=FILE_WRITEAPPEND)))
		mode=FILE_READ|FILE_WRITEAPPEND;
	else
		m_mode=mode;
}

CFile::~CFile() {
}

void CFile::print(void)
{
	cout << "CFile[" << hex << (int)m_mode << dec << "]: " << m_path << endl;
}

/**
 * utility method to get textual description for the error codes
 */
string CFile::getErrorTxt(ASDD_FILEERRORS e) {
	switch(e)
	{
	case FILE_E_UNKNOWNOPENMODE: return string("unknown file open mode");
	case FILE_E_NOFILE: return string("file not found");
	case FILE_E_FILENOTOPEN: return string("file not open");
	case FILE_E_NOBUFFER: return string("no data buffer available");
	case FILE_E_CANTREAD: return string( "file has been opened in write only mode");
	case FILE_E_READ: return string("error during read");
	case FILE_E_CANTWRITE: return string("file has been opened in read only mode");
	case FILE_E_WRITE: return string("error during write");
	case FILE_E_SPECIAL: return string("special file type error");
	default:return string("unknown error");
	}
}

CRawFile::CRawFile(const char* path, int mode)
: CFile(path, mode)
{
	m_pFile=NULL;
}

CRawFile::~CRawFile() {
	close();
}

void CRawFile::open() {
	string mode;
	if(((m_mode & FILE_WRITE) != 0) && ((m_mode & FILE_WRITEAPPEND) != 0))mode="a+";
	else if((m_mode & FILE_WRITE) != 0)mode="w";
	else if((m_mode & FILE_WRITEAPPEND) != 0)mode="a";
	else if((m_mode & FILE_READ) != 0)mode="r";
	else throw CASDDException(SRC_File, FILE_E_UNKNOWNOPENMODE, getErrorTxt(FILE_E_UNKNOWNOPENMODE));

	m_pFile=fopen(m_path.c_str(),mode.c_str());
	if(m_pFile == NULL)throw CASDDException(SRC_File, FILE_E_NOFILE, getErrorTxt(FILE_E_NOFILE));
}

void CRawFile::close() {
	if(m_pFile != NULL)
		fclose(m_pFile);
	m_pFile=NULL;
}

int CRawFile::read(char* buf, int bufsize) {
	if(m_pFile == NULL)throw CASDDException(SRC_File, FILE_E_FILENOTOPEN,getErrorTxt(FILE_E_FILENOTOPEN));
	if((buf == NULL) || (bufsize == 0))throw CASDDException(SRC_File, FILE_E_NOBUFFER,getErrorTxt(FILE_E_NOBUFFER));
	if((m_mode & FILE_READ) == 0)throw CASDDException(SRC_File, FILE_E_CANTREAD,getErrorTxt(FILE_E_CANTREAD));

	int szread= fread(buf, 1, bufsize, m_pFile);
	if((szread != bufsize) && (feof(m_pFile) == 0))
		throw CASDDException(SRC_File, FILE_E_READ,getErrorTxt(FILE_E_READ));
	return szread;
}

void CRawFile::write(char* buf, int bufsize)
{
	if(m_pFile == NULL)throw CASDDException(SRC_File, FILE_E_FILENOTOPEN,getErrorTxt(FILE_E_FILENOTOPEN));
	if(buf == NULL)throw CASDDException(SRC_File, FILE_E_NOBUFFER,getErrorTxt(FILE_E_NOBUFFER));
	if(((m_mode & FILE_WRITE) == 0) && ((m_mode & FILE_WRITEAPPEND) == 0))throw CASDDException(SRC_File, FILE_E_CANTWRITE,getErrorTxt(FILE_E_CANTWRITE));

	int szwrite= fwrite(buf, 1, bufsize, m_pFile);
	if(szwrite != bufsize)
	{
		close();
		throw CASDDException(SRC_File, FILE_E_WRITE,getErrorTxt(FILE_E_WRITE));
	}
}

void CRawFile::print(void)
{
	cout << "CRawFile[" << hex << m_pFile << dec << "]" << endl;
	CFile::print();
}

CFilterFile::CFilterFile(int fs, const char* path, const int mode)
: CRawFile(path, mode)
{
//	if(fs == 0)throw CASDDException(SRC_File, FILE_E_SPECIAL,getErrorTxt(FILE_E_SPECIAL)+": sample rate must be specified for a filter!");
	m_fs=abs(fs);	// sample rate of the filter to load
	m_order=0;		// filter order
	m_delay=0.;		// filter order
	m_b=NULL;		// filter numerator coefficients
	m_blen=0;		// number of filter numerator coefficients
	m_a=NULL;		// filter denominator coefficients
	m_alen=0;		// number of filter denominator coefficients
}

CFilterFile::~CFilterFile()
{
	close();
}

void CFilterFile::open()
{
	if(m_mode != FILE_READ)throw CASDDException(SRC_File, FILE_E_SPECIAL,getErrorTxt(FILE_E_SPECIAL)+": Writing is not implemented for this class!");
	CRawFile::open();		// perform base class open
}

void CFilterFile::close()
{
	if(m_b)
	{
		delete[]m_b;
		m_b=NULL;
	}
	if(m_a)
	{
		delete[]m_a;
		m_a=NULL;
	}
	CRawFile::close();		// perform base class close
}

int CFilterFile::read(char* buf, int bufsize)
{
	if(m_pFile==NULL)throw CASDDException(SRC_File, FILE_E_FILENOTOPEN,getErrorTxt(FILE_E_FILENOTOPEN));
	if((buf == NULL) || (bufsize == 0))throw CASDDException(SRC_File, FILE_E_NOBUFFER,getErrorTxt(FILE_E_NOBUFFER));

	fseek(m_pFile, 0, SEEK_SET);		// start at the beginning of the file

	// get the header
	fgets(buf, bufsize, m_pFile);

	// parse the header
	string s=buf;
	int pos=0,end=0;
	end=s.find(";",pos);
	m_filterType= s.substr(pos, end-pos);
	pos=end+1;
	end=s.find(";",pos);
	m_filterSubType= s.substr(pos, end-pos);
	pos=end+1;
	end=s.find(";",pos);
	string sorder= s.substr(pos, end-pos);
	m_order=stoi(sorder);
	pos=end+1;
	end=s.find(";",pos);
	string sdelay= s.substr(pos, end-pos);
	m_delay=stof(sdelay);
	m_filterInfo=s.substr(end+1);

	// read data
	int i, fsr;
	while( NULL != fgets(buf, bufsize, m_pFile))
	{
		fsr=stoi(buf);			// find fs
		if(m_fs!=fsr && (fsr != 0))
		{
			char* pgot=buf;
			while(pgot)			// skip b coefficients
			{
				pgot=fgets(buf, bufsize, m_pFile);
				if(NULL != strrchr(buf,'\n'))break; // end of line has been read
			}
			while(pgot)			// skip a coefficients
			{
				pgot=fgets(buf, bufsize, m_pFile);
				if(NULL != strrchr(buf,'\n'))break;
			}
		}
		else
		{
			if(m_filterType == "delay")
				m_order=1;

			m_b=new float[m_order+1];
			m_a=new float[m_order+1];
			char sep;
			for(i=0; i<m_order+1;i++)
			{
				if(EOF==fscanf(m_pFile,"%f%c",&m_b[i],&sep))
					break;
				if(sep == '\n')
					break;
			}
			m_blen=i;
			if(sep!='\n')fscanf(m_pFile,"%c",&sep);
			for(i=0; i<m_order+1;i++)
			{
				if(EOF==fscanf(m_pFile,"%f%c",&m_a[i],&sep))
					break;
				if(sep == '\n')
					break;
			}
			m_alen=i;
			if(sep!='\n')fscanf(m_pFile,"%c",&sep);

			if(m_filterType == "delay")
				m_order=0;

			return ftell(m_pFile);						// returns the number of bytes read
		}
	}
	return 0;											// 0 if no appropriate sampling rate found
}


void CFilterFile::print(void)
{
	CRawFile::print();
	cout << m_filterType << "/" << m_filterSubType << " filter [order=" << m_order << ", delay=" << m_delay << "s, fs=" << m_fs << "] " << m_filterInfo << endl;
	cout << "coefficients b={";
	for(int i=0; i < m_blen; i++)
		cout << m_b[i] << "\t";
	cout << "}" << endl << "coefficients a={";
	for(int i=0; i < m_alen; i++)
		cout << m_a[i] << "\t";
	cout << "}" << endl;
}

int CFilterFile::getOrder() {
	return m_order;
}

float CFilterFile::getDelay() {
	return m_delay;
}

string CFilterFile::getFilterType()
{
	return m_filterType;
}

string CFilterFile::getFilterSubType()
{
	return m_filterSubType;
}

string CFilterFile::getFilterInfo()
{
	return m_filterInfo;
}

float* CFilterFile::getBCoeffs()
{
	return m_b;
}

int CFilterFile::getNumBCoeffs()
{
	return m_blen;
}

float* CFilterFile::getACoeffs()
{
	return m_a;
}

int CFilterFile::getNumACoeffs()
{
	return m_alen;
}


// Lab03:
void CFilterFile::getFs(string path, int* fs, int& numFs)
{
	FILE* pFile=fopen(path.c_str(),"r");
	if(pFile==NULL)throw CASDDException(SRC_File, FILE_E_FILENOTOPEN,"Can't open file for reading sample rates!");
	const int bufsize=100;
	char buf[bufsize];

	// skip the header
	do
	{
		fgets(buf, bufsize, pFile);
	}while(NULL == strrchr(buf,'\n'));

	// read fs
	int i=0;
	while( (NULL != fgets(buf, bufsize, pFile)) && (numFs > i))
	{
		fs[i++]=stoi(buf);			// store fs
		for(int j=0; j < 2; j++)	// skip coefficients
		{
			do
			{
				fgets(buf, bufsize, pFile);
			}while(NULL == strrchr(buf,'\n'));
		}
	}
	numFs=i;
	fclose(pFile);
}
