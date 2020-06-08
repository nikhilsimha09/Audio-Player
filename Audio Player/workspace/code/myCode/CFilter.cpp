/*
 * CFilter.cpp
 */
#include <math.h>
#include "CASDDException.h"
#include "CFilter.h"

CFilter::CFilter(float* ca, float* cb, int order, int delay_ms, int fs, int channels)
{
	int numCoeff;
	if(order == 0 )
	{
		m_order=(int)(abs(delay_ms)*abs(fs)/1000.);
		m_bDelay=true;
		numCoeff=2;
	}
	else
	{
		m_order=abs(order);
		m_bDelay=false;
		numCoeff=m_order+1;
	}
	m_channels=abs(channels);
	m_firstZ=0;

	if((m_order!=0) && (m_channels!=0))
	{
		// buffer for the intermediate filter states from last sample
		m_z=new float[m_channels*(m_order+1)];
		for(int i=0;i < m_channels*(m_order+1);i++){m_z[i]=0.;}

		if((ca!=NULL) && (cb!=NULL))
		{
			m_a=new float[numCoeff];
			m_b=new float[numCoeff];
			for(int i=0;i<numCoeff;i++)
			{
				m_a[i]=ca[i]/ca[0];
				m_b[i]=cb[i]/ca[0];
			}
		}
		else
			throw CASDDException(SRC_Filter, -1, "Filter coefficients not available!");
	}
	else
		throw CASDDException(SRC_Filter, -1, "Filter order and channels must not be zero!");
}

CFilter::~CFilter()
{
	if(m_a!=NULL)
		delete[]m_a;
	if(m_b!=NULL)
		delete[]m_b;
	if(m_z!=NULL)
		delete[]m_z;
}

void CFilter::reset()
{
	m_firstZ=0;
	for(int i=0;i < m_channels*(m_order+1);i++)
	{
		m_z[i]=0.;
	}
}

int CFilter::getOrder() {
	return m_order;
}
bool CFilter::filter(float* x, float* y, int framesPerBuffer)
{
	if((framesPerBuffer < m_order) || (x == NULL) || (y == NULL))
		return false;
	int bufsize=framesPerBuffer*m_channels;
	if(m_bDelay)
		return _filterDelay(x, y, framesPerBuffer, bufsize);
	else
		return _filter(x, y, framesPerBuffer, bufsize);
}

bool CFilter::_filter(float* x, float* y, int framesPerBuffer, int bufsize)
{
	for(int k=0; k < bufsize; k+=m_channels)
	{
		for(int c=0;c<m_channels;c++)
		{
			y[k+c]=m_b[0]*x[k+c]+m_z[0+c];
		}
		for(int n=1; n <= m_order; n++)
		{
			for(int c=0;c<m_channels;c++)
				m_z[m_channels*(n-1)+c]=m_b[n]*x[k+c]+m_z[m_channels*n+c]-m_a[n]*y[k+c];
		}
	}
	return true;
}

bool CFilter::_filterDelay(float* x, float* y, int framesPerBuffer, int bufsize)
{
//	int bufsize=framesPerBuffer*m_channels;
//	for(int k=0; k < bufsize; k++)
//	{
//		y[k]=m_b[0]*x[k]+m_z[(m_firstZ+k)%(m_channels*(m_order+1))];
//		m_z[(m_firstZ+k)%(m_channels*(m_order+1))]=m_b[1]*x[k]-m_a[1]*y[k];
//	}
//	m_firstZ=(m_firstZ+(bufsize-1))%(m_channels*(m_order+1))+1;

	for(int k=0; k < bufsize; k++)
	{
		y[k]=m_b[0]*x[k]+m_z[m_firstZ];
		m_z[m_firstZ]=m_b[1]*x[k]-m_a[1]*y[k];
		m_firstZ=(m_firstZ+1)%(m_channels*(m_order+1));
	}

	return true;
}


