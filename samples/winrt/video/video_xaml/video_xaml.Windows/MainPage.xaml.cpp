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

MainPage::MainPage()
    : _width(0)
    , _height(0)
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

        _width = props->Width;
        _height = props->Height;

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
        auto bitmap = ref new WriteableBitmap(_width, _height);

        CHK(buffer->ContiguousCopyTo(GetData(bitmap->PixelBuffer), bitmap->PixelBuffer->Capacity));

        unsigned long length;
        CHK(buffer->GetContiguousLength(&length));
        bitmap->PixelBuffer->Length = length;

        Preview->Source = bitmap;

        _GrabFrameAsync(frameGrabber);
    }, task_continuation_context::use_current());
}

