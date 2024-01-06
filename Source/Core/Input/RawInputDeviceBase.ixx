/*
    Description:
    The interface of RawInputDeviceBase module

    暂时不考虑其他OS平台, 比如Linux或Mac一类的，因为只看了Windows的Raw Input机制, 只针对该机制进行抽象，
    其他的等以后了，目前没这个需求，Don't overcomplicate things you don't need

    Created Date:
    1/6/2024 12:31:09 PM
*/


export module RawInputDeviceBase;

import <Windows.h>;
//export import :Mouse;

namespace Input
{
    // Abstract base class for different input devices
    export class RawInputDevice
    {
        

    public:
        virtual bool SetupDevice();


        static void RegisterDevices()
        {
            RAWINPUTDEVICE Rid[2];

            Rid[0].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
            Rid[0].usUsage = 0x02;              // HID_USAGE_GENERIC_MOUSE
            Rid[0].dwFlags = RIDEV_NOLEGACY;    // adds mouse and also ignores legacy mouse messages
            Rid[0].hwndTarget = 0;

            Rid[1].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
            Rid[1].usUsage = 0x06;              // HID_USAGE_GENERIC_KEYBOARD
            Rid[1].dwFlags = RIDEV_NOLEGACY;    // adds keyboard and also ignores legacy keyboard messages
            Rid[1].hwndTarget = 0;

            if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE)
            {
                //registration failed. Call GetLastError for the cause of the error
            }

            //UINT numDevices;
            //GetRegisteredRawInputDevices(0, &numDevices, sizeof(RAWINPUTDEVICE));
            //return static_cast<int>(numDevices);
        }

    protected:
        RAWINPUTDEVICE device;
    };
}