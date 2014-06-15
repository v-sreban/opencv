// Highgui for OpenCV to XAML
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
#include "cap_winrt_video.hpp"

using namespace Windows::Foundation;
using namespace Windows::Media::Capture;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Devices::Enumeration;

using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Microsoft::WRL;

using namespace Platform;
using namespace ::Concurrency;

using namespace ::std;

// non-blocking
bool initGrabber(int device, int w, int h)
{
    // nb. Video class is not exported outside of this DLL
    // due to complexities in the CaptureFrameGrabber ref class 
    // as written in the header not mixing well with pure C++ classes
    return Video::get().initGrabber(device, w, h);
}

// non-blocking
void HighguiBridge::requestForUIthreadAsync(int action, int widthp, int heightp)
{
    if (action == HighguiBridge_OPEN_CAMERA) {
        width = widthp == 0 ? 640 : widthp;
        height = heightp == 0 ? 480 : heightp;
    }

    reporter.report(action);
}

HighguiBridge& HighguiBridge::get()
{
    static HighguiBridge instance;
    return instance;
}


void HighguiBridge::SwapInputBuffers()
{
    lock_guard<mutex> lock(inputBufferMutex);
    if (currentFrame != frameCounter)
    {
        currentFrame = frameCounter;
        // zv
//        swap(m_backInputBuffer, m_frontInputBuffer);
    }
}

void HighguiBridge::SwapOutputBuffers()
{
    lock_guard<mutex> lock(outputBufferMutex);
    swap(m_backOutputBuffer, m_frontOutputBuffer);
}

unsigned char * HighguiBridge::GetInputDataPtr(){
    return Video::get().GetInputDataPtr();
}

void imshow_winrt(cv::InputArray img)
{
    auto m = img.getMat();
    auto in = m.ptr(0);
    int width = img.size().width;
    int height = img.size().height;

    // GetOutputDataPtr() throws exception - moved to Video class
#if 0
    auto out = Video::get().GetOutputDataPtr();

    // copy InputArray to Writeable bitmap
    const int bytesPerPixel = 3;
    BYTE *pbScanline = in;
    LONG plPitch = width;
    unsigned int numBytes = width * bytesPerPixel;
    auto buf = out;
    for (unsigned int row = 0; row < (unsigned)height; row++)
    {
        for (unsigned int i = 0; i < numBytes; i += bytesPerPixel)
        {
            buf[i] = pbScanline[i];
            buf[i + 1] = pbScanline[i + 1];
            buf[i + 2] = pbScanline[i + 2];
        }
        pbScanline += plPitch;
        buf += numBytes;
    }
#endif

    // zv
    // Video::get().CopyOutputBuffer(in, width, height, 3, width);

    // request UI thread XAML image element update
    // HighguiBridge::get().SwapOutputBuffers();
    // HighguiBridge::get().requestForUIthreadAsync(HighguiBridge_UPDATE_IMAGE_ELEMENT);
}


// maybe not needed?
#if 0

void HighguiBridge::CopyOutputBuffer(unsigned char *p, int width, int height, int bytesPerPixel, int stride)
{
    // do the RGB swizzle while copying the pixels from the IMF2DBuffer2
    BYTE *pbScanline = p;
    LONG plPitch = stride;
    unsigned int numBytes = width * bytesPerPixel;

    {
        std::lock_guard<std::mutex> lock(HighguiBridge::get().outputBufferMutex);
        auto buf = GetData(HighguiBridge::get().m_backOutputBuffer->PixelBuffer);

        for (unsigned int row = 0; row < height; row++)
        {
            for (unsigned int i = 0; i < numBytes; i += bytesPerPixel)
            {
                // swizzle the R and B values (BGR to RGB)
                buf[i] = pbScanline[i + 2];
                buf[i + 1] = pbScanline[i + 1];
                buf[i + 2] = pbScanline[i];
            }
            pbScanline += plPitch;
            buf += numBytes;
        }
    }
}

// extracted from MFincludes.h:
#include <robuffer.h>

namespace WSS = ::Windows::Storage::Streams;
namespace MW = ::Microsoft::WRL;

#define CHK(statement)  {HRESULT _hr = (statement); if (FAILED(_hr)) { throw ref new Platform::COMException(_hr); };}

// Cast a C++/CX smartpointer to an ABI smartpointer
template<typename T, typename U>
MW::ComPtr<T> As(U^ in)
{
    MW::ComPtr<T> out;
    CHK(reinterpret_cast<IInspectable*>(in)->QueryInterface(IID_PPV_ARGS(&out)));
    return out;
}

unsigned char* GetData(_In_ WSS::IBuffer^ buffer)
{
    unsigned char* bytes = nullptr;
    CHK(As<WSS::IBufferByteAccess>(buffer)->Buffer(&bytes));
    return bytes;
}

void HighguiBridge::imshow(cv::InputArray matToShow)
{
    const int bytesPerPixel = 3;

    // copy from matToShow into back buffer
    // INCOMPLETE

    BYTE *pbScanline;
    LONG plPitch;
    unsigned int numBytes = width * bytesPerPixel;
    CHK(buffer->Lock2D(&pbScanline, &plPitch));

    {
        std::lock_guard<std::mutex> lock(HighguiBridge::get().outputBufferMutex);
        auto buf = GetData(HighguiBridge::get().m_backOutputBuffer->PixelBuffer);

        for (unsigned int row = 0; row < height; row++)
        {
            for (unsigned int i = 0; i < numBytes; i += bytesPerPixel)
            {
                buf[i] = pbScanline[i];
                buf[i + 1] = pbScanline[i + 1];
                buf[i + 2] = pbScanline[i + 2];
            }
            pbScanline += plPitch;
            buf += numBytes;
        }
    }

    CHK(buffer->Unlock2D());

    SwapOutputBuffers();
    requestForUIthreadAsync(HighguiBridge_UPDATE_IMAGE_ELEMENT);
}
#endif


// notes
#if 0
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

void HighguiBridge::waitForUIthreadRequest()
{
    int count = 0;
    while (!UIthreadTaskDone)
    {
        count++;
    }
}

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
