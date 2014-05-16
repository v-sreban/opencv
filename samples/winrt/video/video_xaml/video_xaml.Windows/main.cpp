// main.cpp

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

// for XAML
#include "pch.h"
#include "App.xaml.h"

// for OpenCV:
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>

// #include "opencv2/core/core_c.h"
// #include "opencv2/imgproc/imgproc_c.h"
// #include <opencv2/highgui/highgui_c.h>

#include <opencv2/highgui/cdebug.h>

#include <thread>
#include <chrono>
#include <atomic>

using namespace cv;


VideoCapture cam;


// __declspec(dllimport) std::atomic<bool> startProcessing;

void process();

// nb. t1 must exist as long as the app exists
// should do a join somewhere ...
static std::thread t1{ process };

void process()
{
    // TCC("process thread init"); TCNL;

    //// use condition var instead with wait()
    //while (!startProcessing)
    //{
    //    // wait 100 ms
    //    std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(100));
    //}

    // for testing only
    TCC("process thread running");
    TC(t1.get_id());
    TC(t1.native_handle());
    TCNL;

    // for testing only
    std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(1000));

    // Mat edges;
    // namedWindow("edges", 1);

    Mat frame;

    // process frames
    while (1)
    {
        // get a new frame from camera
        // this will block until the device is initialized and a frame is available
        cam >> frame;
    }

    // cvtColor(frame, edges, CV_BGR2GRAY);
    //GaussianBlur(edges, edges, Size(7, 7), 1.5, 1.5);
    //Canny(edges, edges, 0, 30, 3);
    //imshow("edges", edges);

    // if (waitKey(30) >= 0) break;
}

// called by XAML window OnNavigate event (please see MainPage.xaml.cpp)
void init()
{
    // for testing only
    TCC("main thread running");
    TC(std::this_thread::get_id);
    TCNL;

    cam.open(0);    // open the default camera - but do not start until size is set

    // set desired frame size before starting - WinRT requirement
    cam.set(CAP_PROP_FRAME_WIDTH, 640);
    cam.set(CAP_PROP_FRAME_HEIGHT, 480);

    // start the device on main thread - WinRT requirement
    cam.set(CAP_PROP_WINRT_START_DEVICE, 1);

//    t1.join();
}

// end


// not used & notes
#if 0
// DISABLE_XAML_GENERATED_MAIN must be defined to allow main to be located here
// nb. we don't do anything special here yet
//
int __cdecl main(::Platform::Array<::Platform::String^>^ args)
{
    (void)args; // Unused parameter
    Windows::UI::Xaml::Application::Start(ref new Windows::UI::Xaml::ApplicationInitializationCallback(
        [](Windows::UI::Xaml::ApplicationInitializationCallbackParams^ ) {
        auto app = ref new ::video_xaml::App();
    }));
    // control never gets here
}

// mainPanel.Children.Add(myCanvas);



// from
http://docs.opencv.org/modules/highgui/doc/reading_and_writing_images_and_video.html#videocapture
#include "opencv2/opencv.hpp"

using namespace cv;

int main(int, char**)
{
    VideoCapture cap(0); // open the default camera
    if (!cap.isOpened())  // check if we succeeded
        return -1;

    Mat edges;
    namedWindow("edges", 1);
    for (;;)
    {
        Mat frame;
        cap >> frame; // get a new frame from camera
        cvtColor(frame, edges, CV_BGR2GRAY);
        GaussianBlur(edges, edges, Size(7, 7), 1.5, 1.5);
        Canny(edges, edges, 0, 30, 3);
        imshow("edges", edges);
        if (waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
#endif
