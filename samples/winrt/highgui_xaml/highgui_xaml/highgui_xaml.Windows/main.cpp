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

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>

// test
#include "../../../modules/highgui/src/cap_winrt_highgui.hpp"
#include <opencv2/highgui/cdebug.h>

using namespace cv;

//__declspec(dllexport) 
void cvMain()
{
    VideoCapture cam;

    cam.open(0);    // open the default camera  

    // Mat edges;
    // namedWindow("edges", 1);

    Mat frame;

    // process frames
    while (1)
    {
        // get a new frame from camera
        cam >> frame;

        // image processing calculations here

        //TCC("    main");
        //TC((void*)frame.data); TCNL;
        //TC((void*)frame.ptr(0)); TCNL;

        // test img manip
        auto ar = frame.ptr(0);
        // insert a green area
        for (int i = 1; i < 100000; i+=4 ) ar[i] = 0xff;

        //TC((void*)HighguiBridge::get().frontInputPtr); TCNL;
        //TC((void*)HighguiBridge::get().backInputPtr); TCNL;

        //for (int i = 0; i < 1000; i++)
        //    frame.at<unsigned char>(i, i) = 0xFF;

        imshow("xaml", frame);
    }

    // cvtColor(frame, edges, CV_BGR2GRAY);
    //GaussianBlur(edges, edges, Size(7, 7), 1.5, 1.5);
    //Canny(edges, edges, 0, 30, 3);
    ////imshow("edges", edges);
}


// not used & notes
#if 0

// for XAML
#include "pch.h"
#include "App.xaml.h"

// called by XAML window OnNavigate event (please see MainPage.xaml.cpp)
void init()
{
    // for testing only
    TCC("main thread running");
    TC(std::this_thread::get_id);
    TCNL;

    // move all this code to cvMain

    cam.open(0);    // open the default camera - but do not start until size is set    

    // set desired frame size before starting - WinRT requirement
    cam.set(CAP_PROP_FRAME_WIDTH, 640);
    cam.set(CAP_PROP_FRAME_HEIGHT, 480);

    // start the device on main thread - WinRT requirement
    cam.set(CAP_PROP_WINRT_START_DEVICE, 1);

//    image_processing_thread.join();
}

// end


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
