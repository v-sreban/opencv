//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
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

// for opencv to write to XAML image 
// decl in cap_winrt.cpp
// nb. C++ extern will not work across DLLs - must use dllimport
__declspec(dllimport) ::Windows::UI::Xaml::Controls::Image^ gOutput;

MainPage::MainPage()
    : width(0)
    , height(0)
{
	InitializeComponent();
}

// implemented in main.cpp
void init();

void MainPage::OnNavigatedTo(NavigationEventArgs^ e)
{
    (void)e;	// Unused parameter

    init();

    gOutput = Preview;
}
