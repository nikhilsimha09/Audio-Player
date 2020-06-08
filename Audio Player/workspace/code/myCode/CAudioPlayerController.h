/*
 * CAudioPlayerController.h
 */

#ifndef SRC_CAUDIOPLAYERCONTROLLER_H_
#define SRC_CAUDIOPLAYERCONTROLLER_H_
#include "CFile.h"
#include "CSoundFile.h"

#include "CFilterDB.h"
#include "CSoundCollectionDB.h"

#include "CFilter.h"
#include "CSimpleAudioOutStream.h"

#include "CUserInterface.h"

class CAudioPlayerController {
private:
	CUserInterface* m_ui;
	CSimpleAudioOutStream m_audiostream;
	CFilterDB m_filterColl;
	CSoundCollectionDB m_soundColl;
	CSoundFile* m_pSFile;
	CFilter* m_pFilter;

public:
	CAudioPlayerController();
	~CAudioPlayerController();
	void run(CUserInterface* pui);

private:
	void init();
	void play();
	void selectSound();
	void selectFilter();
	void selectAmplitudeScale();
	void manageSoundCollection();
	void manageFilterCollection();
};

#endif /* SRC_CAUDIOPLAYERCONTROLLER_H_ */
