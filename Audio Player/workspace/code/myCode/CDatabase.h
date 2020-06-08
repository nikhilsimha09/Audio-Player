//////////////////////////
// Database base class - to access databases via the ODBC API

#ifndef CDatabase_H	
#define CDatabase_H

#include <windows.h>
#include <sql.h>
#include <sqlext.h>

#define DB_TEXTLEN 255
enum DB_STATE{DB_S_NOTREADY, DB_S_CONNECTED, DB_S_FETCH}; // states of the database object

class CDatabase
{
protected:
	SQLHENV m_hDBEnv;		// Environment Handle
	SQLHDBC m_hDBC;			// Connection Handle
	SQLHDBC m_hStmt;		// Statement Handle
	SQLLEN 	m_info;			// value for bindcol and fetch
	DB_STATE m_eState;		// current state of the database object
	string m_lasterrmsg;	// last error message
	bool m_bPrint;			// allows printing (true, default) or not (false)

public:
//////////////////////////////////////////////////////////////////////////////
//Methods, usable for any database
	CDatabase();
	~CDatabase();
	
	// opens a database
	// Parameters:
	// pDB ... name of the data source (like registered in the ODBC driver / OS)
	// pUsr ... user name
	// pPwd ... password (if entered at registration)
	bool open(const char* pDB, const char* pUsr=NULL, const char* pPwd=NULL);

	// fetches one record from the database (used after select statements)
	// returns:
	// true .... new data available
	// false ... no more data available or an error occured
	bool fetch();	

	// terminates a database action (select, insert, delete, update, ...)
	// must be called at the end of each database action
	void closeQuery();					
	
	// closes a database
	void close();						

	// returns a string with the current state of the database object and displays it, if printing is allowed
	string showState();

	// returns a string with a message of the last error
	string getSQLErrorMsg();

	// allow printing or not
	void allowPrint(bool bPrint=true);

protected:
	// executes an SQL statement
	// Parameters:
	// sSQLStmt ....... complete statement to be sent to the database
	// errText ........ text to be added to the error message if an error occurs
	// isSelectStmt ... if the statement is a select statement, the action will not be terminated (it has to be terminated outside after fetch)
	bool _executeSQLStmt(string sSQLStmt, string errText, bool isSelectStmt);
	// retrieves the text message of the error in rc
	// Parameters:
	// infotxt ........ text to be added to the error message if an error occurs
	// rc ............. error code
	// hndl ........... handle
	// type ........... type of the handle (may be SQL_HANDLE_ENV, SQL_HANDLE_DBC, SQL_HANDLE_STMT, SQL_HANDLE_DESC)
	void _showSQLError( const char* infotxt, SQLRETURN rc, SQLHANDLE hndl, int type);
	// retrieves the native error code
	// Parameters: see _executeSQLStmt
	int _getNativeErrorCode(SQLRETURN rc, SQLHANDLE hndl, int type);
};	
#endif

