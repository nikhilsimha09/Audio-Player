/*
 * CFilter.h
 */

/**
 * \file CFilter.h
 *
 * \brief header file for filter class
 *
 * \see CFilter
 */
#ifndef CFILTER_H_
#define CFILTER_H_

/**
 * \brief General filter class to calculate digital filter output via direct form II transposed
 * \brief implements operation filter, handles errors
 */
class CFilter
{
private:
	/**
	 * \brief numerator filter coefficients
	 */
	float* m_b;
	/**
	 * \brief denominator filter coefficients
	 */
	float* m_a;
	/**
	 * \brief intermediate states from last sample or circular buffer (optimized delay filters)
	 */
	float* m_z;
	/**
	 * \brief filter order
	 */
	int m_order;
	/**
	 * \brief number of channels of the signals to be filtered
	 */
	int m_channels;
	/**
	 * \brief first element to read and write after block has been changed (for optimized delay filters circular buffer only)
	 */
	int m_firstZ;
	/**
	 * \brief first element to read and write after block has been changed (for optimized delay filters circular buffer only)
	 */
	int m_bDelay;

public:
	/**
	 * \brief Creates a filter object.
	 * \param ca, cb [in] pointers to the arrays of denominator and numerator filter coefficients
	 * \param order [in] filter order
	 * \param delay_ms [in] delay time in ms
	 * \param fs [in] sampling frequency of the signal to be filtered  (input signal)
	 * \param channels [in] number of channels of the signals to be filtered  (input signal)
	 */
	CFilter(float* ca, float* cb, int  order, int delay_ms, int fs, int channels=2);
	~CFilter();
	/**
	 * \brief Filters a signal in dependence of the type (delay or not)
	 * \param x [in] pointer to the signal to be filtered (input signal)
	 * \param y [in] pointer to a buffer to store the filtered signal (output signal)
	 * \param framesPerBuffer [in] number of frames to be filtered (the signal length resp. buffer size of x and y must be framesPerBuffer*channels)
	 */
	bool filter(float* x, float* y, int framesPerBuffer);	// straight forward difference equation

	/**
	 * \brief Clears all intermediate values. May be used before filtering a new signal by the same filter object.
	 */
	void reset();

	/**
	 * \brief Getter for filter order.
	 */
	int getOrder();

private:
	/**
	 * \brief Filters a signal.
	 * \param x [in] pointer to the signal to be filtered (input signal)
	 * \param y [in] pointer to a buffer to store the filtered signal (output signal)
	 * \param framesPerBuffer [in] number of frames to be filtered (the signal length resp. buffer size of x and y must be framesPerBuffer*channels)
	 */
	bool _filter(float* x, float* y, int framesPerBuffer, int bufsize);
	/**
	 * \brief to be implemented in task 2 - optimized delay filters
	 * \brief Filters a signal (delay filter).
	 * \param x [in] pointer to the signal to be filtered (input signal)
	 * \param y [in] pointer to a buffer to store the filtered signal (output signal)
	 * \param framesPerBuffer [in] number of frames to be filtered (the signal length resp. buffer size of x and y must be framesPerBuffer*channels)
	 */
	bool _filterDelay(float* x, float* y, int framesPerBuffer, int bufsize);
};

#endif /* CFILTER_H_ */
