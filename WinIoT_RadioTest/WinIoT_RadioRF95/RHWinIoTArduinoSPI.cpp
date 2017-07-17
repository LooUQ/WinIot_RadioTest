/// This concrete subclass of the RadioHead GenericSPIClass that supports the Raspberry PI running Windows 10 IoT Core
/// RadioHead is // Copyright (C) 2014 Mike McCauley, to use RadioHead ensure you review and comply with their licensing

/*  Copyright 2017 LooUQ Incorporated.
 *  Licensed under AGPLv3
 */

/* This is the project specific SPI driver implemenation for the LooUQ RadioHead port, using the 
 * LooUQ WinIotArduinoSPI library.
 * The RHWinIotArduionoSPI library invokes WinIotArduinoSPI methods to perform SPI I/O and creates a new
 * project specific method to translate the RH SPI frequency enum into an int for the WinIotArduinoSPI library.
*/

#include "pch.h"
#include "RHWinIoTArduinoSPI.h"


RHWinIotArduinoSPI::RHWinIotArduinoSPI(int spiCtrlIndx, RHGenericSPI::Frequency frequency, RHGenericSPI::BitOrder bitOrder, RHGenericSPI::DataMode dataMode):
	RHGenericSPI(frequency, bitOrder, dataMode)
{
	/* Base class default constructor
	_frequency = frequency;
	_bitOrder = bitOrder;			// ignored by Win Iot (always sends MSB first)
	_dataMode = dataMode;
	*/

	int clockFrequency = getFrequencyAsInt();
	_winIotSpi = new WinIotArduinoSPI(spiCtrlIndx, (SpiMode)_dataMode, SpiBitOrder::MSBFIRST, clockFrequency);
}

void RHWinIotArduinoSPI::begin()		// required for RHGenericSPI base
{
	_winIotSpi->begin();
}

void RHWinIotArduinoSPI::end()			// required for RHGenericSPI base
{
	_winIotSpi->end();
}


uint8_t RHWinIotArduinoSPI::transfer(uint8_t data)
{
	return _winIotSpi->transfer(data);
}


int RHWinIotArduinoSPI::getFrequencyAsInt()
{
	int intFreq = 1000000;

	switch (_frequency)
	{
	case RHGenericSPI::Frequency::Frequency2MHz:
		intFreq = 2000000;
		break;
	case RHGenericSPI::Frequency::Frequency4MHz:
		intFreq = 4000000;
		break;
	case RHGenericSPI::Frequency::Frequency8MHz:
		intFreq = 8000000;
		break;
	case RHGenericSPI::Frequency::Frequency16MHz:
		intFreq = 16000000;
		break;
	}
	return intFreq;
}

