/*
 * CUserInterface.cpp
 */
#include <conio.h>
#include <iostream>
using namespace std;

#include "CUserInterface.h"

CUserInterfaceCmdIOW::CUserInterfaceCmdIOW() {
}

void CUserInterfaceCmdIOW::init()
{
	m_iow.open();
	m_ampMeter.init(-.1,.1,SCALING_MODE_LIN, -6., &m_iow);
}

int CUserInterfaceCmdIOW::getListSelection(string* items, int* idItems) {
	int usel=CUI_UNKNOWN;
	int numItems=0;
	while(!items[numItems].empty())numItems++;
	int rmin=0,rmax=numItems-1;
	if(idItems != NULL)
	{
		rmin=idItems[0];
		rmax=idItems[0];
		for(int i=0; i<numItems;i++)
		{
			if(idItems[i]<rmin)rmin=idItems[i];
			else if(idItems[i]>rmax)rmax=idItems[i];
		}
	}

	for(int i=0; i<numItems;i++)
	{
		if(idItems==NULL)
			cout << "[" << i << "]\t";
		else
			cout << "[" << idItems[i] << "]\t";
		cout << items[i] << endl;
	}
	cout << "your selection (number): " << endl;
	cin >> usel;
	cin.clear();
	while (cin.get() != '\n');

	if((usel >= rmin) && (usel <= rmax))
		return usel;
	else
		return CUI_UNKNOWN;
}

void CUserInterfaceCmdIOW::showMessage(string msg) {
	cout << msg << endl;
}

bool CUserInterfaceCmdIOW::wait4Key(bool bBlock) {
	if(m_iow.getState()==IOW_S_NOTREADY)m_iow.open();
	if((m_iow.getState()==IOW_S_READY))
	{
		if(bBlock == true)
		{
			while(false == m_iow.keyPressed());
			return true;
		}
		else
			return m_iow.keyPressed();
	}
	else
	{
		if(bBlock == true)
		{
			cin.get();
			return true;
		}
		else
		{
			if(_kbhit())
			{
				while (cin.get() != '\n');
				return true;
			}
			else
				return false;
		}
	}
}

void CUserInterfaceCmdIOW::showAmplitude(float* databuf, int bufsize) {
	if(m_iow.getState()==IOW_S_NOTREADY)m_iow.open();
	if(m_iow.getState()==IOW_S_NOTREADY)
		m_ampMeter.print(databuf,bufsize);
	else
		m_ampMeter.write(databuf,bufsize);
}

void CUserInterfaceCmdIOW::setAmplitudeScaling(SCALING_MODES mode) {
	if(mode==SCALING_MODE_LOG)
		m_ampMeter.init(-.1,.1,SCALING_MODE_LOG, -30., &m_iow);
	else
		m_ampMeter.init(-.1,.1,SCALING_MODE_LIN, -6., &m_iow);
}

string CUserInterfaceCmdIOW::getUserInputPath() {
	string path,modpath;;
	getline(cin,path);
	unsigned int pathlen=path.length();
	unsigned int pos=0, pos1=0;
	while((pos1=path.find("\\",pos))<pathlen)
	{
		modpath=modpath+path.substr(pos,pos1-pos)+"\\";
		pos=pos1+1;
	}
	modpath=modpath+path.substr(pos)+"\\";
	return modpath;
}
