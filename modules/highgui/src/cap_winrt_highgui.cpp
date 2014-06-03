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

#include "cap_winrt.hpp"
#include "cap_winrt_highgui.hpp"

using namespace Windows::Foundation;
using namespace Windows::Media::Capture;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Devices::Enumeration;

using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Microsoft::WRL;

using namespace Platform;
using namespace ::Concurrency;

using namespace ::std;


void HighguiBridge::processOnUIthread(int action)
{
    // this is running on the UI thread
    switch (action)
    {
    case HighguiBridge_OPEN_CAMERA:        
        // initializeDevice();
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

#if 0
bool HighguiBridge::initializeDevice()
{
    // blocking requires both a future and a spinlock on the atomic in the task completion
    UIthreadTaskDone = false;
    std::future<bool> result = std::async(std::launch::async, &HighguiBridge::initializeDeviceTask, this);
    return result.get();
}


// static void GrabFrameAsync(Media::CaptureFrameGrabber^ frameGrabber);


bool HighguiBridge::initializeDeviceTask()
{
    std::atomic<bool> ready(false);

    auto settings = ref new MediaCaptureInitializationSettings();
    settings->StreamingCaptureMode = StreamingCaptureMode::Video; // Video-only capture

    // set m_devices adn  m_capture
    m_capture = ref new MediaCapture();

    create_task(DeviceInformation::FindAllAsync(DeviceClass::VideoCapture))
        .then([this, &ready](task<DeviceInformationCollection^> findTask)
    {
        m_devices = findTask.get();

        if (deviceIndex < 0 || (unsigned)deviceIndex >= m_devices.Get()->Size)
            return false;

        auto devInfo = m_devices.Get()->GetAt(deviceIndex);

        auto settings = ref new MediaCaptureInitializationSettings();
        settings->StreamingCaptureMode = StreamingCaptureMode::Video; // Video-only capture
        settings->VideoDeviceId = devInfo->Id;

        create_task(m_capture->InitializeAsync(settings)).then([this](){

            auto props = safe_cast<VideoEncodingProperties^>(m_capture->VideoDeviceController->GetMediaStreamProperties(MediaStreamType::VideoPreview));
            props->Subtype = MediaEncodingSubtypes::Rgb24;
            props->Width = width;
            props->Height = height;

            return ::Media::CaptureFrameGrabber::CreateAsync(m_capture.Get(), props);

        }).then([this](::Media::CaptureFrameGrabber^ frameGrabber)
        {
            // m_frameGrabber = frameGrabber;
            deviceReady = true;
            // GrabFrameAsync(frameGrabber);
            //ofAddListener(ofEvents().appResume, this, &ofWinrtVideoGrabber::appResume, ofEventOrder::OF_EVENT_ORDER_AFTER_APP);
        });
        return true;
    });

    // wait for async tasks to complete
    int count = 0;
    while (!deviceReady)
    {
        count++;
    }
    UIthreadTaskDone = true;

    return true;
}
#endif

void HighguiBridge::waitForUIthreadRequest()
{
    int count = 0;
    while (!UIthreadTaskDone)
    {
        count++;
    }
}

void HighguiBridge::requestForUIthread(int action) 
{ 
    reporter.report(action); 
}

HighguiBridge& HighguiBridge::get()
{
    static HighguiBridge instance;
    return instance;
}

#if 0
void GrabFrameAsync(::Media::CaptureFrameGrabber^ frameGrabber)
{
    create_task(frameGrabber->GetFrameAsync()).then([frameGrabber](const ComPtr<IMF2DBuffer2>& buffer)
    {
        auto width = HighguiBridge::get().width;
        auto height = HighguiBridge::get().height;
#if 1
        auto bitmap = ref new WriteableBitmap(width, height);

        CHK(buffer->ContiguousCopyTo(GetData(bitmap->PixelBuffer),
            bitmap->PixelBuffer->Capacity));

        unsigned long length;
        CHK(buffer->GetContiguousLength(&length));
        bitmap->PixelBuffer->Length = length;

        // write to the XAML image element (temp)
        // if (gOutput) gOutput->Source = bitmap;
#else

        const int bytesPerPixel = 3;

        auto p = m_backBuffer.get();
        auto pbOut = GetData((*p)->PixelBuffer);

        BYTE *pbScanline;
        LONG plPitch;
        unsigned int numBytes = width * bytesPerPixel;
        CHK(buffer->Lock2D(&pbScanline, &plPitch));
        {
            lock_guard<mutex> lock(bufferMutex);

            // nb. no R/B swizzle seems to be needed
            cv::Mat InputFrame(height, width, CV_8UC3 | CV_MAT_CONT_FLAG, pbScanline);
            cv::Mat OutputFrame(height, width, CV_8UC3 | CV_MAT_CONT_FLAG, pbOut);

            // no effect - straight copy
            InputFrame.copyTo(OutputFrame);
        }

        CHK(buffer->Unlock2D());

        // TODO: move to draw loop and add buffer swapping code
        if (gOutput) gOutput->Source = *m_backBuffer.get();
#endif

        HighguiBridge::get().frameCounter++;

        // notify frame is ready
        {
            unique_lock<mutex> lck(HighguiBridge::get().frameReadyMutex);
            HighguiBridge::get().frameReadyEvent.notify_one();
        }

        GrabFrameAsync(frameGrabber);

    }, task_continuation_context::use_current());
}

#endif
