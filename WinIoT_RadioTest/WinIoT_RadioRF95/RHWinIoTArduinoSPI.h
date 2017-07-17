/////////////////////////////////////////////////////////////////////
/// \class RHHardwareSPI RHHardwareSPI.h <RHHardwareSPI.h>
/// \brief Encapsulate a hardware SPI bus interface
///
/// This concrete subclass of the RadioHead GenericSPIClass that supports the Raspberry PI running Windows 10 IoT Core
/// RadioHead is // Copyright (C) 2014 Mike McCauley, to use RadioHead ensure you review and comply with their licensing

/*  Copyright 2017 LooUQ Incorporated.
 *  Licensed under AGPLv3
 */


/* This is the project specific SPI driver for the LooUQ RadioHead port, using the LooUQ WinIotArduinoSPI library
 * The RHWinIotArduionoSPI implements from the RHGenericSPI base class using the WinIotArduinoSPI library
 * for actual work completed.  It maintains a private instance of the WinIotArduinoSPI class which is used
 * for the actual SPI I/O.  Additionally the WinIotArduinoSPI class expects an integer for SPI bus frequency
 * RadioHead supplies an enum, this is translated by the RHWinIotArduinoSPI specific getFrequencyAsInt() method.
*/

#pragma once


#include "pch.h"
#include "RHGenericSPI.h"
#include "WinIotArduinoSpi.h"


class RHGenericSPI;

class RHWinIotArduinoSPI : public RHGenericSPI
{
public:
	// default constructor from based class RHGenericSPI
	// RHGenericSPI(Frequency frequency = Frequency1MHz, BitOrder bitOrder = BitOrderMSBFirst, DataMode dataMode = DataMode0);

	RHWinIotArduinoSPI(int spiCtrlIndx = DEFAULT_SPI_CTRLINDX, RHGenericSPI::Frequency frequency = Frequency1MHz, RHGenericSPI::BitOrder bitOrder = BitOrderMSBFirst, RHGenericSPI::DataMode dataMode = DataMode0);

	void begin();
	void end();

	uint8_t transfer(uint8_t data);

private:
	WinIotArduinoSPI* _winIotSpi;
	int getFrequencyAsInt();
};

