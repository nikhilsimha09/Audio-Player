/*
 * CIOWarrior.h
 */

/**
 * \file CIOWarrior.h
 *
 * \brief header file of the IOWarrior control class (performs output only)
 */
#ifndef CIOWARRIOR_H_
#define CIOWARRIOR_H_

#include <wtypes.h>
#include "iowkit.h"

/**
 * \brief Error Codes
 */
enum IOW_ERRORS{IOW_E_OK,IOW_E_NODEVICE,IOW_E_INVALIDDEVICE,IOW_E_WRITEERROR,IOW_E_DEVICENOTREADY};
/**
 * \brief Device States (used to implement a small state machine)
 */
enum IOW_STATES{IOW_S_NOTREADY,IOW_S_READY};

/**
 * \brief IOWarrior control class, designed to control one single device only
 */
class CIOWarrior
{
	/**
	 * handle of the device (NULL if no device is connected at program start)
	 */
	IOWKIT_HANDLE m_handle;
	/**
	 * saves the last error occurred (IOW_E_OK if no error occurred)
	 */
	IOW_ERRORS m_lastError;
	/**
	 * saves the current state of the device control (IOW_S_READY if device is connected)
	 */
	IOW_STATES m_state;

public:
	/**
	 * Constructor
	 * initializes the attributes with initial values
	 */
	CIOWarrior();

	/**
	 * Destructor
	 * closes the device
	 */
	virtual ~CIOWarrior();

	/**
	 * \brief Opens an IOWarrior device.
	 * \brief Returns true if the device is ready to use.
	 */
	bool open();

	/**
	 * \brief Closes an IOWarrior device.
	 */
	void close();

	/**
	 * \brief Writes a byte to IOWarriors port 3 with LEDs connected. Returs true if the byte was written successfully.
	 * \param data [in] 8-bit pattern to show on LEDs (1-bit switches the LED on, 0-bit switches the LED off)
	 */
	bool write(unsigned char data);

	/**
	 * \brief Prep 1: Looks for a pressed button and returns immediately. Returns true if the button had been pressed, otherwise false.
	 */
	bool keyPressed();

	/**
	 * \brief Prints a binary formatted byte on the screen.
	 * \param data [in] 8-bit pattern to show on LEDs (1-bit switches the LED on, 0-bit switches the LED off)
	 */
	void printData(unsigned char data);

	/**
	 * \brief Prints the current state of the IOWarrior device control.
	 */
	void printState();

	/**
	 * \brief Queries the current state of the IOWarrior device control.
	 */
	IOW_STATES getState();
};

/////////////////////////////
// IOWarrior control with Exception handling
////////////////////////////
/**
 * \brief IOWarrior control class, designed to control one single device only
 */
class CIOWarriorEx
{
	/**
	 * handle of the device (NULL if no device is connected at program start)
	 */
	IOWKIT_HANDLE m_handle;
	/**
	 * saves the current state of the device control (IOW_S_READY if device is connected)
	 */
	IOW_STATES m_state;

public:
	/**
	 * Constructor
	 * initializes the attributes with initial values
	 */
	CIOWarriorEx();

	/**
	 * Destructor
	 * closes the device
	 */
	virtual ~CIOWarriorEx();

	/**
	 * \brief Opens an IOWarrior device.
	 * \brief Returns true if the device is ready to use.
	 */
	void open();

	/**
	 * \brief Closes an IOWarrior device.
	 */
	void close();

	/**
	 * \brief Writes a byte to IOWarriors port 3 with LEDs connected. Returs true if the byte was written successfully.
	 * \param data [in] 8-bit pattern to show on LEDs (1-bit switches the LED on, 0-bit switches the LED off)
	 */
	void write(unsigned char data);

	/**
	 * \brief Prep 1: Looks for a pressed button and returns immediately. Returns true if the button had been pressed, otherwise false.
	 */
	bool keyPressed();

	/**
	 * \brief Prints a binary formatted byte on the screen.
	 * \param data [in] 8-bit pattern to show on LEDs (1-bit switches the LED on, 0-bit switches the LED off)
	 */
	void printData(unsigned char data);

	/**
	 * \brief Prints the current state of the IOWarrior device control.
	 */
	void printState();

	/**
	 * \brief Queries the current state of the IOWarrior device control.
	 */
	IOW_STATES getState();

	/**
	 * \brief Queries the current state of the IOWarrior device control.
	 */
	const char* getIOWErrorText(IOW_ERRORS err);		// query of the last error (readable text)

};

#endif /* CIOWARRIOR_H_ */
