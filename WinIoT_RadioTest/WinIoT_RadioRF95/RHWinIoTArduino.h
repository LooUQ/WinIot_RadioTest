// Project specific WinIoTArduino header file: specifies HW plaform and project specific abstractions (class hierarchy, method signatures, etc.)


/* WinIoTArduino: Arduino emulation for Win10 IoT
 * Copyright 2017 LooUQ Incorporated
 * Licensed AGPL
*/

#pragma once

#include "HW_WinIoTArduino.h"
#define WINIOT_DEVICE WINIOT_DEVICE_RASBERRYPI					

#include "pch.h"
#include "WinIoTArduino.h"

#define DEBUG_MODULE
//#define DEBUG_IRQHANDLER

#define RADIO_SPI_CS_PIN WinIotGPIOPins::GPIO_26
#define RADIO_SPI_INDEX WinIotSpiControllers::Spi_1

/* the RadioHead project utilizes 2 semaphores to gaurd autonomous actions
 * the macros below make implementation easier
 * the tok parameter is only required for debugging output to show the caller
*/
#define RADIOMODE_BLOCK_START(dbgTok) { radioModeSemaphore.acquire(dbgTok); }
#define RADIOMODE_BLOCK_END { radioModeSemaphore.release(); }

/* Create the semaphore class instances
 * one of these is for radio (rf95 chip) mode, the _mode variable and the chip (spi writes) stay atomic and together
 * one is to implement the RadioHead library's ATOMICBLOCK macros (same pattern as above)
*/
// create instances of required WinIoTArduino service classes (if req'd)
extern SemaphoreLock radioModeSemaphore;
extern SemaphoreLock atomicBlockSemaphore;
