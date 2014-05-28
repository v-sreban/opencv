// Capture support for WinRT - bridge from OpenCV to XAML
// Microsoft Surface and Phone using Media Foundation

// Copyright (c) 2013, Microsoft Open Technologies, Inc. 
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice, 
//   this list of conditions and the following disclaimer in the documentation 
//   and/or other materials provided with the distribution.
// - Neither the name of Microsoft Open Technologies, Inc. nor the names 
//   of its contributors may be used to endorse or promote products derived 
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include "cap_winrt_highgui.hpp"

using namespace Windows::Foundation;
using namespace Windows::Media::Capture;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Devices::Enumeration;

using namespace Platform;
using namespace ::Concurrency;
using namespace ::Windows::Foundation;


void HighguiBridge::processOnUIthread(int action)
{
    // this is running on the UI thread
    switch (action)
    {
    case HighguiBridge_OPEN_CAMERA:        
        initializeDevice();
        break;
    case HighguiBridge_CLOSE_CAMERA:
        // closeDevice();
        break;
    case HighguiBridge_UPDATE_IMAGE_ELEMENT:
        // copy Mat into backbuffer;
        // swap preview buffers
        // Preview = frontbuffer;
        break;
    }
}


// block
bool HighguiBridge::initializeDevice()
{
    done = false;
    std::future<bool> result = std::async(std::launch::async, &HighguiBridge::initializeDeviceTask, this);
    return result.get();
}


bool HighguiBridge::initializeDeviceTask()
{
    std::atomic<bool> ready(false);

    auto settings = ref new MediaCaptureInitializationSettings();
    settings->StreamingCaptureMode = StreamingCaptureMode::Video; // Video-only capture

    // set m_capture and m_devices
    m_capture = ref new MediaCapture();
    create_task(m_capture->InitializeAsync(settings)).then([this, &ready]() {

        create_task(DeviceInformation::FindAllAsync(DeviceClass::VideoCapture))
            .then([this, &ready](task<DeviceInformationCollection^> findTask)
        {
            m_devices = findTask.get();
            ready = true;
        });
    });

    // wait for async tasks to complete
    int count = 0;
    while (!ready)
    {
        count++;
    }
    done = true;

    return true;
}

void HighguiBridge::waitForUIthreadRequest()
{
    int count = 0;
    while (!done)
    {
        count++;
    }
}
