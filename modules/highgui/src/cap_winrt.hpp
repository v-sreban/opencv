// Capture support for WinRT
// Microsoft Surface and Phone using Media Foundation

// Copyright (c) 2013, Microsoft Open Technologies, Inc. 
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice, 
//   this list of conditions and the following disclaimer in the documentation 
//   and/or other materials provided with the distribution.
// - Neither the name of Microsoft Open Technologies, Inc. nor the names 
//   of its contributors may be used to endorse or promote products derived 
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include "precomp.hpp"

#include <collection.h>
#include <ppltasks.h>

#include <mutex>
#include <condition_variable>
#include <atomic>

#include <cap_winrt/CaptureFrameGrabber.h>

#include <opencv2/highgui/cdebug.h>

// for using MF:
using namespace concurrency;
using namespace Microsoft::WRL;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Media::Capture;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::Devices::Enumeration;


//// pull in MF libs (this has to be somewhere in the project)
//#pragma comment(lib, "mfplat")
//#pragma comment(lib, "mf")
//#pragma comment(lib, "mfuuid")
//#pragma comment(lib, "Shlwapi")


// implement the newer IVideoCapture so that we can work
// directly with Mat, not the cv interface which has added overhead

namespace cv {

    class VideoCapture_WinRT : public IVideoCapture
    {
    public:
        VideoCapture_WinRT() :
            started(false),
            bytesPerPixel(0),
            frameCounter(0),
            frameCurrent(0),
            deviceID(0)
        {}

        VideoCapture_WinRT(int device);

        virtual ~VideoCapture_WinRT()
        {
            // close();
        }

        // from base class IVideoCapture
        virtual double getProperty(int) { return 0; }
        virtual bool setProperty(int, double);
        virtual bool grabFrame();
        virtual bool retrieveFrame(int, cv::OutputArray);

        // Return the type of the capture object
        virtual int getCaptureDomain() { return CAP_WINRT; }

        // not part of IVideoCapture, but could be called directly
        // static std::vector <std::string&> listDevices();
        static void listDevices();

    protected:

        int deviceID;

        // double buffering
        std::mutex              bufferMutex;
        std::unique_ptr<Windows::UI::Xaml::Media::Imaging::WriteableBitmap^>   m_frontBuffer;
        std::unique_ptr<Windows::UI::Xaml::Media::Imaging::WriteableBitmap^>   m_backBuffer;
        
        void SwapBuffers();

        void    start();
        void GrabFrameAsync(::Media::CaptureFrameGrabber^ frameGrabber);
        Platform::Agile<::Windows::Media::Capture::MediaCapture> m_capture;

        CvSize                  size;
        std::atomic<bool>       started;
        int                     bytesPerPixel;
        unsigned long           frameCounter;
        unsigned long           frameCurrent;
        // std::atomic<bool>       isFrameNew;

        std::mutex              frameReadyMutex;
        std::condition_variable frameReadyEvent;
    };
}
