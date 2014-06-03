//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

#include "CaptureFrameGrabber/CaptureFrameGrabber.h"

#include <mutex>
#include <memory>

namespace Media
{
    ref class CaptureFrameGrabber;
}

namespace highgui_xaml
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
    public ref class MainPage sealed
    {
    public:
        MainPage();

    private:
        bool initGrabber(int device, int w, int h);

        Windows::Foundation::IAsyncActionWithProgress<int>^ TaskWithProgressAsync();

        std::vector<int>	listDevices();

        Platform::Agile<Windows::Devices::Enumeration::DeviceInformationCollection> m_devices;

        ::Media::CaptureFrameGrabber^ m_frameGrabber;

        std::vector <int> listDevicesTask();

        bool					bChooseDevice;
        bool 					bVerbose;
        bool                    bFlipImageX;
        std::atomic<bool>       bGrabberInited;
        int						m_deviceID;
        int						attemptFramerate;
        std::atomic<bool>       bIsFrameNew;
        int						width, height;
        int                     bytesPerPixel;
        unsigned long           frameCounter;
        unsigned long           currentFrame;

    };
}
