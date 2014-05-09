//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

#include <mutex>
#include <memory>

namespace Media
{
    ref class CaptureFrameGrabber;
}

namespace video_xaml
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void _GrabFrameAsync(::Media::CaptureFrameGrabber^ frameGrabber);

        Platform::Agile<::Windows::Media::Capture::MediaCapture> _capture;
        unsigned int width;
        unsigned int height;

        // double buffering
        std::mutex                 m_mutex;
        std::unique_ptr<Windows::UI::Xaml::Media::Imaging::WriteableBitmap^>   m_frontBuffer;
        std::unique_ptr<Windows::UI::Xaml::Media::Imaging::WriteableBitmap^>   m_backBuffer;

    };
}
