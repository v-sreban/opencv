
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

// DISABLE_XAML_GENERATED_MAIN must be defined

using namespace cv;


int __cdecl main(::Platform::Array<::Platform::String^>^ args)
{
    (void)args; // Unused parameter
    Windows::UI::Xaml::Application::Start(ref new Windows::UI::Xaml::ApplicationInitializationCallback(
        [](Windows::UI::Xaml::ApplicationInitializationCallbackParams^ p) {
        (void)p; // Unused parameter

        // initialization
        VideoCapture cap(0);    // open the default camera
        if (!cap.isOpened())    // check if we succeeded
            return;

        auto app = ref new ::video_xaml::App();
    }));
    // control never gets here
}


// incomplete
void drawLoop()
{
    Mat edges;
    namedWindow("edges", 1);

    Mat frame;
//    cap >> frame; // get a new frame from camera
    
    // cvtColor(frame, edges, CV_BGR2GRAY);
    GaussianBlur(edges, edges, Size(7, 7), 1.5, 1.5);
    Canny(edges, edges, 0, 30, 3);
    imshow("edges", edges);
    // if (waitKey(30) >= 0) break;
}


// notes
#if 0
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
