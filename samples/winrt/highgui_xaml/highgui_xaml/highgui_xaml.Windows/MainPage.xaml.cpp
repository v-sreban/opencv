//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

// cannot include here because we can't export CaptureFrameGrabber WinRT class
// #include "../../../modules/highgui/src/cap_winrt_video.hpp"

#include "../../../modules/highgui/src/cap_winrt_highgui.hpp"

using namespace highgui_xaml;

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
#include <ppltasks.h>
#include <concrt.h>
#include <agile.h>

using namespace ::concurrency;
using namespace ::Windows::Foundation;

using namespace Windows::UI::Xaml::Media::Imaging;

// needed for linker
extern bool initGrabber(int device, int w, int h);
extern void copyOutput();


MainPage::MainPage()
{
    InitializeComponent();

    // set XAML elements
    HighguiBridge::get().cvImage = cvImage;
    HighguiBridge::get().cvSlider = cvSlider;

    auto asyncTask = TaskWithProgressAsync();
    asyncTask->Progress = ref new AsyncActionProgressHandler<int>([this](IAsyncActionWithProgress<int>^ act, int progress)
    {
        int action = progress;

        // these actions will be processed on the UI thread asynchronously
        switch (action)
        {
        case HighguiBridge_OPEN_CAMERA:
            {
                int device = HighguiBridge::get().deviceIndex;
                int width = HighguiBridge::get().width;
                int height = HighguiBridge::get().height;

                // buffers must alloc'd on UI thread
                //HighguiBridge::get().m_frontInputBuffer = ref new WriteableBitmap(width, height);
                //HighguiBridge::get().m_backInputBuffer = ref new WriteableBitmap(width, height);
                HighguiBridge::get().frontOutputBuffer = ref new WriteableBitmap(width, height);
                HighguiBridge::get().backOutputBuffer = ref new WriteableBitmap(width, height);

                // video capture device init must be done on UI thread;
                // code is located in the OpenCV Highgui DLL, class Video
                // not class member due to linker errors
                initGrabber(device, width, height);
        }
            break;
        case HighguiBridge_CLOSE_CAMERA:
            // closeDevice();
            break;
        case HighguiBridge_UPDATE_IMAGE_ELEMENT:
            // zv
            // testing: for direct copy bypassing OpenCV:
            // HighguiBridge::get().m_cvImage->Source = HighguiBridge::get().backInputPtr;

            // copy output Mat to WBM
            copyOutput();

            // set XAML image element with image WBM
            HighguiBridge::get().cvImage->Source = HighguiBridge::get().frontOutputBuffer;
            break;
        case HighGuiAssist_SHOW_TRACKBAR:
            cvSlider->Visibility = Windows::UI::Xaml::Visibility::Visible;
            break;
        }
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

