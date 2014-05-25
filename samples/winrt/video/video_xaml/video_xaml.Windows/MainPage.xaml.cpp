//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

#include "../../../modules/highgui/src/cap_winrt_highgui.hpp"

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

#include <ppl.h>
#include <concrt.h>
#include <atomic>

using namespace ::concurrency;
using namespace ::Windows::Foundation;

// implemented in main.cpp
__declspec(dllimport) void cvMain();


MainPage::MainPage()
{
    InitializeComponent();

    int count = 0;
    auto asyncTask = TaskWithProgressAsync();
    asyncTask->Progress = ref new AsyncActionProgressHandler<int>([this, &count](IAsyncActionWithProgress<int>^ act, int progress)
    {
        // this is running on the UI thread
        switch (progress)
        {
        case HighguiBridge_OPEN_CAMERA:
            HighguiBridge::getInstance().initializeDevice();
            break;
        case HighguiBridge_CLOSE_CAMERA:
            // HighguiBridge::getInstance().closeDevice();
            break;
        case HighguiBridge_UPDATE_IMAGE_ELEMENT:
            // copy Mat into backbuffer;
            // swap preview buffers
            // Preview = frontbuffer;
            break;
        }
        count++;
    });

}

// set the reporter method for the HighguiAssist singleton
// start the main OpenCV as an async thread in WinRT
IAsyncActionWithProgress<int>^ MainPage::TaskWithProgressAsync()
{
    return create_async([this](progress_reporter<int> reporter)
    {
        HighguiBridge::getInstance().setReporter(reporter);
        cvMain();
    });
}