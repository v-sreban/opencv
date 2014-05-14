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

        virtual void OnRendering(Object^ sender, Object^ args)
        {
            int i = 0;
        }

    private:

        unsigned int width;
        unsigned int height;
    };
}
