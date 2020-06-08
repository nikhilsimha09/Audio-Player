/*
 * CFilterDB.h
 */

#ifndef SRC_CFILTERDB_H_
#define SRC_CFILTERDB_H_

#include <string>
using namespace std;
#include "CDatabase.h"

class CFilterDB: public CDatabase {
private:
	char m_filterType[DB_TEXTLEN];		// type of the filter (e.g. lowpass, shelving, delay, ...)
	char m_filterSubType[DB_TEXTLEN];	// NEW: subtype of the filter (e.g. butter, yulewalk, feedback, feedforward, ...)
	char m_filterInfo[DB_TEXTLEN];		// free textual filter description
	int m_order;			// filter order
	int m_tdelay;			// NEW: delay time (echo filters only)
	int m_fs;				// fs (non-echo filters only)
	int m_fid;				// filter ID from Data base
	float* m_b;				// filter numerator coefficients
	float* m_a;				// filter denominator coefficients
	int m_alen;				// number of filter denominator coefficients
	int m_blen;				// number of filter numerator coefficients

public:
	CFilterDB();
	virtual ~CFilterDB();

	/**
	 * \brief record query methods
	 */
		int getOrder();						// filter order
		int getDelay();						// NEW: delay time [ms]
		int getFs();						// sampling frequency
		string getFilterType();				// type of the filter (e.g. lowpass, shelving, delay, ...)
		string getFilterSubType();			// subtype of the filter (e.g. butter, yulewalk, feedback, ...)
		string getFilterInfo();				// free textual filter description
		float* getBCoeffs();				// filter numerator coefficients
		int getNumBCoeffs();				// number of filter numerator coefficients
		float* getACoeffs();				// filter denominator coefficients
		int getNumACoeffs();				// number of filter denominator coefficients
		int getFilterID();					// FilterID from Database

/**
 * \brief Database special query methods
 */
	/**
	 * \brief select all existing filters from the filterview
	 * \param
	 */
		bool deleteFilterType(string type, string subtype);

	/**
	 * \brief select all existing filters from the filterview
	 * \param
	 */
		bool selectAllFilters();

	/**
	 * \brief insert a filter into the database
	 * \param
	 */
		bool insertFilter(string type, string subtype, int fs, int order, int delay_ms, string info, float* b, int blen, float*a, int alen);

	/**
	 * \brief select all filters from the filterview that are appropriate for a given sampling rate fs
	 * \param
	 */
		bool selectFilters(int fs);

	/**
	 * \brief select the number of filters from the filterview that are appropriate for a given sampling rate fs
	 * \param
	 */
		int selectNumFilters(int fs);

	/**
	 * \brief get the filter data (including coefficients) of the filter with the index fid
	 * \param
	 */
		bool selectFilterData(int fid);

private:
	/**
	 * \brief get the number of filter coefficients of the polynom of a filter with the id fid
	 * \param
	 */
		int _selectNumFilterCoefficients(int fid, string polynom);
		bool _selectFilterCoefficients(int fid, string polynom, float* coeffs);
		bool _selectFilter(int fid);
};

#endif /* SRC_CFILTERDB_H_ */
