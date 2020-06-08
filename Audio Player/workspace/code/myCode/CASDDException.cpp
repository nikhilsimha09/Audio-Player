/*
 * CASDDException.cpp
 */
#include <iostream>
using namespace std;

#include "CASDDException.h"

CASDDException::CASDDException(ASDD_SOURCES src, int errCode, string errText)
{
	m_source=src;
	m_errorCode=errCode;
	m_errorText=errText;
}

CASDDException::CASDDException(const CASDDException& orig)
{
	m_source=orig.m_source;
	m_errorCode=orig.m_errorCode;
	m_errorText=orig.m_errorText;
	cout << "Copying the exception object" << endl;
}

ASDD_SOURCES CASDDException::getSource()
{
	return m_source;
}

int CASDDException::getErrorCode()
{
	return m_errorCode;
}

string CASDDException::getErrorText()
{
	return m_errorText;
}

void CASDDException::setErrorText(string txt)
{
	m_errorText=txt;
}


string CASDDException::getSrcAsString()
{
	switch(m_source)
	{
	case SRC_IOWarrior: return "SRC_IOWarrior "; break;
	case SRC_SimpleAudioDevice: return "CSimpleAudioDevice "; break;
	case SRC_File: return "CFile "; break;
	case SRC_Filter: return "CFilter "; break;
	case SRC_Database: return "CDatabase "; break;
	default: return "Unknown Source ";
	}
}

void CASDDException::print()
{
	cout << "Error from [" << getSrcAsString() << "]: " << m_errorText << endl;
}

ostream& operator << (ostream& out, CASDDException& e)
{
	out << "Error from [" << e.getSrcAsString() << "]: " << e.getErrorText();
	return out;
}
