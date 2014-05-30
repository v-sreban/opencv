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


MainPage::MainPage()
{
    InitializeComponent();

    auto asyncTask = TaskWithProgressAsync();
    asyncTask->Progress = ref new AsyncActionProgressHandler<int>([](IAsyncActionWithProgress<int>^ act, int progress)
    {
        int action = progress;
        HighguiBridge::get().processOnUIthread(action);
    });
}

// implemented in main.cpp
void cvMain();

// set the reporter method for the HighguiAssist singleton
// start the main OpenCV as an async thread in WinRT
IAsyncActionWithProgress<int>^ MainPage::TaskWithProgressAsync()
{
    return create_async([this](progress_reporter<int> reporter)
    {
        HighguiBridge::get().setReporter(reporter);
        cvMain();
    });
}