using IotqiDevices.Network.RadioHead;
using System;
using Windows.ApplicationModel.Background;
using Windows.Foundation;
using Windows.System.Threading;

// The Background Application template is documented at http://go.microsoft.com/fwlink/?LinkID=533884&clcid=0x409



namespace LooUQ.iotQi.Apps.WinIoT_UWP
{
    public sealed class StartupTask : IBackgroundTask
    {
        internal static BackgroundTaskDeferral Deferral = null;
        internal static IAsyncAction loraRadioModule;
        internal static IotqiGatewayModule loraRadio;
        private static int txCount = 1;

        public void Run(IBackgroundTaskInstance taskInstance)
        {
            Deferral = taskInstance.GetDeferral();

            IAsyncAction runModule = ThreadPool.RunAsync(
                (workItem) => { RadioModule(); },
                WorkItemPriority.Normal,
                WorkItemOptions.None);

            loraRadioModule = runModule;

            ThreadPoolTimer txTimer = ThreadPoolTimer.CreatePeriodicTimer(GenerateMessageToSend, TimeSpan.FromSeconds(15));
        }


        private static void RadioModule()
        {
            // create radio instance as a gateway module
            loraRadio = new IotqiGatewayModule();

            // subscribe to message ready event, delivery message right here.
            loraRadio.MessageReadyEvent += (sender, msg) => { ProcessModuleMesssage(sender, msg); };

            // start (run) radio receiver 
            loraRadio.RunModule();
            //loraRadio.RunSpiTest();
        }

        private static void ProcessModuleMesssage(object sender, string messageData)
        {
            Console.WriteLine("LoRa Recv: {0}", messageData);
        }

        public void GenerateMessageToSend(ThreadPoolTimer timer)
        {
            /* prefix loraMessage with a ! to perform thread action but skip actual LoRa radio send */
            //string loraMessage = string.Format("!Hello: {0}", txCount);

            string loraMessage = string.Format("Hello Arduino: {0}", txCount);
            txCount++;
            loraRadio.SendToDevice(loraMessage);
        }

    }
}

