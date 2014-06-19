// Video support with XAML
// Microsoft Surface and Phone using Media Foundation

// Copyright (c) 2014, Microsoft Open Technologies, Inc. 
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

#include "cap_winrt_video.hpp"

#include <ppl.h>
#include <ppltasks.h>
#include <concrt.h>
#include <agile.h>

#include <atomic>
#include <future>
#include <vector>


using namespace ::concurrency;
using namespace ::Windows::Foundation;
using namespace ::std;

using namespace Microsoft::WRL;
using namespace Windows::Media::Devices;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Media::Capture;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::Devices::Enumeration;

#include "cap_winrt/CaptureFrameGrabber.h"

// pull in MF libs (this has to be somewhere in the project)
#pragma comment(lib, "mfplat")
#pragma comment(lib, "mf")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "Shlwapi")

#include "cap_winrt_highgui.hpp"

#include "cdebug.h"

Video::Video() {
}

Video &Video::get() {
    static Video v;
    return v;
}


bool Video::initGrabber(int device, int w, int h)
{
    // std::atomic<bool> ready(false);

    width = w;
    height = h;
    bGrabberInited = false;

    //m_frontBuffer = std::unique_ptr<ofPixels>(new ofPixels);
    //m_backBuffer = std::unique_ptr<ofPixels>(new ofPixels);
    //m_frontBuffer->allocate(w, h, bytesPerPixel);
    //m_backBuffer->allocate(w, h, bytesPerPixel);

    //if (bChooseDevice){
    //    bChooseDevice = false;
    //    // ofLogNotice("ofWinrtVideoGrabber") << "initGrabber(): choosing " << m_deviceID;
    //}
    //else {
    //    m_deviceID = 0;
    //}

    //if (!m_devices.Get())
    //{
    //    listDevices();      
    //    if (!m_devices.Get())
    //    {
    //        // ofLogError("ofWinrtVideoGrabber") << "no video devices are available";
    //        return false;
    //    }
    //}

    m_deviceID = device;

    create_task(DeviceInformation::FindAllAsync(DeviceClass::VideoCapture))
        .then([this](task<DeviceInformationCollection^> findTask)
    {
        m_devices = findTask.get();

        // got selected device?
        if ((unsigned)m_deviceID >= m_devices.Get()->Size) return false;

        auto devInfo = m_devices.Get()->GetAt(m_deviceID);

        auto settings = ref new MediaCaptureInitializationSettings();
        settings->StreamingCaptureMode = StreamingCaptureMode::Video; // Video-only capture
        settings->VideoDeviceId = devInfo->Id;

        auto location = devInfo->EnclosureLocation;
        if (location != nullptr && location->Panel == Windows::Devices::Enumeration::Panel::Front)
        {
            bFlipImageX = true;
        }

        m_capture = ref new MediaCapture();
        create_task(m_capture->InitializeAsync(settings)).then([this](){

            auto props = safe_cast<VideoEncodingProperties^>(m_capture->VideoDeviceController->GetMediaStreamProperties(MediaStreamType::VideoPreview));
            // zv
            // props->Subtype = MediaEncodingSubtypes::Rgb24;          // for 24 bpp
            props->Subtype = MediaEncodingSubtypes::Bgra8;       // for test
            bytesPerPixel = 4;

            props->Width = width;
            props->Height = height;

            return ::Media::CaptureFrameGrabber::CreateAsync(m_capture.Get(), props);

        }).then([this](::Media::CaptureFrameGrabber^ frameGrabber)
        {
            m_frameGrabber = frameGrabber;
            bGrabberInited = true;
            //ready = true;
            _GrabFrameAsync(frameGrabber);
            //ofAddListener(ofEvents().appResume, this, &ofWinrtVideoGrabber::appResume, ofEventOrder::OF_EVENT_ORDER_AFTER_APP);
        });

        return true;
    });


    // cannot use - this will lock the UI thread:
    // wait for async task to complete
    //int count = 0;
    //while (!ready)
    //{
    //    count++;
    //}

    return true;
}


void Video::_GrabFrameAsync(::Media::CaptureFrameGrabber^ frameGrabber)
{
    // zv
#if 0
    // test: copy to temp buffer, then to Mat - use Bgr8 layout
    create_task(frameGrabber->GetFrameAsync()).then([this, frameGrabber](const ComPtr<IMF2DBuffer2>& buffer)
    {
        unsigned long length;
        // auto in = ref new WriteableBitmap(width, height);
        {
            std::lock_guard<std::mutex> lock(HighguiBridge::get().inputBufferMutex);

            // test
            // auto bitmap = ref new WriteableBitmap(width, height);
            // TCNL;  TC(bitmap->PixelBuffer->Capacity); TCNL;
            // auto bitmap = HighguiBridge::get().frontOutputBuffer;

            // ptr to input Mat data array
            auto inAr = HighguiBridge::get().backInputPtr;

            // CHK(buffer->ContiguousCopyTo(GetData(bitmap->PixelBuffer), bitmap->PixelBuffer->Capacity));
            // CHK(buffer->ContiguousCopyTo(GetData(in->PixelBuffer), in->PixelBuffer->Capacity));
            CHK(buffer->ContiguousCopyTo(inAr, width * height * 4));

            CHK(buffer->GetContiguousLength(&length));
            // in->PixelBuffer->Length = length;
            // nb. length = 1228800  
            // 1228800 / 4 bpp = 307200 = 640 x 480
            // TC(length); TCNL;
        }

        // zv test
        // HighguiBridge::get().m_cvImage->Source = HighguiBridge::get().frontOutputBuffer;

        // notify frame is ready
        HighguiBridge::get().bIsFrameNew = true;
        HighguiBridge::get().frameCounter++;

        if (HighguiBridge::get().frameCounter == 1) {
            // TCC("\n_GrabFrameAsync");  TC((void *)HighguiBridge::get().frontInputPtr); TCC("\n\n");
        }
        // TC(HighguiBridge::get().frameCounter); TCNL;

        // HighguiBridge::get().SwapInputBuffers();

        // test: copy from input WBM to input Mat
        //{
        //    auto inAr = GetData(in->PixelBuffer);
        //    auto outAr = HighguiBridge::get().frontInputPtr;
        //    for (unsigned int i = 0; i < length; i++)
        //        outAr[i] = inAr[i];
        //}

        // copy from input Mat to output WBM
        // CopyOutput();

        // post output WBM to XAML image element
        // HighguiBridge::get().m_cvImage->Source = HighguiBridge::get().frontOutputBuffer;
        //HighguiBridge::get().m_cvImage->Source = in;

        // test
        // HighguiBridge::get().requestForUIthreadAsync(HighguiBridge_UPDATE_IMAGE_ELEMENT);

        // for blocking:
        //{
        //    unique_lock<mutex> lck(HighguiBridge::get().frameReadyMutex);
        //    HighguiBridge::get().frameReadyEvent.notify_one();
        //}

        // loss of camera device & restart is not yet handled

        _GrabFrameAsync(frameGrabber);
    }, task_continuation_context::use_current());

#else
    // use rgb24 layout
    create_task(frameGrabber->GetFrameAsync()).then([this, frameGrabber](const ComPtr<IMF2DBuffer2>& buffer)
    {
        // do the RGB swizzle while copying the pixels from the IMF2DBuffer2
        BYTE *pbScanline;
        LONG plPitch;
        unsigned int colBytes = width * bytesPerPixel;
        CHK(buffer->Lock2D(&pbScanline, &plPitch));

        // TC(plPitch);    TC(numBytes);   TCNL;

        // flip
#if 1
        if (bFlipImageX)
        {
            std::lock_guard<std::mutex> lock(HighguiBridge::get().inputBufferMutex);

            // auto buf = GetData(HighguiBridge::get().m_backInputBuffer->PixelBuffer);
            // uint8_t* buf = reinterpret_cast<uint8_t*>(m_backInputBuffer->getPixels());

            // ptr to input Mat data array
            auto buf = HighguiBridge::get().backInputPtr;


            for (unsigned int row = 0; row < height; row++)
            {
                unsigned int i = 0;
                unsigned int j = colBytes - 1;

                while (i < colBytes)
                {
                    // reverse the scan line
                    // as a side effect this also swizzles R and B channels
                    buf[j--] = pbScanline[i++];
                    buf[j--] = pbScanline[i++];
                    buf[j--] = pbScanline[i++];
                }
                pbScanline += plPitch;
                buf += colBytes;
            }
        }
        else
#endif
        {
            std::lock_guard<std::mutex> lock(HighguiBridge::get().inputBufferMutex);
            // auto buf = HighguiBridge::get().frontInputPtr;

            // TCC("_GrabFrameAsync");  TC(HighguiBridge::get().frontInputPtr); TCNL;

            // auto buf = GetData(HighguiBridge::get().m_frontInputBuffer->PixelBuffer);
            // uint8_t* buf = reinterpret_cast<uint8_t*>(m_frontInputBuffer->getPixels());

            // from test:
            //auto bitmap = HighguiBridge::get().m_frontInputBuffer;
            //CHK(buffer->ContiguousCopyTo(GetData(bitmap->PixelBuffer), bitmap->PixelBuffer->Capacity));
            //unsigned long length;
            //CHK(buffer->GetContiguousLength(&length));
            //bitmap->PixelBuffer->Length = length;

            // ptr to input Mat data array
            auto buf = HighguiBridge::get().backInputPtr;

            for (unsigned int row = 0; row < height; row++)
            {
                for (unsigned int i = 0; i < colBytes; i++ /*i += bytesPerPixel*/)
                {
                    // swizzle the R and B values (BGR to RGB)
                    //buf[i] = pbScanline[i + 2];
                    //buf[i + 1] = pbScanline[i + 1];
                    //buf[i + 2] = pbScanline[i];

                    buf[i] = pbScanline[i];
                }
                pbScanline += plPitch;
                buf += colBytes;
            }
        }
        CHK(buffer->Unlock2D());

        // set length
        // HighguiBridge::get().m_backInputBuffer->PixelBuffer->Length = numBytes;

        // notify frame is ready
        HighguiBridge::get().bIsFrameNew = true;
        HighguiBridge::get().frameCounter++;

        // zv immed test
        //HighguiBridge::get().m_cvImage->Source = HighguiBridge::get().m_frontInputBuffer;

        //TC(HighguiBridge::get().frameCounter);
        //TCNL;

        if (bGrabberInited)
        {
            _GrabFrameAsync(frameGrabber);
        }
    }, task_continuation_context::use_current());

#endif
}


// copy from input Mat to output WBM
// must be on UI thread
void Video::CopyOutput()
{
    //std::lock_guard<std::mutex> lock1(HighguiBridge::get().inputBufferMutex);
    //std::lock_guard<std::mutex> lock2(HighguiBridge::get().outputBufferMutex);

    std::lock_guard<std::mutex> lock(HighguiBridge::get().outputBufferMutex);

    unsigned length = width * height * 4;
    auto inAr = HighguiBridge::get().backInputPtr;
    // auto inAr = HighguiBridge::get().frontInputPtr;
    auto outAr = GetData(HighguiBridge::get().frontOutputBuffer->PixelBuffer);
    for (unsigned int i = 0; i < length; i++)
        outAr[i] = inAr[i];
    HighguiBridge::get().frontOutputBuffer->PixelBuffer->Length = length;
}


#if 0
// CopyOutputBuffer must be in this file due to "GetData" macro
void Video::CopyOutputBuffer(unsigned char *p, int width, int height, int bytesPerPixel, int stride)
{
    BYTE *pbScanline = p;
    LONG plPitch = stride;
    unsigned int colBytes = width * bytesPerPixel;

    {
        // test:
        //std::lock_guard<std::mutex> lock(HighguiBridge::get().inputBufferMutex);
        //auto buf = GetData(HighguiBridge::get().m_frontInputBuffer->PixelBuffer);

        std::lock_guard<std::mutex> lock(HighguiBridge::get().outputBufferMutex);
        auto buf = GetData(HighguiBridge::get().backOutputBuffer->PixelBuffer);

        for (unsigned int row = 0; row < (unsigned)height; row++)
        {
            for (unsigned int i = 0; i < colBytes; i++ /*i += bytesPerPixel*/)
            {
                // no swizzle 
                //buf[i] = pbScanline[i];
                //buf[i + 1] = pbScanline[i + 1];
                //buf[i + 2] = pbScanline[i + 2];
                buf[i] = pbScanline[i];
            }
            pbScanline += plPitch;
            buf += colBytes;
        }
    }

    HighguiBridge::get().SwapOutputBuffers();
}

// must be in this file for GetData macro
unsigned char* Video::GetInputDataPtr()
{
    //auto bp = GetData(HighguiBridge::get().m_frontInputBuffer->PixelBuffer);
    return 0;
}

unsigned char* Video::GetOutputDataPtr()
{
    auto bp = GetData(HighguiBridge::get().backOutputBuffer->PixelBuffer);
    return bp;
}
#endif

bool Video::listDevicesTask()
{
    std::atomic<bool> ready(false);

    auto settings = ref new MediaCaptureInitializationSettings();

    //vector <int> devices;

    create_task(DeviceInformation::FindAllAsync(DeviceClass::VideoCapture))
        .then([this, &ready](task<DeviceInformationCollection^> findTask)
    {
        m_devices = findTask.get();

        for (size_t i = 0; i < m_devices->Size; i++)
        {
            // ofVideoDevice deviceInfo;
            auto d = m_devices->GetAt(i);
            //deviceInfo.bAvailable = true;
            //deviceInfo.deviceName = PlatformStringToString(d->Name);
            //deviceInfo.hardwareName = deviceInfo.deviceName;
            // devices.push_back(deviceInfo);
        }

        ready = true;
    });

    // wait for async task to complete
    int count = 0;
    while (!ready)
    {
        count++;
    }

    return true;
}


bool Video::listDevices()
{
    // synchronous version of listing video devices on WinRT
    std::future<bool> result = std::async(std::launch::async, &Video::listDevicesTask, this);
    return result.get();
}


