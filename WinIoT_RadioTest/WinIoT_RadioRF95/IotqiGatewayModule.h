#pragma once

using namespace Platform;

namespace IotqiDevices
{
	namespace Network
	{
		namespace RadioHead 
		{
			public delegate void MessageReadyEventHandler(Platform::Object^, Platform::String^);

			public ref class IotqiGatewayModule sealed
			{
			public:
				IotqiGatewayModule();

				void RunModule();
				void SendToDevice(Platform::String^ message);
				void RunSpiTest();
				event MessageReadyEventHandler^ MessageReadyEvent;
			};
		}
	}
}
