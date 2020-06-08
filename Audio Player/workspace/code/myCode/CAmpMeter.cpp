/*
 * CAmpMeter.cpp
 */
#include <iostream>
//#include <stdlib.h>
#include <math.h>
using namespace std;

#include "CIOWarrior.h"
#include "CAmpMeter.h"

CAmpMeter::CAmpMeter()
{
	// todo: initialize all attributes (see UML class diagram)
	m_scmode=SCALING_MODE_LIN;			// logarithmic or linear bar?
	m_scMax=0;
	for(int i=0;i<8;i++)
		m_thresholds[i]=0;				// thresholds for the bar with 8 segments (LEDs)
	m_iowdev=NULL;						// address of an object which controls an IOWarrior device

}

void CAmpMeter::init(float min, float max, SCALING_MODES scmode, int logScaleMin,CIOWarrior* iowdev)
{
	// todo: initialize the amplitude meter on the basis of the given parameter values
	// The maximum absolute value of min and max is taken for the maximum of the scale.
	// The thresholds are calculated for a LED line with 8 LEDs in dependence of the given scaling mode (scmode).
		// Linear scaling: 0 ... maximum of the scale
		// Logarithmic scaling: logScaleMin ... 0 [dB], logScaleMin must be negative
	m_scMax=fmax(fabs(max), fabs(min));
	m_scmode=scmode;
	m_iowdev=iowdev;

	for(int i=0;i<8;i++)
	{
		if(m_scmode==SCALING_MODE_LOG)
		{
			if(logScaleMin > 0)logScaleMin=-logScaleMin;
			m_thresholds[i]=logScaleMin+(i+1)*abs(logScaleMin)/9.; // logScaleMin ... 0dB
		}
		else
			m_thresholds[i]=(i+1)*m_scMax/9.;
	}
}

bool CAmpMeter::write(float* databuf, unsigned long databufsize)
{
	if(NULL == databuf || NULL==m_iowdev)return false;
	return write(_getValueFromBuffer(databuf, databufsize));
}

bool CAmpMeter::write(float data)
{
	// todo:
	// if an IOWarrior is connected it gets the appropriate bar pattern and writes it to the IOWarrior
	// otherwise it does nothing
	if(m_iowdev)
	{
		if(m_iowdev->getState() == IOW_S_NOTREADY)
			return false;
	}
	else
		return false;

	char pat;
	pat= _getBarPattern(data);
	m_iowdev->write(pat);
	return true;
}

void CAmpMeter::print(float data)
{
	//todo:
	// get the appropriate bar pattern
	// print the bar pattern in binary format on the screen (see CIOWarrior::printData() method for an example)
	char pat;
	pat= _getBarPattern(data);
	for(int i=0;i<8;i++)
	{
		pat & 0x80 ? cout << '1' : cout << '0';
		pat <<= 1;
	}
	cout << "\r";
}

void CAmpMeter::print(float* databuf, unsigned long databufsize)
{
	if(NULL == databuf)
	{
		cout << "no data for printing amplitude bar!" << endl;
		return;
	}
	return print(_getValueFromBuffer(databuf, databufsize));
}


unsigned char CAmpMeter::_getBarPattern(float data)
{
	char pat=0;
	// todo: Calculate appropriate bar pattern pat for data. Data is a linear value in any case. The bar pattern may be used
	// for visualization on the screen or the LED line.
	// Example: pat is 0b11111111 if the absolute value of data is equal to the maximum data value (m_scMax)
	// In dependence of the scaling mode (m_scmode), the absolute value of data (linear scaling) or
	// the dB value of the absolute value of data (logarithmic scaling) has to be used for the bar pattern calculation.
	// Before calculating the dB value, the absolute value of data shall be divided by the linear scale maximum to
	// adjust the highest value to 0 dB (this is called peak normalization).
	if(m_scmode==SCALING_MODE_LOG)
		data=20.*log10(fabs(data/m_scMax)+1e-16);
	else
		data=fabs(data);

	int i=0;
	char mask=1;
	while(data > m_thresholds[i])
	{
		pat|=mask;
		mask <<= 1;
		i++;
	}
	return pat;
}


float CAmpMeter::_getValueFromBuffer(float* databuf, unsigned long databufsize)
{
/*
	float dmax=0., d;
	unsigned long imax=0;
	for(unsigned long i=0; i < databufsize; i++)
	{
		d=fabs(databuf[i]);
		if(dmax < d)
		{
			dmax=d;
			imax=i;
		}
	}
	return databuf[imax];
*/
	// better implementation, e.g. calculate the power of the buffered data
	float pbuf=0;
	for(unsigned long i=0; i < databufsize; i++)
	{
		pbuf+=(databuf[i]*databuf[i])/databufsize;
	}
	return pbuf;

}

////////////////////////////////
// uses IOWarrior control with exception handling
//
CAmpMeterEx::CAmpMeterEx()
{
	// todo: initialize all attributes (see UML class diagram)
	m_scmode=SCALING_MODE_LIN;			// logarithmic or linear bar?
	m_scMax=0;
	for(int i=0;i<8;i++)
		m_thresholds[i]=0;				// thresholds for the bar with 8 segments (LEDs)
	m_iowdev=NULL;						// address of an object which controls an IOWarrior device

}

void CAmpMeterEx::init(float min, float max, SCALING_MODES scmode, int logScaleMin,CIOWarriorEx* iowdev)
{
	// todo: initialize the amplitude meter on the basis of the given parameter values
	// The maximum absolute value of min and max is taken for the maximum of the scale.
	// The thresholds are calculated for a LED line with 8 LEDs in dependence of the given scaling mode (scmode).
		// Linear scaling: 0 ... maximum of the scale
		// Logarithmic scaling: logScaleMin ... 0 [dB], logScaleMin must be negative
	m_scMax=fmax(fabs(max), fabs(min));
	m_scmode=scmode;
	m_iowdev=iowdev;

	for(int i=0;i<8;i++)
	{
		if(m_scmode==SCALING_MODE_LOG)
		{
			if(logScaleMin > 0)logScaleMin=-logScaleMin;
			m_thresholds[i]=logScaleMin+(i+1)*abs(logScaleMin)/9.; // logScaleMin ... 0dB
		}
		else
			m_thresholds[i]=(i+1)*m_scMax/9.;
	}
}

bool CAmpMeterEx::write(float* databuf, unsigned long databufsize)
{
	if(NULL == databuf || NULL==m_iowdev)return false;
	return write(_getValueFromBuffer(databuf, databufsize));
}

bool CAmpMeterEx::write(float data)
{
	if(NULL==m_iowdev)return false;
	char pat;
	pat= _getBarPattern(data);
	m_iowdev->write(pat);
	return true;
}

void CAmpMeterEx::print(float data)
{
	//todo:
	// get the appropriate bar pattern
	// print the bar pattern in binary format on the screen (see CIOWarrior::printData() method for an example)
	char pat;
	pat= _getBarPattern(data);
	for(int i=0;i<8;i++)
	{
		pat & 0x80 ? cout << '1' : cout << '0';
		pat <<= 1;
	}
	cout << "\r";
}

void CAmpMeterEx::print(float* databuf, unsigned long databufsize)
{
	if(NULL == databuf)
	{
		cout << "no data for printing amplitude bar!" << endl;
		return;
	}
	return print(_getValueFromBuffer(databuf, databufsize));
}


unsigned char CAmpMeterEx::_getBarPattern(float data)
{
	char pat=0;
	// todo: Calculate appropriate bar pattern pat for data. Data is a linear value in any case. The bar pattern may be used
	// for visualization on the screen or the LED line.
	// Example: pat is 0b11111111 if the absolute value of data is equal to the maximum data value (m_scMax)
	// In dependence of the scaling mode (m_scmode), the absolute value of data (linear scaling) or
	// the dB value of the absolute value of data (logarithmic scaling) has to be used for the bar pattern calculation.
	// Before calculating the dB value, the absolute value of data shall be divided by the linear scale maximum to
	// adjust the highest value to 0 dB (this is called peak normalization).
	if(m_scmode==SCALING_MODE_LOG)
		data=20.*log10(fabs(data/m_scMax)+1e-16);
	else
		data=fabs(data);

	int i=0;
	char mask=1;
	while(data > m_thresholds[i])
	{
		pat|=mask;
		mask <<= 1;
		i++;
	}
	return pat;
}


float CAmpMeterEx::_getValueFromBuffer(float* databuf, unsigned long databufsize)
{
/*
	float dmax=0., d;
	unsigned long imax=0;
	for(unsigned long i=0; i < databufsize; i++)
	{
		d=fabs(databuf[i]);
		if(dmax < d)
		{
			dmax=d;
			imax=i;
		}
	}
	return databuf[imax];
*/
	// better implementation, e.g. calculate the power of the buffered data
	float pbuf=0;
	for(unsigned long i=0; i < databufsize; i++)
	{
		pbuf+=(databuf[i]*databuf[i])/databufsize;
	}
	return pbuf;

}
