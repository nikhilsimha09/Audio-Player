/*
 * CUserInterface.h
 */

#ifndef SRC_CUSERINTERFACE_H_
#define SRC_CUSERINTERFACE_H_
#include <string>
using namespace std;

#include "CIOWarrior.h"
#include "CAmpMeter.h"

#define CUI_UNKNOWN 0xffff // error value (maximum valid is CUI_UNKNOWN-1)

// Strategy interface (pure virtual class) for user interface class
class CUserInterface
{
public:
	CUserInterface(){};
	virtual ~CUserInterface(){};
	virtual void init()=0;
	virtual int getListSelection(string* items, int* idItems=NULL)=0;
	virtual string getUserInputPath()=0;
	virtual void showMessage(string msg)=0;
	virtual bool wait4Key(bool bBlock=true)=0;
	virtual void showAmplitude(float* databuf, int bufsize)=0;
	virtual void setAmplitudeScaling(SCALING_MODES mode)=0;
};

// concrete strategy for a user interface class with command-line and IOWarrior
class CUserInterfaceCmdIOW : public CUserInterface
{
private:
	CIOWarrior m_iow;
	CAmpMeter m_ampMeter;

public:
	CUserInterfaceCmdIOW();
	void init();
	// last item must be an empty string!
	int getListSelection(string* items, int* idItems=NULL);
	string getUserInputPath();
	void showMessage(string msg);
	bool wait4Key(bool bBlock=true);
	void showAmplitude(float* databuf, int bufsize);
	void setAmplitudeScaling(SCALING_MODES mode);
};

#endif /* SRC_CUSERINTERFACE_H_ */
