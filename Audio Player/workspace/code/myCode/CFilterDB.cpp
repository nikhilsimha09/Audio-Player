/**
 * CFilterDB.cpp
 */
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

#include "CASDDException.h"
#include "CFilterDB.h"

CFilterDB::CFilterDB() {
	// TODO Auto-generated constructor stub
	m_filterType[0]=0;		// type of the filter (e.g. lowpass, shelving, delay, ...)
	m_filterSubType[0]=0;	// NEW: subtype of the filter (e.g. butter, yulewalk, feedback, feedforward, ...)
	m_filterInfo[0]=0;		// free textual filter description
	m_order=0;			// filter order
	m_tdelay=0;			// NEW: delay time (echo filters only)
	m_fs=0;				// fs (non-echo filters only)
	m_fid=0;				// filter ID from Data base
	m_b=NULL;				// filter numerator coefficients
	m_a=NULL;				// filter denominator coefficients
	m_alen=0;				// number of filter denominator coefficients
	m_blen=0;				// number of filter numerator coefficients
}

CFilterDB::~CFilterDB() {
	// TODO Auto-generated destructor stub
	if(m_b!=NULL) delete[]m_b;
	if(m_a!=NULL) delete[]m_a;
}

int CFilterDB::getOrder() {
	return m_order;
}

int CFilterDB::getFilterID() {
	return m_fid;
}

// NEW
int CFilterDB::getDelay() {
	return m_tdelay;
}

string CFilterDB::getFilterType()
{
	return m_filterType;
}

// NEW
string CFilterDB::getFilterSubType()
{
	return m_filterSubType;
}

string CFilterDB::getFilterInfo()
{
	return m_filterInfo;
}

float* CFilterDB::getBCoeffs()
{
	return m_b;
}

int CFilterDB::getNumBCoeffs()
{
	return m_blen;
}

float* CFilterDB::getACoeffs()
{
	return m_a;
}

int CFilterDB::getFs()
{
	return m_fs;
}

int CFilterDB::getNumACoeffs()
{
	return m_alen;
}

/**
 * \brief Database special query methods
 */

bool CFilterDB::deleteFilterType(string type, string subtype)
{
	if(m_eState!=DB_S_CONNECTED)
		return false;

	string sSQL;
	sSQL="DELETE FROM filtertypes WHERE f_type=\'" + type + "\' AND f_subtype=\'" + subtype + "\';";
	return _executeSQLStmt(sSQL, "deleteFilterType ", false);
}

bool CFilterDB::selectAllFilters()
{
	if(m_eState!=DB_S_CONNECTED)
		return false;

	string sSQL;
	sSQL="SELECT * FROM filterview;";
	if( _executeSQLStmt(sSQL, "selectAllFilters ", true) )
	{
		SQLBindCol( m_hStmt, 1, SQL_C_LONG, &m_fid, 0, &m_info );
		SQLBindCol( m_hStmt, 2, SQL_C_CHAR, m_filterType, DB_TEXTLEN, &m_info );
		SQLBindCol( m_hStmt, 3, SQL_C_CHAR, m_filterSubType, DB_TEXTLEN, &m_info );
		SQLBindCol( m_hStmt, 4, SQL_C_LONG, &m_order, 0, &m_info );
		SQLBindCol( m_hStmt, 5, SQL_C_LONG, &m_fs, 0, &m_info );
		SQLBindCol( m_hStmt, 6, SQL_C_LONG, &m_tdelay, 0, &m_info );
		SQLBindCol( m_hStmt, 7, SQL_C_CHAR, &m_filterInfo, 0, &m_info );
		return true;
	}
	else
		return false;
}

bool CFilterDB::insertFilter(string type, string subtype,
							 int fs, int order, int delay_ms, string info,
							 float* b, int blen, float*a, int alen)
{
	if(m_eState!=DB_S_CONNECTED)
		return false;

	string sSQL;
	int id=0;
	bool bDelOldCoeffs=false;

	// insert type and subtype into filtertypes (because the unique constraint an error will occur if the type/subtype pair already exists)
	sSQL= "INSERT INTO filtertypes (f_type,f_subtype) VALUES (\'" + type + "\',\'" + subtype + "\');";
	_executeSQLStmt(sSQL, "insertFilter: type", false);

	// get ftid
	sSQL= "SELECT ftid FROM filtertypes WHERE f_type=\'" + type + "\' AND f_subtype=\'" + subtype + "\';";
	if( true == _executeSQLStmt(sSQL, "insertFilter: get type id", true))
	{
		SQLBindCol( m_hStmt, 1, SQL_C_LONG, &id, 0, &m_info );
		if(false == fetch())	//Daten holen
		{
			closeQuery();
			return false;
		}
		closeQuery();
	}

	// insert filter into filters (with ftid_fk=ftid, because the unique constraint an error will occur if the filter already exists)
	sSQL= "INSERT INTO filters (ftid_fk,fs_Hz,f_order,delay_ms,f_info) VALUES (" + to_string(id) + "," + to_string(fs) + "," + to_string(order) + "," + to_string(delay_ms) + ",\'" + info + "\');";
	if(m_bPrint)cout << sSQL << endl;
	SQLRETURN ret = SQLExecDirect( m_hStmt, (SQLCHAR*)sSQL.c_str(), SQL_NTS);
	if (!(ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO))
	{
		_showSQLError( "insertFilter: filter", ret, m_hStmt, SQL_HANDLE_STMT);
		// the filter already exists: delete its filter coefficients (no define for Native error codes?)
		if(1062 == _getNativeErrorCode(ret, m_hStmt, SQL_HANDLE_STMT))
			bDelOldCoeffs=true;
	}
	closeQuery();

	// get fid
	sSQL= "SELECT fid FROM filters WHERE ftid_fk=" + to_string(id) + " AND fs_Hz=" + to_string(fs) + " AND f_order=" + to_string(order) + " AND delay_ms=" + to_string(delay_ms) + ";";
	if( true == _executeSQLStmt(sSQL, "insertFilter: get filter id", true))
	{
		SQLBindCol( m_hStmt, 1, SQL_C_LONG, &id, 0, &m_info );
		if(false == fetch())	//Daten holen
		{
			closeQuery();
			return false;
		}
		closeQuery();
	}

	// insert coefficients into filtercoefficients (with fid_fk=fid)
	// delete existing filter coefficients for this filter (to update will cause a lot of programming effort)
	if(bDelOldCoeffs == true)
	{
		sSQL= "DELETE FROM filtercoefficients WHERE fid_fk=" + to_string(id) + ";";
		_executeSQLStmt(sSQL, "insertFilter: delete filter coefficients", false);
	}

	// insert denominator coefficients
	for(int k=0; k < alen;k++)
	{
		sSQL= "INSERT INTO filtercoefficients (fid_fk,coefficient,position,polynom) VALUES (" + to_string(id) + "," + to_string(a[k]) + "," + to_string(k) + "," + "\'DENOMINATOR\');";
		_executeSQLStmt(sSQL, "insertFilter: insert filter coefficients (denominator)", false);
	}

	// insert numerator coefficients
	for(int k=0; k < blen;k++)
	{
		sSQL= "INSERT INTO filtercoefficients (fid_fk,coefficient,position,polynom) VALUES (" + to_string(id) + "," + to_string(b[k]) + "," + to_string(k) + "," + "\'NUMERATOR\');";
		_executeSQLStmt(sSQL, "insertFilter: insert filter coefficients (numerator)", false);
	}
	return true;
}

bool CFilterDB::selectFilters(int fs)
{
	if(m_eState!=DB_S_CONNECTED)
		return false;

	string sSQL;
	sSQL= "SELECT FilterID, Type, Subtype, Filterorder, Fs, Delay, Description FROM filterview WHERE Fs=0 OR Fs=" + to_string(fs);
	if( true == _executeSQLStmt(sSQL, "selectFilters for given fs", true))
	{
		SQLBindCol( m_hStmt, 1, SQL_C_LONG, &m_fid, 0, &m_info );
		SQLBindCol( m_hStmt, 2, SQL_C_CHAR, m_filterType, DB_TEXTLEN, &m_info );
		SQLBindCol( m_hStmt, 3, SQL_C_CHAR, m_filterSubType, DB_TEXTLEN, &m_info );
		SQLBindCol( m_hStmt, 4, SQL_C_LONG, &m_order, 0, &m_info );
		SQLBindCol( m_hStmt, 5, SQL_C_LONG, &m_fs, 0, &m_info );
		SQLBindCol( m_hStmt, 6, SQL_C_LONG, &m_tdelay, 0, &m_info );
		SQLBindCol( m_hStmt, 7, SQL_C_CHAR, m_filterInfo, DB_TEXTLEN, &m_info );
		return true;
	}
	else
		return false;
}

int CFilterDB::selectNumFilters(int fs)
{
	if(m_eState!=DB_S_CONNECTED)
		return false;

	int fltnum=-1;
	string sSQL;
	sSQL= "SELECT count(*) FROM filterview WHERE Fs=0 OR Fs=" + to_string(fs);
	if( true == _executeSQLStmt(sSQL, "selectFilters for given fs", true))
	{
		SQLBindCol( m_hStmt, 1, SQL_C_LONG, &fltnum, 0, &m_info );
		bool bfetch=fetch(); // get the number of available sounds
		closeQuery();
		return bfetch ? fltnum : -1;
	}
	else
		return -1;
}

bool CFilterDB::selectFilterData(int fid)
{
	if(m_eState!=DB_S_CONNECTED)
		return false;

	if(false == _selectFilter(fid))
		return false;

	if(m_b!=NULL) delete[]m_b;
	if(m_a!=NULL) delete[]m_a;

	m_alen=_selectNumFilterCoefficients(fid, "DENOMINATOR");
	m_blen=_selectNumFilterCoefficients(fid, "NUMERATOR");

	if( (m_alen >= 0) && (m_blen >= 0))
	{
		m_a=new float[m_alen];
		m_b=new float[m_blen];

		bool ba=_selectFilterCoefficients(fid, "DENOMINATOR", m_a);
		bool bb=_selectFilterCoefficients(fid, "NUMERATOR", m_b);

		if(ba && bb)
			return true;
	}

	m_b=NULL;
	m_a=NULL;
	m_alen=0;
	m_blen=0;
	return false;
}

bool CFilterDB::_selectFilter(int fid)
{
	string sSQL;
	sSQL= "SELECT FilterID, Type, Subtype, Filterorder, Fs, Delay, Description FROM filterview WHERE FilterID=" + to_string(fid);
	if( true == _executeSQLStmt(sSQL, "_selectFilter data for a given filter id", true))
	{
		SQLBindCol( m_hStmt, 1, SQL_C_LONG, &m_fid, 0, &m_info );
		SQLBindCol( m_hStmt, 2, SQL_C_CHAR, m_filterType, DB_TEXTLEN, &m_info );
		SQLBindCol( m_hStmt, 3, SQL_C_CHAR, m_filterSubType, DB_TEXTLEN, &m_info );
		SQLBindCol( m_hStmt, 4, SQL_C_LONG, &m_order, 0, &m_info );
		SQLBindCol( m_hStmt, 5, SQL_C_LONG, &m_fs, 0, &m_info );
		SQLBindCol( m_hStmt, 6, SQL_C_LONG, &m_tdelay, 0, &m_info );
		SQLBindCol( m_hStmt, 7, SQL_C_CHAR, m_filterInfo, DB_TEXTLEN, &m_info );
		bool bret= fetch();
		closeQuery();
		return bret;
	}
	else
		return false;
}

int CFilterDB::_selectNumFilterCoefficients(int fid, string polynom)
{
	if(m_eState!=DB_S_CONNECTED)
		return false;

	string sSQL;
	int cnt=-1;

	sSQL= "SELECT count(*) FROM filtercoefficients WHERE fid_fk=" + to_string(fid) + " AND polynom=\'" + polynom + "\'";
	if( true == _executeSQLStmt(sSQL, "_selectNumFilterCoefficients for a given filter id and polynom", true))
	{
		SQLBindCol( m_hStmt, 1, SQL_C_LONG, &cnt, 0, &m_info );
		fetch();
		closeQuery();
	}
	return cnt;
}

bool CFilterDB::_selectFilterCoefficients(int fid, string polynom, float* coeffs)
{
	string sSQL;
	float coeff;
	int i=0;

	sSQL= "SELECT coefficient FROM filtercoefficients WHERE fid_fk=" + to_string(fid) + " AND polynom=\'" + polynom + "\'";
	if( true == _executeSQLStmt(sSQL, "_selectFilterCoefficients for a given filter id and polynom", true))
	{
		SQLBindCol( m_hStmt, 1, SQL_C_FLOAT, &coeff, 0, &m_info );
		while(true==fetch())
		{
			coeffs[i]=coeff;
			i++;
		}
		closeQuery();
		return true;
	}
	else
		return false;
}

