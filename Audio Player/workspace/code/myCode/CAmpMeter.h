/*
 * CAmpMeter.h
 */

#ifndef CAMPMETER_H_
#define CAMPMETER_H_

enum SCALING_MODES{SCALING_MODE_LIN,SCALING_MODE_LOG};

class CIOWarrior;


class CAmpMeter
{
private:
	SCALING_MODES m_scmode;				// logarithmic or linear scaling
	float m_scMax;						// maximum of the scale
	float m_thresholds[8];				// thresholds for the bar with 8 segments (LEDs)
	CIOWarrior* m_iowdev;				// address of an object that controls an IOWarrior device

public:
	/**
	 * Constructor
	 * initializes the attributes with initial values (see UML class diagram)
	 */
	CAmpMeter();

	/**
	 * \brief Initializes the amplitude meter.
	 * \param min, max [in] The range of the data to visualize. The maximum absolute value of min and max is taken for the maximum of the scale.
	 * \param scmode [in] The scaling of the amplitude meter (may be SCALING_MODE_LIN or SCALING_MODE_LOG).
	 * \param logScaleMin [in] The minimum of the logarithmic scale (used for logarithmic scaling only, may be set to 0 for linear scaling).
	 * \param iowdev [in] The address of an object that controls an IOWarrior device.
	 */
	void init(float min, float max, SCALING_MODES scmod, int logScaleMin, CIOWarrior* iowdev);

	/**
	 * \brief Visualizes the amplitude of a data buffer on the LED line. Returns true if the visualization was successfully, otherwise false.
	 * \param databuf [in] The address of the first data buffer element.
	 * \param databufsize [in] The number of elements of the data buffer.
	 */
	bool write(float* databuf, unsigned long databufsize);

	/**
	 * \brief Visualizes the amplitude of one single data value on the LED line. Returns true if the visualization was successfully, otherwise false.
	 * \param data [in] The data value.
	 */
	bool write(float data);

	/**
	 * \brief Visualizes the amplitude of a data buffer on the computer screen.
	 * \param databuf [in] The address of the first data buffer element.
	 * \param databufsize [in] The number of elements of the data buffer.
	 */
	void print(float* databuf, unsigned long databufsize);

	/**
	 * \brief Visualizes the amplitude of one single data value on the computer screen.
	 * \param data [in] The data value.
	 */
	void print(float data);

private:
	/**
	 * \brief Returns an appropriate bar pattern for the data value (e.g. returns 0b11111111 if the absolute value of data is equal to the maximum data value).
	 * \brief The bar pattern may be used for visualization on the screen or the LED line.
	 * \param data [in] The data value.
	 */
	unsigned char _getBarPattern(float data);

	/**
	 * \brief Returns an representative amplitude value for the data buffer. This value is taken for the visualization of the buffer.
	 * \param databuf [in] The address of the first data buffer element.
	 * \param databufsize [in] The number of elements of the data buffer.
	 */
	float _getValueFromBuffer(float* databuf, unsigned long databufsize);
};

/////////////////
// uses IOWarriorEx with exception handling
//
class CAmpMeterEx
{
private:
	SCALING_MODES m_scmode;				// logarithmic or linear scaling
	float m_scMax;						// maximum of the scale
	float m_thresholds[8];				// thresholds for the bar with 8 segments (LEDs)
	CIOWarriorEx* m_iowdev;				// address of an object that controls an IOWarrior device

public:
	/**
	 * Constructor
	 * initializes the attributes with initial values (see UML class diagram)
	 */
	CAmpMeterEx();

	/**
	 * \brief Initializes the amplitude meter.
	 * \param min, max [in] The range of the data to visualize. The maximum absolute value of min and max is taken for the maximum of the scale.
	 * \param scmode [in] The scaling of the amplitude meter (may be SCALING_MODE_LIN or SCALING_MODE_LOG).
	 * \param logScaleMin [in] The minimum of the logarithmic scale (used for logarithmic scaling only, may be set to 0 for linear scaling).
	 * \param iowdev [in] The address of an object that controls an IOWarrior device.
	 */
	void init(float min, float max, SCALING_MODES scmod, int logScaleMin, CIOWarriorEx* iowdev);

	/**
	 * \brief Visualizes the amplitude of a data buffer on the LED line. Returns true if the visualization was successfully, otherwise false.
	 * \param databuf [in] The address of the first data buffer element.
	 * \param databufsize [in] The number of elements of the data buffer.
	 */
	bool write(float* databuf, unsigned long databufsize);

	/**
	 * \brief Visualizes the amplitude of one single data value on the LED line. Returns true if the visualization was successfully, otherwise false.
	 * \param data [in] The data value.
	 */
	bool write(float data);

	/**
	 * \brief Visualizes the amplitude of a data buffer on the computer screen.
	 * \param databuf [in] The address of the first data buffer element.
	 * \param databufsize [in] The number of elements of the data buffer.
	 */
	void print(float* databuf, unsigned long databufsize);

	/**
	 * \brief Visualizes the amplitude of one single data value on the computer screen.
	 * \param data [in] The data value.
	 */
	void print(float data);

private:
	/**
	 * \brief Returns an appropriate bar pattern for the data value (e.g. returns 0b11111111 if the absolute value of data is equal to the maximum data value).
	 * \brief The bar pattern may be used for visualization on the screen or the LED line.
	 * \param data [in] The data value.
	 */
	unsigned char _getBarPattern(float data);

	/**
	 * \brief Returns an representative amplitude value for the data buffer. This value is taken for the visualization of the buffer.
	 * \param databuf [in] The address of the first data buffer element.
	 * \param databufsize [in] The number of elements of the data buffer.
	 */
	float _getValueFromBuffer(float* databuf, unsigned long databufsize);
};

#endif /* CAMPMETER_H_ */
