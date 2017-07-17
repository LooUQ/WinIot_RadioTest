/* iotQi-RPI-PlatformHelper is a collection of helper methods to support Raspberry PI features when running on Windows 10 IoT Core
 */

//using Loouq.Iotqi.Devices;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Diagnostics;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.System;
using Windows.System.Profile;

namespace LooUQ.iotQi.Apps.WinIoT_UWP
{
    internal class PlatformHelper
    {
        /* The GetConfigConnectionString() method allows for the iotQi connection string to be stored on the SD card in a JSON based config file
         * The file is in the root and must be named iotqi-config.json and the recommended method for placing it there is via the device dashboard
         * accessible via the "Windows 10 IoT Core Dashboard"
         */

        const string _ConfigFileName = "iotqi-config.json";

        internal static async Task<string> GetConfigConnectionStringAsync()
        {
            var connectionString = "";
            try
            {
                StorageFolder appDataLocalFolder = ApplicationData.Current.LocalFolder;
                StorageFile iotqiConfigFile = await appDataLocalFolder.GetFileAsync(_ConfigFileName);
                string fileContent = await FileIO.ReadTextAsync(iotqiConfigFile);

                // parse JSON content and return connectionString
                JObject jsonObj = JObject.Parse(fileContent);
                if (jsonObj["connectionString"] != null)
                {
                    connectionString = jsonObj["connectionString"].Value<string>();
                }
            }
            catch (Exception ex)
            {
                var m = ex.Message;     // gobble exception
            }
            return connectionString;
        }


        internal static async Task<string> GetConfigAsync(string key = null)
        {
            var config = "{}";
            try
            {
                StorageFolder localCacheFolder = ApplicationData.Current.LocalFolder;
                StorageFile iotqiConfigFile = await localCacheFolder.GetFileAsync(_ConfigFileName);
                string fileContent = await FileIO.ReadTextAsync(iotqiConfigFile);

                if (key != null)
                {
                    // parse JSON content and return connectionString
                    JObject jsonObj = JObject.Parse(fileContent);
                    if (jsonObj[key] != null)
                        config = jsonObj[key].ToString(Formatting.None);
                    else
                        config = jsonObj.ToString(Formatting.None);
                }
            }
            catch (Exception ex)
            {
                var m = ex.Message;     // gobble exception
            }
            return config;
        }

        
        /* The SetIotqiDeviceVersion() method allows for platform specific code to set the software version string reported by iotQi to WebAPI callers */

        internal static string GetDeviceVersionInfo()
        {
            return JsonConvert.SerializeObject(new { VersionInfo = "Version=" + GetFamilyVersion() });

        }

        public static string GetFamilyVersion()
        {
            string deviceFamilyVersion = AnalyticsInfo.VersionInfo.DeviceFamilyVersion;
            ulong version = ulong.Parse(deviceFamilyVersion);
            ulong major = (version & 0xFFFF000000000000L) >> 48;
            ulong minor = (version & 0x0000FFFF00000000L) >> 32;
            ulong build = (version & 0x00000000FFFF0000L) >> 16;
            ulong revision = (version & 0x000000000000FFFFL);
            var osVersion = $"{major}.{minor}.{build}.{revision}";
            return osVersion;
        }

        public static string GetMemoryStatistics()
        {
            AppMemoryReport memReport = MemoryManager.GetAppMemoryReport();
            ulong memUsage = MemoryManager.AppMemoryUsage;
            AppMemoryUsageLevel memLevel = MemoryManager.AppMemoryUsageLevel;
            ulong memLimit = MemoryManager.AppMemoryUsageLimit;

            return "";
        }

    }

    /* ------------------------------------------------------------------------------------------------------------------- */

    public sealed class CommandMethodResult
    {
        public CommandMethodResult()
        {
            StatusCode = 200;
            ResultBody = "{}";
        }

        public CommandMethodResult(object bodyValue) : base()
        {
            if (bodyValue.ToString().StartsWith("{"))
            {
                resultBody = bodyValue.ToString();
            }
            else
                resultBody = JsonConvert.SerializeObject(new { result = bodyValue });
        }

        private string resultBody;

        public int StatusCode { get; set; }
        public bool SuccessResult { get { return StatusCode == 200; }  }
        public string ResultBody {
            get { return resultBody; }
            set
            {
                if (value.ToString().StartsWith("{"))
                {
                    resultBody = value.ToString();
                }
                else
                    resultBody = JsonConvert.SerializeObject(new { result = value });
            }
        }
    }


    /* ------------------------------------------------------------------------------------------------------------------- */

    //internal static class Display
    //{
    //    private static _Display _display = new _Display();

    //    public static void Write(string messageText, string messageColor = "Gray")
    //    {
    //        _display.Write(messageText, messageColor);
    //    }


        //private sealed class _Display : IIotqiDisplayWriter
        //{
        //    private IIotqiDisplayWriter _displayWriter = null;

        //    public void Write(string messageText, string messageColor = "Gray")
        //    {
        //        if (Debugger.IsAttached)
        //        {
        //            Debug.WriteLine(messageText);
        //        }
        //        if (_displayWriter != null)
        //        {
        //            _displayWriter.Write(messageText, messageColor);
        //        }

        //    }
        //}
    }


//}
