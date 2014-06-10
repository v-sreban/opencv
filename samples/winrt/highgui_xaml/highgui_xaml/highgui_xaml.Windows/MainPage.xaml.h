//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

//#include "CaptureFrameGrabber/CaptureFrameGrabber.h"
//#include <mutex>
//#include <memory>

namespace highgui_xaml
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
    public ref class MainPage sealed
    {
    public:
        MainPage();

    protected:

        Windows::Foundation::IAsyncActionWithProgress<int>^ TaskWithProgressAsync();

        // virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:

    };
}
