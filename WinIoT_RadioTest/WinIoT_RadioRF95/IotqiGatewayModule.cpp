#include "pch.h"
#include "IotqiGatewayModule.h"

using namespace IotqiDevices::Network::RadioHead;

#include "RH_RF95.h"
#include "RHReliableDatagram.h"
#include "RHWinIoTArduinoSPI.h"

// support wide/narrow char conversion
#include <cvt/wstring>
#include <codecvt>

#define RADIO_SPI_CTRLINDX WinIotSpiControllers::SPI_1
#define RADIO_CS_GPIONUM WinIotGPIOPins::GPIO_26
#define RADIO_IRQ_GPIONUM WinIotGPIOPins::GPIO_5

#define RADIO_THIS_ADDRESS 10
#define RADIO_DEVICE_ADDRESS 1
#define RADIO_BROADCAST_ADDRESS 255

RHWinIotArduinoSPI* rhWinIotSPI;
RH_RF95* radio = NULL;
//RHDatagram* manager = NULL;

uint8_t recvBuffer[251];
uint8_t sendBuffer[251];
uint8_t bytesToSend = 0;
bool modeOperating = false;

SemaphoreLock radioModeSemaphore = SemaphoreLock(1, 'R', "radioMode");
SemaphoreLock atomicBlockSemaphore = SemaphoreLock(1, 'A', "atomic");


IotqiGatewayModule::IotqiGatewayModule()
{
	rhWinIotSPI = new RHWinIotArduinoSPI(RADIO_SPI_CTRLINDX, RHGenericSPI::Frequency8MHz, RHGenericSPI::BitOrderMSBFirst, RHGenericSPI::DataMode3);

	//rhWinIotSPI = new RHWinIotArduinoSPI(0, RHGenericSPI::Frequency1MHz, RHGenericSPI::BitOrderMSBFirst, RHGenericSPI::DataMode3);
	
	radio = new RH_RF95(RADIO_CS_GPIONUM, RADIO_IRQ_GPIONUM, *rhWinIotSPI);
	//manager = new RHDatagram(*radio, RADIO_THIS_ADDRESS);

	Serial.println("Initializing RF95 radio modem...");
	int attempts = 0;
	while (!radio->init() && attempts < 3)
	{
		attempts++;
		Serial.println("***Initialization of RF95 radio modem failed");
		if (attempts >= 3)
		{
			Serial.println("Max initializaiton attempts, module failed.");
			while (true);
		}
	}
	radio->setFrequency(915.0);
	//radio->setTxPower(23);
	Serial.println("RF95 modem initialized.");
}


void IotqiDevices::Network::RadioHead::IotqiGatewayModule::RunModule()
{
	#define recvPriorityTimeout 2000

	Serial.println("\r\nStarting radio loop...");
	uint16_t rxCount = 0;
	uint16_t txCount = 0;

	while (true)
	{
		Serial.print('.');

		/* Using the 2 nested loops, you can prioritize duty cycle of radio for receive or transmit
		   Outer loop keeps radio running,
		   Inner loop prioritizes receives, waiting for incoming packets (recv), occasionally exiting loop to send
		*/

		unsigned long startRecv = millis();
		while ((millis() - startRecv) < recvPriorityTimeout)
		{
			if (radio->available())
			{
				uint8_t len = sizeof(recvBuffer);
				if (radio->recv(recvBuffer, &len))
				{
					rxCount++;

					#ifdef DEBUG_MODULE
					char printBuffer[300];
					//snprintf(printBuffer, sizeof(printBuffer), "\r\nRecv(#%d) from %d: bytes=%d msg=%.*s", rxCount, manager->headerFrom(), len, len, recvBuffer);
					snprintf(printBuffer, sizeof(printBuffer), "\r\nRecv(#%d) from %d: bytes=%d msg=%.*s", rxCount, 0, len, len, recvBuffer);
					Serial.println(printBuffer);
					#endif

					if (len > 0)
					{
						std::string recvBuffer_str = std::string((char*)recvBuffer, len);
						std::wstring recvBuffer_wstr = std::wstring(recvBuffer_str.begin(), recvBuffer_str.end());
						Platform::String^ recvBuffer_platform = ref new Platform::String(recvBuffer_wstr.c_str());
						MessageReadyEvent(this, recvBuffer_platform);
					}
				}
			}
		}
	

		if (bytesToSend > 0)
		{
			//if (radio->sendto(sendBuffer, bytesToSend, RADIO_DEVICE_ADDRESS))
			if (radio->send(sendBuffer, bytesToSend))
			{
				txCount++;

				#ifdef DEBUG_MODULE
				Serial.printf("  :Sent %d Bytes\r\n", bytesToSend);
				#endif
			}
			else
			{
				#ifdef DEBUG_MODULE
				Serial.println("  :SEND FAILED***");
				#endif
			}
			bytesToSend = 0;
			memset(sendBuffer, 0, sizeof(sendBuffer));
		}

		/* The line below with delay() below would be bad and would break receive functionality.
		   The radio needs to be in receive mode as much as possible, the duty cycle should be:
			- in receive mode continuously (awaiting incoming packets),
			- check for send pending (if so, transmit it),
			- then immediately get back in to receive mode.

			delay(5000);
		*/
	}
}


void IotqiDevices::Network::RadioHead::IotqiGatewayModule::SendToDevice(Platform::String^ platformMessage)
{
	stdext::cvt::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string stringUtf8Message = converter.to_bytes(platformMessage->Data());

	// debugging jig: allow send to "thread in" without actual radio send
	if (stringUtf8Message[0] == '!')
	{
		Serial.printf("\r\nSkipping Send (!) for %.*s \r\n", platformMessage->Length(), stringUtf8Message.c_str());
	}
	else
	{
		bytesToSend = platformMessage->Length();
		memcpy(sendBuffer, stringUtf8Message.c_str(), bytesToSend);
		#ifdef DEBUG_MODULE
		Serial.printf("\r\nQueued %d bytes for SEND: %.*s", bytesToSend, bytesToSend, sendBuffer);
		#endif
	}
}

void IotqiDevices::Network::RadioHead::IotqiGatewayModule::RunSpiTest()
{
	int i = 0;
	while (true)
	{
		i++;
		uint8_t statusReg = radio->spiTestOnce(1);
		delay(10);
		if (i % 10 == 0)
			Serial.print('.');
	}
}



