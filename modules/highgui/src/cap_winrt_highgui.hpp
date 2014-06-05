// Highgui for OpenCV to XAML
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

#include <ppl.h>
#include <ppltasks.h>
#include <concrt.h>

#include <mutex>
#include <memory>
#include <atomic>
// #include <future>
// #include <condition_variable>

#include <agile.h>

// #include "cap_winrt/CaptureFrameGrabber.h"

#if 0
// this is an OpenCV class
// stub
namespace cv
{
    class Mat
    {
    };
}
#endif

enum {
    HighguiBridge_OPEN_CAMERA = 300,
    HighguiBridge_CLOSE_CAMERA,
    HighguiBridge_UPDATE_IMAGE_ELEMENT,
    HighGuiAssist_SHOW_TRACKBAR
};


// class HighguiBridge is needed because the interface for 
// VideoCapture_WinRT in cap_winrt.hpp is fixed by OpenCV
// (or can it be extended?)

// singleton
class HighguiBridge
{
public:
    // common methods for all DLLs

    __declspec(dllexport) static HighguiBridge& get();

    // called from XAML task (UI thread)
    // __declspec(dllexport) void processOnUIthread(int action);

    // call after initialization
    void setReporter(Concurrency::progress_reporter<int> pr) { reporter = pr; }

    // to be called from cvMain via cap_winrt on bg thread - non-blocking (async)
    void requestForUIthreadAsync(int action);
    
    //void waitForUIthreadRequest();

    // highgui video interface
    // bool initializeDevice();

    // highgui UI interface
    void createTrackbar( /* callbackOnChange */) {} // unhides trackbar and registers OpenCV callback
    void setTrackbarPos(int pos) {}                 // unhides trackbar and sets its position

    void imshow(/*cv::Mat matToShow*/) {}                   // shows Mat in the cvImage element
    // namedWindow                                  // no op: only one XAML window used
    // destroyWindow                                // no op
    // destroyAllWindows                            // no op
    void MoveWindow( /* int x, int y */) {}         // moves the cvImage element on MainPage
    void ResizeWindow( /* int w, int h */) {}       // resizes the cvImage element on MainPage

    void setMouseCallback( /* callback, userdata */) {}
    // calls OpenCV callback when mouse moves inside of cvImage element on MainPage

    int waitKey(int delay = 0) { return 0; }

    // setOpenGlDrawCallback                        // no op
    // setOpenGlContext                             // no op
    // updateWindow                                 // no op

    // end highgui UI interface

    // MediaCapture ^capture;
    int deviceIndex;
    int width, height;

    unsigned long           frameCounter;

    std::atomic<bool>       bIsFrameNew;

    // for blocking
    //std::mutex              frameReadyMutex;
    //std::condition_variable frameReadyEvent;

    // double buffering
    std::mutex              inputBufferMutex;
    std::mutex              outputBufferMutex;

    std::unique_ptr<Windows::UI::Xaml::Media::Imaging::WriteableBitmap^>   m_frontInputBuffer;
    std::unique_ptr<Windows::UI::Xaml::Media::Imaging::WriteableBitmap^>   m_backInputBuffer;
    std::unique_ptr<Windows::UI::Xaml::Media::Imaging::WriteableBitmap^>   m_frontOutputBuffer;
    std::unique_ptr<Windows::UI::Xaml::Media::Imaging::WriteableBitmap^>   m_backOutputBuffer;

    void SwapInputBuffers() {}
    void SwapOutputBuffers() {}

private:

    //Platform::Agile<Windows::Media::Capture::MediaCapture> m_capture;
    //Platform::Agile<Windows::Devices::Enumeration::DeviceInformationCollection> m_devices;

    // to solve linker error, CaptureFrameGrabber cannot be a member of this class
    // ::Media::CaptureFrameGrabber^ m_frameGrabber;
    //void GrabFrameAsync(Media::CaptureFrameGrabber^ frameGrabber);

    HighguiBridge() {
        deviceIndex = 0;
        width = 640;
        height = 480;
        UIthreadTaskDone = false;
        deviceReady = false;
        bIsFrameNew = false;
    };

    // bool initializeDeviceTask();

    std::atomic<bool>       deviceReady;

    // singleton
    HighguiBridge(HighguiBridge const &);
    void operator=(const HighguiBridge &);

    Concurrency::progress_reporter<int> reporter;

    std::atomic<bool> UIthreadTaskDone;
};
