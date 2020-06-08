/*
 * CASDDException.h
 */

#ifndef CASDDEXCEPTION_H_
#define CASDDEXCEPTION_H_
#include <string>
using namespace std;

enum ASDD_SOURCES{SRC_Unknown, SRC_IOWarrior, SRC_SimpleAudioDevice, SRC_File, SRC_Filter, SRC_Database};

class CASDDException
{
	ASDD_SOURCES m_source;
	int m_errorCode;
	string m_errorText;

public:
	CASDDException(ASDD_SOURCES src=SRC_Unknown, int errCode=-1, string errText="");
	CASDDException(const CASDDException& orig);
	ASDD_SOURCES getSource();
	int getErrorCode();
	string getErrorText();
	void setErrorText(string txt);
	string getSrcAsString();
	void print();
	friend ostream& operator << (ostream& out, CASDDException& e);
};

#endif /* CASDDEXCEPTION_H_ */
