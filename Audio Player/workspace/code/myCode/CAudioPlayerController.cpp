/*
 * CAudioPlayerController.cpp
 */
////////////////////////////////////////////////////////////////////////////////
// Header
#define USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>			// functions to scan files in folders (used in Lab04prep_DBAdminInsert)
using namespace std;

#include "CASDDException.h"


#include "CFile.h"
#include "CSoundFile.h"

#include "CFilterDB.h"
#include "CSoundCollectionDB.h"

#include "CFilter.h"
#include "CSimpleAudioOutStream.h"

#include "CUserInterface.h"
#include "CAudioPlayerController.h"

CAudioPlayerController::CAudioPlayerController() {
	m_pSFile=NULL;
	m_pFilter=NULL;
	m_ui=NULL;
}

CAudioPlayerController::~CAudioPlayerController() {
	if(m_pSFile)delete m_pSFile;
	if(m_pFilter)delete m_pFilter;
}

void CAudioPlayerController::run(CUserInterface* pui) {
	try
	{
		m_ui=pui;
		init();
	}
	catch(CASDDException& e)
	{
		string eMsg="Error from: ";
		m_ui->showMessage(eMsg+e.getSrcAsString()+""+e.getErrorText());
		return;
	}

	string mainMenue[]={"select sound","select filter","select amplitude meter scaling mode","play sound","manage sound collection", "manage filter collection", "terminate player", ""};
	while(1)
	{
		try
		{
			int selitem=m_ui->getListSelection(mainMenue);
			switch(selitem)
			{
			case 0: selectSound();break;
			case 1: selectFilter();break;
			case 2: selectAmplitudeScale();break;
			case 3: play();break;
			case 4: manageSoundCollection();break;
			case 5: manageFilterCollection();break;
			default: return;
			}
		}
		catch(CASDDException& e)
		{
			string eMsg="Error from: ";
			m_ui->showMessage(eMsg+e.getSrcAsString()+""+e.getErrorText());
		}
	}
}

void CAudioPlayerController::init()
{
	m_ui->init();
	m_soundColl.allowPrint(false); // no prining
	m_filterColl.allowPrint(false); // no prining
	if(false == m_soundColl.open("SoundCollectionDB","root"))
		throw CASDDException(SRC_Database,-1,m_soundColl.getSQLErrorMsg());
	if(false == m_filterColl.open("AudioFilterCollectionDB","root"))
		throw CASDDException(SRC_Database,-1,m_filterColl.getSQLErrorMsg());
}

void CAudioPlayerController::play() {
	if(!m_pSFile)
	{
		m_ui->showMessage("Error from play: No sound file. Select sound file before playing!");
		return;
	}

	if(m_pFilter)
	{
		if(m_filterColl.getFs() && (m_pSFile->getSampleRate()!=m_filterColl.getFs()))
		{
			m_ui->showMessage("Message from play: Filter does not fit sound! Playing unfiltered sound.");
			delete m_pFilter;
			m_pFilter=NULL;
		}
	}

	// configure sample buffer
	int framesPerBlock; 			// 1s/8=125ms per block
	if(m_pFilter)
		framesPerBlock= m_pSFile->getSampleRate()/8 > m_pFilter->getOrder() ? m_pSFile->getSampleRate()/8 : m_pFilter->getOrder() * 2;
	else
		framesPerBlock= m_pSFile->getSampleRate()/8; // 1s/8=125ms per block
	int sblockSize=m_pSFile->getNumChannels()*framesPerBlock; 	// total number of samples per block

	float* sbuf=new float[sblockSize];
	float* sfbuf=new float[sblockSize];
	float* playbuf;
	if(m_pFilter==NULL)playbuf=sbuf;
	else playbuf=sfbuf;

	// open and start the audio stream
	m_audiostream.open(m_pSFile->getNumChannels(), m_pSFile->getSampleRate(), framesPerBlock,false);
	m_audiostream.start();

	bool bPlay=true;
	m_ui->showMessage("Press button or key to start!");
	m_ui->wait4Key();

	// play the file
	// reads the number of frames passed (1 frame == 1 for mono, 2 for stereo, channel number in general)
	int readSize= m_pSFile->read(sbuf, framesPerBlock);
	if(m_pFilter)
	{
		if(false==m_pFilter->filter(sbuf,sfbuf,framesPerBlock))
		{
			delete[]sbuf;
			delete[]sfbuf;
			throw CASDDException(SRC_Filter,-1,"Filter order exceeds buffer size!");
		}
	}

	while( framesPerBlock == readSize)
	{
		if(m_ui->wait4Key(false)==true)bPlay=!bPlay;
		if(bPlay)
		{
			if(m_pFilter)
				m_pFilter->filter(sbuf,sfbuf,framesPerBlock);
			m_audiostream.play(playbuf,framesPerBlock);
			m_ui->showAmplitude(playbuf,sblockSize);
			readSize=m_pSFile->read(sbuf, framesPerBlock);
		}
	}

	// process the rest of data
	if(m_pFilter)m_pFilter->filter(sbuf,sfbuf,framesPerBlock);
	m_audiostream.play(playbuf,readSize);
	m_ui->showAmplitude(playbuf,readSize*m_pSFile->getNumChannels());

	// prepare next playing
	m_audiostream.close();
	m_pSFile->rewind();

	// release resources
	// audio data buffers
	if(sbuf)delete[]sbuf;
	if(sfbuf)delete[]sfbuf;
}

void CAudioPlayerController::selectSound()
{
	int numsnd=m_soundColl.selectNumSounds();
	if(numsnd > 0)
	{
		string* pSounds=new string[numsnd+1];
		int* pIDs=new int[numsnd];

		m_soundColl.selectAllSounds();

		for(int i=0; i < numsnd; i++)
		{
			m_soundColl.fetch();
			pIDs[i]= m_soundColl.getID();
			pSounds[i]=m_soundColl.getPath() + m_soundColl.getName() + "[" + to_string(m_soundColl.getFs()) + "Hz, " + to_string(m_soundColl.getNumChan()) + " Channels]";
		}
		m_soundColl.closeQuery();
		int sID=m_ui->getListSelection(pSounds,pIDs);
		delete[]pSounds;
		delete[]pIDs;
		if(sID!=CUI_UNKNOWN)
		{
			if(m_soundColl.selectSoundData(sID))
			{
				CSoundFile* psf=new CSoundFile((m_soundColl.getPath()+m_soundColl.getName()).c_str(),FILE_READ);
				try{
					psf->open();
					if(m_pSFile)delete m_pSFile;
					m_pSFile=psf;
				}
				catch(CASDDException& e){
					m_ui->showMessage("Error from " + e.getSrcAsString() + ": " + e.getErrorText());
				}
			}
			else
				m_ui->showMessage("Error from selectSound: No sound data available!");
		}
		else
			m_ui->showMessage("Error from selectSound: Invalid sound selection!");
	}
	else
		throw CASDDException(SRC_Database,-1,"No sounds available!");
}

void CAudioPlayerController::selectFilter() {
	if(!m_pSFile)
	{
		m_ui->showMessage("Error from selectFilter: No sound file. Select sound file before filter!");
		return;
	}
	int fs=m_pSFile->getSampleRate();

	// list the appropriate filters for the sound
	int numflt=m_filterColl.selectNumFilters(fs);
	if(numflt)
	{
		string* pFlt=new string[numflt+2];
		int* pFIDs=new int[numflt+1];

		m_filterColl.selectFilters(fs);

		for(int i=0; i < numflt; i++)
		{
			m_filterColl.fetch();
			pFIDs[i]= m_filterColl.getFilterID();
			pFlt[i]= m_filterColl.getFilterType() + "/" + m_filterColl.getFilterSubType()
				   + ", order=" + to_string(m_filterColl.getOrder()) + "/delay="
				   + to_string(m_filterColl.getDelay()) + "s]: "+ m_filterColl.getFilterInfo();
		}
		m_filterColl.closeQuery();

		pFIDs[numflt]=-1;
		pFlt[numflt]="-1 [unfiltered sound]";
		m_filterColl.closeQuery();

		// choose a filter
		int fid=m_ui->getListSelection(pFlt,pFIDs);
		delete[]pFlt;
		delete[]pFIDs;

		if(fid != CUI_UNKNOWN)
		{
			if(fid>=0)
			{
				// get the filters data
				if(true == m_filterColl.selectFilterData(fid))
				{
					if(m_pFilter)delete m_pFilter;
					// create filter
					m_pFilter=new CFilter(m_filterColl.getACoeffs(), m_filterColl.getBCoeffs(), m_filterColl.getOrder(), m_filterColl.getDelay(), m_pSFile->getSampleRate(), m_pSFile->getNumChannels());
				}
				else
				{
					m_ui->showMessage("Error from selectFilter: No filter data available! Did not change filter. ");
				}
			}
			else
			{
				if(m_pFilter)
				{
					delete m_pFilter;
					m_pFilter=NULL;
					m_ui->showMessage("Message from selectFilter: Filter removed. ");
				}
			}
		}
		else
			m_ui->showMessage("Error from selectFilter: Invalid filter selection! Play unfiltered sound. ");
	}
	else
		m_ui->showMessage("Error from selectFilter: No filter available! Play unfiltered sound. ");
}

void CAudioPlayerController::selectAmplitudeScale() {
	string pscale_menue[]={"linear","logarithmic",""};
	int sel=m_ui->getListSelection(pscale_menue);
	switch(sel)
	{
	case 0: m_ui->setAmplitudeScaling(SCALING_MODE_LIN);;break;
	case 1: m_ui->setAmplitudeScaling(SCALING_MODE_LOG);;break;
	default: m_ui->setAmplitudeScaling(SCALING_MODE_LIN);break;
	}
}

void CAudioPlayerController::manageSoundCollection() {
	string sndfolder;

	m_ui->showMessage("Enter sound file path: ");
	sndfolder=m_ui->getUserInputPath();

	dirent* entry;
	DIR* dp=NULL;
	string sndfile;

	 dp = opendir(sndfolder.c_str());
	 if (dp == NULL)
	 {
		 m_ui->showMessage("Could not open sound file folder.");
		 return;
	 }

	 while((entry = readdir(dp)))
	 {
		 sndfile=entry->d_name;
		 m_ui->showMessage(sndfile+":");
		 if(sndfile.rfind(".wav")!=string::npos)
		 {
			CSoundFile mysf((sndfolder+sndfile).c_str(),FILE_READ);
			mysf.open();
			string fileinfo = "Inserting sound file: channels(" + to_string(mysf.getNumChannels()) + ") fs(" + to_string(mysf.getSampleRate()) + "Hz)";
			m_ui->showMessage(fileinfo);
			if(false == m_soundColl.insertSound(sndfolder,sndfile,mysf.getSampleRate(),mysf.getNumChannels()))
				m_ui->showMessage(m_soundColl.getSQLErrorMsg());
		 }
		 else
			 m_ui->showMessage(" irrelevant file of other type or directory");
	 }
	if(dp)closedir(dp);
}

void CAudioPlayerController::manageFilterCollection() {
	string fltfolder;
	m_ui->showMessage("Enter filter file path: ");
	fltfolder=m_ui->getUserInputPath();

	 dirent* entry;
	 DIR* dp;
	 string fltfile;

	 dp = opendir(fltfolder.c_str());
	 if (dp == NULL)
	 {
		 m_ui->showMessage("Could not open filter file folder.");
		 return;
	 }

	 while((entry = readdir(dp)))
	 {
		 fltfile=entry->d_name;
		 m_ui->showMessage("Filter file to insert into the database: " + fltfolder+fltfile+":");

		 if(fltfile.rfind(".txt")!=string::npos)
		 {
			const int rbufsize=100;
			char readbuf[rbufsize];

			// get all sampling frequencies contained in the file
			int numFs=rbufsize;
			int fsbuf[rbufsize];
			CFilterFile::getFs(fltfolder+fltfile,fsbuf,numFs);

			for(int i=0; i < numFs;i++)
			{
				CFilterFile ff(fsbuf[i], (fltfolder+fltfile).c_str(), FILE_READ);
				ff.open();
				if(ff.read(readbuf,rbufsize))
				{
					string fileinfo = "Inserting filter file: " + ff.getFilterType() + "/" + ff.getFilterSubType() + " filter [order=" + to_string(ff.getOrder())
							        + ", delay=" + to_string(ff.getDelay()) + "s, fs=" + to_string(fsbuf[i]) + "Hz] " + ff.getFilterInfo();
					m_ui->showMessage(fileinfo);

					if( false == m_filterColl.insertFilter(ff.getFilterType(),ff.getFilterSubType(),
									  fsbuf[i],ff.getOrder(),1000*ff.getDelay(),ff.getFilterInfo(),
									  ff.getBCoeffs(),ff.getNumBCoeffs(),ff.getACoeffs(),ff.getNumACoeffs()))
						m_ui->showMessage(m_filterColl.getSQLErrorMsg());
				}
				else
					m_ui->showMessage("No coefficients available for fs=" + to_string(fsbuf[i]) + "Hz");
				ff.close();
			}
		 }
		 else
			 m_ui->showMessage(" irrelevant file of other type or directory");
	 }
	 closedir(dp);
}
