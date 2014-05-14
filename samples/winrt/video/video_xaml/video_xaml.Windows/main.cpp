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
// - Neither the name of Microsoft Open Technologies, Inc. nor the names of its contributors 
//   may be used to endorse or promote products derived from this software 
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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

#include <thread>
#include <chrono>

using namespace cv;
namespace SC = std::chrono;

VideoCapture cap;


// incomplete - not called yet
void process()
{
    SC::duration<int, std::milli> delay{ 100 };

    // we could use a mutex or atomic here instead
    while (!cap.isOpened())
    {
        std::this_thread::sleep_for<int, std::milli>(delay);
    }
    //if (!cap.isOpened())
    //    return;

    Mat edges;
    // namedWindow("edges", 1);

    Mat frame;
    //    cap >> frame; // get a new frame from camera

    // cvtColor(frame, edges, CV_BGR2GRAY);
    //GaussianBlur(edges, edges, Size(7, 7), 1.5, 1.5);
    //Canny(edges, edges, 0, 30, 3);
    //imshow("edges", edges);

    // if (waitKey(30) >= 0) break;
}

// called by XAML window OnNavigate event (please see MainPage.xaml.cpp)
void init()
{
    cap.open(0);    // open the default camera
}

// nb. t1 must exist as long as the app exists
// should do a join somewhere ...
static std::thread t1{ process };

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


// notes
#if 0


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
