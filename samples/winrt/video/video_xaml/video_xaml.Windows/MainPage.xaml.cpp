//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "CaptureFrameGrabber/CaptureFrameGrabber.h"
#include "MainPage.xaml.h"

using namespace video_xaml;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

// for using MF:
using namespace concurrency;
using namespace Microsoft::WRL;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Media::Capture;
using namespace Windows::UI::Xaml::Media::Imaging;

// pull in MF libs (this has to be somewhere in the project)
#pragma comment(lib, "mfplat")
#pragma comment(lib, "mf")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "Shlwapi")

// for OpenCV:
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\features2d\features2d.hpp>

// #include "opencv2/core/core_c.h"
#include "opencv2/imgproc/imgproc_c.h"



MainPage::MainPage()
    : width(0)
    , height(0)
{
	InitializeComponent();
}

void MainPage::OnNavigatedTo(NavigationEventArgs^ e)
{
    (void)e;	// Unused parameter

    auto settings = ref new MediaCaptureInitializationSettings();
    settings->StreamingCaptureMode = StreamingCaptureMode::Video; // Video-only capture

    _capture = ref new MediaCapture();
    create_task(_capture->InitializeAsync(settings)).then([this](){

        auto props = safe_cast<VideoEncodingProperties^>(_capture->VideoDeviceController->GetMediaStreamProperties(MediaStreamType::VideoPreview));
        props->Subtype = MediaEncodingSubtypes::Bgra8; // Ask for color conversion to match WriteableBitmap

        width = props->Width;
        height = props->Height;

        return ::Media::CaptureFrameGrabber::CreateAsync(_capture.Get(), props);

    }).then([this](::Media::CaptureFrameGrabber^ frameGrabber)
    {
        _GrabFrameAsync(frameGrabber);
    });
}

void MainPage::_GrabFrameAsync(::Media::CaptureFrameGrabber^ frameGrabber)
{
    create_task(frameGrabber->GetFrameAsync()).then([this, frameGrabber](const ComPtr<IMF2DBuffer2>& buffer)
    {
#if 0
        auto bitmap = ref new WriteableBitmap(width, height);

        CHK(buffer->ContiguousCopyTo(GetData(bitmap->PixelBuffer), bitmap->PixelBuffer->Capacity));

        unsigned long length;
        CHK(buffer->GetContiguousLength(&length));
        bitmap->PixelBuffer->Length = length;

        Preview->Source = bitmap;
#endif
        const int bytesPerPixel = 3;
        const bool copyOnly = false;

        // output
        auto bitmap = ref new WriteableBitmap(width, height);
        auto pbOut = GetData(bitmap->PixelBuffer);

        BYTE *pbScanline;
        LONG plPitch;
        unsigned int numBytes = width * bytesPerPixel;
        CHK(buffer->Lock2D(&pbScanline, &plPitch));

        if (copyOnly) {

            // nb. no R/B swizzle seems to be needed
            cv::Mat InputFrame(height, width, CV_8UC3 | CV_MAT_CONT_FLAG, pbScanline);
            cv::Mat OutputFrame(height, width, CV_8UC3 | CV_MAT_CONT_FLAG, pbOut);

            // no effect - straight copy
            InputFrame.copyTo(OutputFrame);

        } else {

            // effect
            cv::Mat InputFrame(height, width, CV_8UC3 | CV_MAT_CONT_FLAG, pbScanline);
            cv::Mat OutputFrame(height, width, CV_8UC3 | CV_MAT_CONT_FLAG, pbOut);

            // not working
            cv::Mat InputGreyScale(InputFrame, cv::Range(0, height), cv::Range(0, width));
            OutputFrame.setTo(cv::Scalar(128));
            cv::Mat OutputGreyScale(OutputFrame, cv::Range(0, height), cv::Range(0, width));
            InputGreyScale.copyTo(OutputGreyScale);

            //OutputGreyScale.copyTo(OutputFrame);

            // does something, but incorrect
            //cv::Mat gray(height, width, CV_8UC1);
            //cv::cvtColor(InputFrame, gray, CV_BGR2GRAY);        // convert
            //cv::cvtColor(gray, OutputFrame, CV_GRAY2RGB);       // unpack to BGR

            // has a stride problem?

            // notes
#if 0
            // gray.copyTo(OutputFrame);

            //OutputFrame.setTo(cv::Vec3b(128,128,128));

            /*
            cv::Mat InputGreyScale(InputFrame, cv::Range(0, height), cv::Range(0, width));

            OutputFrame.setTo(cv::Scalar(128));
            cv::Mat OutputGreyScale(OutputFrame, cv::Range(0, height), cv::Range(0, width));
            InputGreyScale.copyTo(OutputGreyScale);

            OutputGreyScale.copyTo(OutputFrame);
            */
            /*
            Mat frame, edges;
            namedWindow("edges",1);
            for(;;)
            {
            cap >> frame;
            cvtColor(frame, edges, CV_BGR2GRAY);
            GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
            Canny(edges, edges, 0, 30, 3);
            */
            //OutputFrame.setTo(cv::Scalar(128));
            //cv::Mat OutputGreyScale(OutputFrame, cv::Range(0, height), cv::Range(0, width));
            //InputGreyScale.copyTo(OutputGreyScale);

            //OutputFrame.setTo(cv::Scalar(128));
            //cv::Mat OutputGreyScale(OutputFrame, cv::Range(0, height), cv::Range(0, width));
            //cv::Canny(InputGreyScale, OutputGreyScale, 80, 90);
#endif
        }

        CHK(buffer->Unlock2D());

        Preview->Source = bitmap;

        // loss of camera device & restart is not yet handled

        _GrabFrameAsync(frameGrabber);
    }, task_continuation_context::use_current());
}

