// Capture support for WinRT
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

#include "precomp.hpp"
#include "cap_winrt.hpp"
#include "cap_winrt_highgui.hpp"

// pull in MF libs (this has to be somewhere in the project)
//#pragma comment(lib, "mfplat")
//#pragma comment(lib, "mf")
//#pragma comment(lib, "mfuuid")
//#pragma comment(lib, "Shlwapi")

#define CHK(statement)  {HRESULT _hr = (statement); if (FAILED(_hr)) { throw ref new Platform::COMException(_hr); };}

// test
#include <opencv2/highgui/cdebug.h>

using namespace Windows::Foundation;
using namespace Windows::Media::Capture;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Devices::Enumeration;

using namespace Platform;
// using namespace ::concurrency;

using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Microsoft::WRL;

// nb. must use dllexport to inform linker
//__declspec(dllexport) ::Windows::UI::Xaml::Controls::Image^ gOutput = nullptr;

// #include <thread>

// temp
// #include "../../samples/winrt/video/video_xaml/video_xaml.Windows/MainPage.xaml.h"

using namespace ::std;

// __declspec(dllexport) atomic<bool> startProcessing = false;

// from main app DLL
// __declspec(dllimport) void SwapInputBuffers();


// nb. VideoCapture_WinRT is not a singleton, so the Mats are made file statics
// We do not support more than one capture device simultaneously with the
// design at this time

// nb. inputBufferMutex was not able to guarantee that OpenCV Mats were
// ready to accept data in the UI thread (memory access exceptions were thrown
// even though buffer address was good).
// Therefore allocation of Mats is also done on the UI thread before the video
// device is initialized.  

static cv::Mat frontInputMat;
static cv::Mat backInputMat;

// performed on UI thread
void allocateBuffers(int width, int height)
{
    // allocate input Mats (bgra8 = CV_8UC4, RGB24 = CV_8UC3)
    frontInputMat.create(height, width, CV_8UC3);
    backInputMat.create(height, width, CV_8UC3);
    HighguiBridge::getInstance().frontInputPtr = frontInputMat.ptr(0);
    HighguiBridge::getInstance().backInputPtr = backInputMat.ptr(0);

    HighguiBridge::getInstance().allocateOutputBuffer();

    // debug
    TCC("    init");
    TC((void*)backInputMat.ptr(0));
    TCNL;
}


namespace cv {

    VideoCapture_WinRT::VideoCapture_WinRT(int device) : started(false)
    {
        HighguiBridge::getInstance().deviceIndex = device;
    }

    // grab a frame:
    // this will block until a frame is grabbed
    // should be called on the image processing thread
    bool VideoCapture_WinRT::grabFrame()
    {
        // if device is not started we must return true so retrieveFrame() is called to start device
        if (!started) return true;

        if (HighguiBridge::getInstance().bIsFrameNew)
        {
            return true;
        }

        // for blocking:
        //unique_lock<mutex> lock(HighguiBridge::getInstance().frameReadyMutex);
        //HighguiBridge::getInstance().frameReadyEvent.wait(lock);
        return false;
    }

    // should be called on the image processing thread after grabFrame
    // see VideoCapture::read
    bool VideoCapture_WinRT::retrieveFrame(int channel, cv::OutputArray outArray)
    {
        if (!started) {

            {
                std::lock_guard<std::mutex> lock(HighguiBridge::getInstance().inputBufferMutex);

                int width, height;
                width = outArray.size().width;
                height = outArray.size().height;
                if (width == 0) width = 640;
                if (height == 0) height = 480;

                HighguiBridge::getInstance().width = width;
                HighguiBridge::getInstance().height = height;

                // Mats will be alloc'd on UI thread

                //frontAr.create(height, width, CV_8UC4);
                //auto p = frontAr.getMat().ptr(0);
                //HighguiBridge::getInstance().frontInputPtr = p;

                //// allocate input Mats (bgra8 for test)
                //frontInputMat.create(height, width, CV_8UC4);
                //backInputMat.create(height, width, CV_8UC4);
                //HighguiBridge::getInstance().frontInputPtr = frontInputMat.ptr(0);
                //HighguiBridge::getInstance().backInputPtr = backInputMat.ptr(0);

                // test
                //outArray.getMat() = frontInputMat;
                //outArray.getObj();
            }

            // request device init on UI thread - this does not block, and is async
            HighguiBridge::getInstance().requestForUIthreadAsync(OPEN_CAMERA,
                outArray.size().width, outArray.size().height);

            started = true;
            return true;
        }

        if (!started) return false;

        // ERROR this is not really working properly - moved to cap.cpp
        //OutputArray out(frontInputMat);
        //outArray = out;
        // outArray.getMat() = frontInputMat;

        //TCC("    retrieveFrame");
        //TC((void*)outArray.getMat().ptr(0)); TCNL;
        //TC((void*)HighguiBridge::getInstance().frontInputPtr); TCNL;

        return HighguiBridge::getInstance().bIsFrameNew;
    }


    bool VideoCapture_WinRT::setProperty(int property_id, double value)
    {
        switch (property_id)
        {
        case CAP_PROP_FRAME_WIDTH:
            //size.width = (int)value;
            break;
        case CAP_PROP_FRAME_HEIGHT:
            //size.height = (int)value;
            break;
        default:
            return false;
        }
        return true;
    }
}

    // notes
#if 0
    void VideoCapture_WinRT::SwapBuffers()
    {
        lock_guard<mutex> lock(bufferMutex);
        if (frameCurrent != frameCounter)
        {
            frameCurrent = frameCounter;
            swap(m_backBuffer, m_frontBuffer);
        }
    }

    void VideoCapture_WinRT::GrabFrameAsync(::Media::CaptureFrameGrabber^ frameGrabber)
    {
        create_task(frameGrabber->GetFrameAsync()).then([this, frameGrabber](const ComPtr<IMF2DBuffer2>& buffer)
        {
            auto width = size.width;
            auto height = size.height;

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

            frameCounter++;

            // notify frame is ready
            {
                unique_lock<mutex> lck(frameReadyMutex);
                frameReadyEvent.notify_one();
            }

            GrabFrameAsync(frameGrabber);

        }, task_continuation_context::use_current());
    }
    void VideoCapture_WinRT::start()
    {
        // temp test
        {
            listDevices();
            //vector<string> devs = listDevices();
            //for (string s : devs) {
            //    TC(s); TCNL;
            //}
        }


        auto settings = ref new MediaCaptureInitializationSettings();
        settings->StreamingCaptureMode = StreamingCaptureMode::Video; // Video-only capture

        // TBI- enumeration etc
        //Windows::Devices::Enumeration::DeviceInformationCollection> devices;
        //
        //create_task(DeviceInformation::FindAllAsync(DeviceClass::VideoCapture))
        //    .then([this, &devices, &ready](task<DeviceInformationCollection^> findTask)
        //{
        //    devices = findTask.get();

        //auto devInfo = devices.Get()->GetAt(deviceID);
        //settings->VideoDeviceId = devInfo->Id;

        //auto location = devInfo->EnclosureLocation;
        //if (location != nullptr && location->Panel == Windows::Devices::Enumeration::Panel::Front)
        //{
        //    bFlipImageX = true;
        //}

        m_capture = ref new MediaCapture();
        create_task(m_capture->InitializeAsync(settings)).then([this](){

            auto props = safe_cast<VideoEncodingProperties^>(m_capture->VideoDeviceController->GetMediaStreamProperties(MediaStreamType::VideoPreview));
            props->Subtype = MediaEncodingSubtypes::Bgra8; // Ask for color conversion to match WriteableBitmap

            auto width = size.width;
            auto height = size.height;
            props->Width = width;
            props->Height = height;

            m_frontBuffer = make_unique<WriteableBitmap^>(ref new WriteableBitmap(width, height));
            m_backBuffer = make_unique<WriteableBitmap^>(ref new WriteableBitmap(width, height));

            return ::Media::CaptureFrameGrabber::CreateAsync(m_capture.Get(), props);

        }).then([this](::Media::CaptureFrameGrabber^ frameGrabber)
        {
            started = true;
            GrabFrameAsync(frameGrabber);
        });
    }

    std::string PlatformStringToString(Platform::String^ s) {
        std::wstring t = std::wstring(s->Data());
        return std::string(t.begin(), t.end());
    }

    // NOT WORKING / NOT USED
    vector <std::string> VideoCapture_WinRT::listDevices()
    {
        TCC("listDevices"); TCNL;

        //std::atomic<bool> ready(false);

        //mutex              readyMutex;
        //condition_variable readyEvent;

        auto settings = ref new MediaCaptureInitializationSettings();

        vector < std::string > dl;

        auto a = DeviceInformation::FindAllAsync(DeviceClass::VideoCapture);

        int i = 0;

        thread t{
            [&,this]{
                TCC("t fn"); TCNL;
                create_task(DeviceInformation::FindAllAsync(DeviceClass::VideoCapture))
                    .then([&,this](task<DeviceInformationCollection^> findTask)
                {
                    TCC("A1");
                    m_devices = findTask.get();

                    TC(m_devices->Size); TCNL;
                    i = 1;
                });
            }
        };

        t.join();
        TC(i); TCNL;

        TCC("A0");
        create_task(DeviceInformation::FindAllAsync(DeviceClass::VideoCapture))
            .then([this,&dl](task<DeviceInformationCollection^> findTask)
        {
            TCC("A1");
            m_devices = findTask.get();

            TC(m_devices->Size); TCNL;

            for (size_t i = 0; i < m_devices->Size; i++)
            {
                auto d = m_devices->GetAt(i);
                TC(i);  TCSW(d->Name->Data()); TCNL;
                wstring ws(d->Name->Data());
                auto sn = string ( ws.begin(), ws.end() );
                //dl.push_back(sn);                
            }

            //ready = true;

            //unique_lock<mutex> lock(readyMutex);
            //readyEvent.notify_all();
        });

        //int c = 0;
        //while (!ready) {
        //    // wait for async task to complete
        //    //unique_lock<mutex> lock(readyMutex);
        //    //readyEvent.wait(lock);
        //    c++;
        //}

        return dl;
        //return devices;
    }

    // C interface (not implemented now)
    //CvCapture *cvCreateCameraCapture_WinRT(int index)
    //{
    //    CvCapture_WinRT* capture = new CvCapture_WinRT;
    //    if (capture->open(index))
    //        return capture;
    //    delete capture;
    //    return 0;
    //}


    void MainPage::_GrabFrameAsync(::Media::CaptureFrameGrabber^ frameGrabber)
    {
        create_task(frameGrabber->GetFrameAsync()).then([this, frameGrabber](const ComPtr<IMF2DBuffer2>& buffer)
        {
#if 0
            auto bitmap = ref new WriteableBitmap(width, height);

            CHK(buffer->ContiguousCopyTo(GetData(bitmap->PixelBuffer), bitmap->PixelBuffer->Capacity));

            unsigned long length;
            CHK(buffer->GetContiguousLength(&length));
            bitmap->PixelBuffer->Length = length;

            Preview->Source = bitmap;
#endif
            const int bytesPerPixel = 3;
            const bool copyOnly = true;

            // output
            //auto bitmap = ref new WriteableBitmap(width, height);
            auto p = m_backBuffer.get();
            auto pbOut = GetData((*p)->PixelBuffer);

            BYTE *pbScanline;
            LONG plPitch;
            unsigned int numBytes = width * bytesPerPixel;
            CHK(buffer->Lock2D(&pbScanline, &plPitch));

            {
                lock_guard<mutex> lock(m_mutex);

                if (copyOnly) {

                    // nb. no R/B swizzle seems to be needed
                    cv::Mat InputFrame(height, width, CV_8UC3 | CV_MAT_CONT_FLAG, pbScanline);
                    cv::Mat OutputFrame(height, width, CV_8UC3 | CV_MAT_CONT_FLAG, pbOut);

                    // no effect - straight copy
                    InputFrame.copyTo(OutputFrame);

                }
                else {

                    // effect
                    cv::Mat InputFrame(height, width, CV_8UC3 | CV_MAT_CONT_FLAG, pbScanline);
                    cv::Mat OutputFrame(height, width, CV_8UC3 | CV_MAT_CONT_FLAG, pbOut);

                    // not working
                    cv::Mat InputGreyScale(InputFrame, cv::Range(0, height), cv::Range(0, width));
                    OutputFrame.setTo(cv::Scalar(128));
                    cv::Mat OutputGreyScale(OutputFrame, cv::Range(0, height), cv::Range(0, width));
                    InputGreyScale.copyTo(OutputGreyScale);

                    //OutputGreyScale.copyTo(OutputFrame);

                    // does something, but incorrect
                    //cv::Mat gray(height, width, CV_8UC1);
                    //cv::cvtColor(InputFrame, gray, CV_BGR2GRAY);        // convert
                    //cv::cvtColor(gray, OutputFrame, CV_GRAY2RGB);       // unpack to BGR

                    // has a stride problem?

                    // notes
#if 0
                    // gray.copyTo(OutputFrame);

                    //OutputFrame.setTo(cv::Vec3b(128,128,128));

                    /*
                    cv::Mat InputGreyScale(InputFrame, cv::Range(0, height), cv::Range(0, width));

                    OutputFrame.setTo(cv::Scalar(128));
                    cv::Mat OutputGreyScale(OutputFrame, cv::Range(0, height), cv::Range(0, width));
                    InputGreyScale.copyTo(OutputGreyScale);

                    OutputGreyScale.copyTo(OutputFrame);
                    */
                    /*
                    Mat frame, edges;
                    namedWindow("edges",1);
                    for(;;)
                    {
                    cap >> frame;
                    cvtColor(frame, edges, CV_BGR2GRAY);
                    GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
                    Canny(edges, edges, 0, 30, 3);
                    */
                    //OutputFrame.setTo(cv::Scalar(128));
                    //cv::Mat OutputGreyScale(OutputFrame, cv::Range(0, height), cv::Range(0, width));
                    //InputGreyScale.copyTo(OutputGreyScale);

                    //OutputFrame.setTo(cv::Scalar(128));
                    //cv::Mat OutputGreyScale(OutputFrame, cv::Range(0, height), cv::Range(0, width));
                    //cv::Canny(InputGreyScale, OutputGreyScale, 80, 90);
#endif
                }
            }

            CHK(buffer->Unlock2D());

            // TODO: move to draw loop and add buffer swapping code
            Preview->Source = *m_backBuffer.get();

            // loss of camera device & restart is not yet handled

            _GrabFrameAsync(frameGrabber);
        }, task_continuation_context::use_current());
    }
#endif

    // reference
#if 0
#include "precomp.hpp"

#include <vfw.h>

#ifdef __GNUC__
#define WM_CAP_FIRSTA              (WM_USER)
#define capSendMessage(hwnd,m,w,l) (IsWindow(hwnd)?SendMessage(hwnd,m,w,l):0)
#endif

#if defined _M_X64 && defined _MSC_VER
#pragma optimize("",off)
#pragma warning(disable: 4748)
#endif

    /********************* Capturing video from AVI via VFW ************************/

    static BITMAPINFOHEADER icvBitmapHeader( int width, int height, int bpp, int compression = BI_RGB )
    {
        BITMAPINFOHEADER bmih;
        memset( &bmih, 0, sizeof(bmih));
        bmih.biSize = sizeof(bmih);
        bmih.biWidth = width;
        bmih.biHeight = height;
        bmih.biBitCount = (WORD)bpp;
        bmih.biCompression = compression;
        bmih.biPlanes = 1;

        return bmih;
    }


    static void icvInitCapture_VFW()
    {
        static int isInitialized = 0;
        if( !isInitialized )
        {
            AVIFileInit();
            isInitialized = 1;
        }
    }


    class CvCaptureAVI_VFW : public CvCapture
    {
    public:
        CvCaptureAVI_VFW()
        {
            CoInitialize(NULL);
            init();
        }

        virtual ~CvCaptureAVI_VFW()
        {
            close();
            CoUninitialize();
        }

        virtual bool open( const char* filename );
        virtual void close();

        virtual double getProperty(int);
        virtual bool setProperty(int, double);
        virtual bool grabFrame();
        virtual IplImage* retrieveFrame(int);
        virtual int getCaptureDomain() { return CV_CAP_VFW; } // Return the type of the capture object: CV_CAP_VFW, etc...

    protected:
        void init();

        PAVIFILE            avifile;
        PAVISTREAM          avistream;
        PGETFRAME           getframe;
        AVISTREAMINFO       aviinfo;
        BITMAPINFOHEADER  * bmih;
        CvSlice             film_range;
        double              fps;
        int                 pos;
        IplImage*           frame;
        CvSize              size;
    };


    void CvCaptureAVI_VFW::init()
    {
        avifile = 0;
        avistream = 0;
        getframe = 0;
        memset( &aviinfo, 0, sizeof(aviinfo) );
        bmih = 0;
        film_range = cvSlice(0,0);
        fps = 0;
        pos = 0;
        frame = 0;
        size = cvSize(0,0);
    }


    void CvCaptureAVI_VFW::close()
    {
        if( getframe )
            AVIStreamGetFrameClose( getframe );

        if( avistream )
            AVIStreamRelease( avistream );

        if( avifile )
            AVIFileRelease( avifile );

        if (frame)
            cvReleaseImage( &frame );

        init();
    }


    bool CvCaptureAVI_VFW::open( const char* filename )
    {
        close();
        icvInitCapture_VFW();

        if( !filename )
            return false;

        HRESULT hr = AVIFileOpen( &avifile, filename, OF_READ, NULL );
        if( SUCCEEDED(hr))
        {
            hr = AVIFileGetStream( avifile, &avistream, streamtypeVIDEO, 0 );
            if( SUCCEEDED(hr))
            {
                hr = AVIStreamInfo( avistream, &aviinfo, sizeof(aviinfo));
                if( SUCCEEDED(hr))
                {
                    size.width = aviinfo.rcFrame.right - aviinfo.rcFrame.left;
                    size.height = aviinfo.rcFrame.bottom - aviinfo.rcFrame.top;
                    BITMAPINFOHEADER bmihdr = icvBitmapHeader( size.width, size.height, 24 );

                    film_range.start_index = (int)aviinfo.dwStart;
                    film_range.end_index = film_range.start_index + (int)aviinfo.dwLength;
                    fps = (double)aviinfo.dwRate/aviinfo.dwScale;
                    pos = film_range.start_index;
                    getframe = AVIStreamGetFrameOpen( avistream, &bmihdr );
                    if( getframe != 0 )
                        return true;

                    // Attempt to open as 8-bit AVI.
                    bmihdr = icvBitmapHeader( size.width, size.height, 8);
                    getframe = AVIStreamGetFrameOpen( avistream, &bmihdr );
                    if( getframe != 0 )
                        return true;
                }
            }
        }

        close();
        return false;
    }

    bool CvCaptureAVI_VFW::grabFrame()
    {
        if( avistream )
            bmih = (BITMAPINFOHEADER*)AVIStreamGetFrame( getframe, pos++ );
        return bmih != 0;
    }

    IplImage* CvCaptureAVI_VFW::retrieveFrame(int)
    {
        if( avistream && bmih )
        {
            bool isColor = bmih->biBitCount == 24;
            int nChannels = (isColor) ? 3 : 1;
            IplImage src;
            cvInitImageHeader( &src, cvSize( bmih->biWidth, bmih->biHeight ),
                IPL_DEPTH_8U, nChannels, IPL_ORIGIN_BL, 4 );

            char* dataPtr = (char*)(bmih + 1);

            // Only account for the color map size if we are an 8-bit image and the color map is used
            if (!isColor)
            {
                static int RGBQUAD_SIZE_PER_BYTE = sizeof(RGBQUAD)/sizeof(BYTE);
                int offsetFromColormapToData = (int)bmih->biClrUsed*RGBQUAD_SIZE_PER_BYTE;
                dataPtr += offsetFromColormapToData;
            }

            cvSetData( &src, dataPtr, src.widthStep );

            if( !frame || frame->width != src.width || frame->height != src.height )
            {
                cvReleaseImage( &frame );
                frame = cvCreateImage( cvGetSize(&src), 8, nChannels );
            }

            cvFlip( &src, frame, 0 );
            return frame;
        }

        return 0;
    }

    double CvCaptureAVI_VFW::getProperty( int property_id )
    {
        switch( property_id )
        {
        case CV_CAP_PROP_POS_MSEC:
            return cvRound(pos*1000./fps);
        case CV_CAP_PROP_POS_FRAMES:
            return pos;
        case CV_CAP_PROP_POS_AVI_RATIO:
            return (pos - film_range.start_index)/
                (film_range.end_index - film_range.start_index + 1e-10);
        case CV_CAP_PROP_FRAME_WIDTH:
            return size.width;
        case CV_CAP_PROP_FRAME_HEIGHT:
            return size.height;
        case CV_CAP_PROP_FPS:
            return fps;
        case CV_CAP_PROP_FOURCC:
            return aviinfo.fccHandler;
        case CV_CAP_PROP_FRAME_COUNT:
            return film_range.end_index - film_range.start_index;
        }
        return 0;
    }

    bool CvCaptureAVI_VFW::setProperty( int property_id, double value )
    {
        switch( property_id )
        {
        case CV_CAP_PROP_POS_MSEC:
        case CV_CAP_PROP_POS_FRAMES:
        case CV_CAP_PROP_POS_AVI_RATIO:
        {
            switch( property_id )
            {
            case CV_CAP_PROP_POS_MSEC:
                pos = cvRound(value*fps*0.001);
                break;
            case CV_CAP_PROP_POS_AVI_RATIO:
                pos = cvRound(value*(film_range.end_index -
                    film_range.start_index) +
                    film_range.start_index);
                break;
            default:
                pos = cvRound(value);
            }
            if( pos < film_range.start_index )
                pos = film_range.start_index;
            if( pos > film_range.end_index )
                pos = film_range.end_index;
        }
            break;
        default:
            return false;
        }

        return true;
    }

    CvCapture* cvCreateFileCapture_VFW (const char* filename)
    {
        CvCaptureAVI_VFW* capture = new CvCaptureAVI_VFW;
        if( capture->open(filename) )
            return capture;
        delete capture;
        return 0;
    }


    /********************* Capturing video from camera via VFW *********************/

    class CvCaptureCAM_VFW : public CvCapture
    {
    public:
        CvCaptureCAM_VFW() { init(); }
        virtual ~CvCaptureCAM_VFW() { close(); }

        virtual bool open( int index );
        virtual void close();
        virtual double getProperty(int);
        virtual bool setProperty(int, double) { return false; }
        virtual bool grabFrame();
        virtual IplImage* retrieveFrame(int);
        virtual int getCaptureDomain() { return CV_CAP_VFW; } // Return the type of the capture object: CV_CAP_VFW, etc...

    protected:
        void init();
        void closeHIC();
        static LRESULT PASCAL frameCallback( HWND hWnd, VIDEOHDR* hdr );

        CAPDRIVERCAPS caps;
        HWND   capWnd;
        VIDEOHDR* hdr;
        DWORD  fourcc;
        HIC    hic;
        IplImage* frame;
    };


    void CvCaptureCAM_VFW::init()
    {
        memset( &caps, 0, sizeof(caps) );
        capWnd = 0;
        hdr = 0;
        fourcc = 0;
        hic = 0;
        frame = 0;
    }

    void CvCaptureCAM_VFW::closeHIC()
    {
        if( hic )
        {
            ICDecompressEnd( hic );
            ICClose( hic );
            hic = 0;
        }
    }


    LRESULT PASCAL CvCaptureCAM_VFW::frameCallback( HWND hWnd, VIDEOHDR* hdr )
    {
        CvCaptureCAM_VFW* capture = 0;

        if (!hWnd) return FALSE;

        capture = (CvCaptureCAM_VFW*)capGetUserData(hWnd);
        capture->hdr = hdr;

        return (LRESULT)TRUE;
    }


    // Initialize camera input
    bool CvCaptureCAM_VFW::open( int wIndex )
    {
        char szDeviceName[80];
        char szDeviceVersion[80];
        HWND hWndC = 0;

        close();

        if( (unsigned)wIndex >= 10 )
            wIndex = 0;

        for( ; wIndex < 10; wIndex++ )
        {
            if( capGetDriverDescription( wIndex, szDeviceName,
                sizeof (szDeviceName), szDeviceVersion,
                sizeof (szDeviceVersion)))
            {
                hWndC = capCreateCaptureWindow ( "My Own Capture Window",
                    WS_POPUP | WS_CHILD, 0, 0, 320, 240, 0, 0);
                if( capDriverConnect (hWndC, wIndex))
                    break;
                DestroyWindow( hWndC );
                hWndC = 0;
            }
        }

        if( hWndC )
        {
            capWnd = hWndC;
            hdr = 0;
            hic = 0;
            fourcc = (DWORD)-1;

            memset( &caps, 0, sizeof(caps));
            capDriverGetCaps( hWndC, &caps, sizeof(caps));
            ::MoveWindow( hWndC, 0, 0, 320, 240, TRUE );
            capSetUserData( hWndC, (size_t)this );
            capSetCallbackOnFrame( hWndC, frameCallback );
            CAPTUREPARMS p;
            capCaptureGetSetup(hWndC,&p,sizeof(CAPTUREPARMS));
            p.dwRequestMicroSecPerFrame = 66667/2;
            capCaptureSetSetup(hWndC,&p,sizeof(CAPTUREPARMS));
            //capPreview( hWndC, 1 );
            capPreviewScale(hWndC,FALSE);
            capPreviewRate(hWndC,1);
        }
        return capWnd != 0;
    }


    void CvCaptureCAM_VFW::close()
    {
        if( capWnd )
        {
            capSetCallbackOnFrame( capWnd, NULL );
            capDriverDisconnect( capWnd );
            DestroyWindow( capWnd );
            closeHIC();
        }
        cvReleaseImage( &frame );
        init();
    }


    bool CvCaptureCAM_VFW::grabFrame()
    {
        if( capWnd )
        {
            SendMessage( capWnd, WM_CAP_GRAB_FRAME_NOSTOP, 0, 0 );
            return true;
        }
        return false;
    }


    IplImage* CvCaptureCAM_VFW::retrieveFrame(int)
    {
        BITMAPINFO vfmt;
        memset( &vfmt, 0, sizeof(vfmt));
        BITMAPINFOHEADER& vfmt0 = vfmt.bmiHeader;
        int sz, prevWidth, prevHeight;

        if( !capWnd )
            return 0;

        sz = capGetVideoFormat( capWnd, &vfmt, sizeof(vfmt));
        prevWidth = frame ? frame->width : 0;
        prevHeight = frame ? frame->height : 0;

        if( !hdr || hdr->lpData == 0 || sz == 0 )
            return 0;

        if( !frame || frame->width != vfmt0.biWidth || frame->height != vfmt0.biHeight )
        {
            cvReleaseImage( &frame );
            frame = cvCreateImage( cvSize( vfmt0.biWidth, vfmt0.biHeight ), 8, 3 );
        }

        if( vfmt.bmiHeader.biCompression != BI_RGB ||
            vfmt.bmiHeader.biBitCount != 24 )
        {
            BITMAPINFOHEADER vfmt1 = icvBitmapHeader( vfmt0.biWidth, vfmt0.biHeight, 24 );

            if( hic == 0 || fourcc != vfmt0.biCompression ||
                prevWidth != vfmt0.biWidth || prevHeight != vfmt0.biHeight )
            {
                closeHIC();
                hic = ICOpen( MAKEFOURCC('V','I','D','C'),
                    vfmt0.biCompression, ICMODE_DECOMPRESS );
                if( hic )
                {
                    if( ICDecompressBegin( hic, &vfmt0, &vfmt1 ) != ICERR_OK )
                    {
                        closeHIC();
                        return 0;
                    }
                }
            }

            if( !hic || ICDecompress( hic, 0, &vfmt0, hdr->lpData,
                &vfmt1, frame->imageData ) != ICERR_OK )
            {
                closeHIC();
                return 0;
            }

            cvFlip( frame, frame, 0 );
        }
        else
        {
            IplImage src;
            cvInitImageHeader( &src, cvSize(vfmt0.biWidth, vfmt0.biHeight),
                IPL_DEPTH_8U, 3, IPL_ORIGIN_BL, 4 );
            cvSetData( &src, hdr->lpData, src.widthStep );
            cvFlip( &src, frame, 0 );
        }

        return frame;
    }


    double CvCaptureCAM_VFW::getProperty( int property_id )
    {
        switch( property_id )
        {
        case CV_CAP_PROP_FRAME_WIDTH:
            return frame ? frame->width : 0;
        case CV_CAP_PROP_FRAME_HEIGHT:
            return frame ? frame->height : 0;
        case CV_CAP_PROP_FOURCC:
            return fourcc;
        }
        return 0;
    }


    CvCapture* cvCreateCameraCapture_VFW( int index )
    {
        CvCaptureCAM_VFW* capture = new CvCaptureCAM_VFW;

        if( capture->open( index ))
            return capture;

        delete capture;
        return 0;
    }


    /*************************** writing AVIs ******************************/

    class CvVideoWriter_VFW : public CvVideoWriter
    {
    public:
        CvVideoWriter_VFW() { init(); }
        virtual ~CvVideoWriter_VFW() { close(); }

        virtual bool open( const char* filename, int fourcc,
            double fps, CvSize frameSize, bool isColor );
        virtual void close();
        virtual bool writeFrame( const IplImage* );

    protected:
        void init();
        bool createStreams( CvSize frameSize, bool isColor );

        PAVIFILE      avifile;
        PAVISTREAM    compressed;
        PAVISTREAM    uncompressed;
        double        fps;
        IplImage*     tempFrame;
        long          pos;
        int           fourcc;
    };


    void CvVideoWriter_VFW::init()
    {
        avifile = 0;
        compressed = uncompressed = 0;
        fps = 0;
        tempFrame = 0;
        pos = 0;
        fourcc = 0;
    }

    void CvVideoWriter_VFW::close()
    {
        if( uncompressed )
            AVIStreamRelease( uncompressed );
        if( compressed )
            AVIStreamRelease( compressed );
        if( avifile )
            AVIFileRelease( avifile );
        cvReleaseImage( &tempFrame );
        init();
    }


    // philipg.  Made this code capable of writing 8bpp gray scale bitmaps
    struct BITMAPINFO_8Bit
    {
        BITMAPINFOHEADER bmiHeader;
        RGBQUAD          bmiColors[256];
    };


    bool CvVideoWriter_VFW::open( const char* filename, int _fourcc, double _fps, CvSize frameSize, bool isColor )
    {
        close();

        icvInitCapture_VFW();
        if( AVIFileOpen( &avifile, filename, OF_CREATE | OF_WRITE, 0 ) == AVIERR_OK )
        {
            fourcc = _fourcc;
            fps = _fps;
            if( frameSize.width > 0 && frameSize.height > 0 &&
                !createStreams( frameSize, isColor ) )
            {
                close();
                return false;
            }
            return true;
        }
        else
            return false;
    }


    bool CvVideoWriter_VFW::createStreams( CvSize frameSize, bool isColor )
    {
        if( !avifile )
            return false;
        AVISTREAMINFO aviinfo;

        BITMAPINFO_8Bit bmih;
        bmih.bmiHeader = icvBitmapHeader( frameSize.width, frameSize.height, isColor ? 24 : 8 );
        for( int i = 0; i < 256; i++ )
        {
            bmih.bmiColors[i].rgbBlue = (BYTE)i;
            bmih.bmiColors[i].rgbGreen = (BYTE)i;
            bmih.bmiColors[i].rgbRed = (BYTE)i;
            bmih.bmiColors[i].rgbReserved = 0;
        }

        memset( &aviinfo, 0, sizeof(aviinfo));
        aviinfo.fccType = streamtypeVIDEO;
        aviinfo.fccHandler = 0;
        // use highest possible accuracy for dwRate/dwScale
        aviinfo.dwScale = (DWORD)((double)0x7FFFFFFF / fps);
        aviinfo.dwRate = cvRound(fps * aviinfo.dwScale);
        aviinfo.rcFrame.top = aviinfo.rcFrame.left = 0;
        aviinfo.rcFrame.right = frameSize.width;
        aviinfo.rcFrame.bottom = frameSize.height;

        if( AVIFileCreateStream( avifile, &uncompressed, &aviinfo ) == AVIERR_OK )
        {
            AVICOMPRESSOPTIONS copts, *pcopts = &copts;
            copts.fccType = streamtypeVIDEO;
            copts.fccHandler = fourcc != -1 ? fourcc : 0;
            copts.dwKeyFrameEvery = 1;
            copts.dwQuality = 10000;
            copts.dwBytesPerSecond = 0;
            copts.dwFlags = AVICOMPRESSF_VALID;
            copts.lpFormat = &bmih;
            copts.cbFormat = (isColor ? sizeof(BITMAPINFOHEADER) : sizeof(bmih));
            copts.lpParms = 0;
            copts.cbParms = 0;
            copts.dwInterleaveEvery = 0;

            if( fourcc != -1 || AVISaveOptions( 0, 0, 1, &uncompressed, &pcopts ) == TRUE )
            {
                if( AVIMakeCompressedStream( &compressed, uncompressed, pcopts, 0 ) == AVIERR_OK &&
                    AVIStreamSetFormat( compressed, 0, &bmih, sizeof(bmih)) == AVIERR_OK )
                {
                    fps = fps;
                    fourcc = (int)copts.fccHandler;
                    frameSize = frameSize;
                    tempFrame = cvCreateImage( frameSize, 8, (isColor ? 3 : 1) );
                    return true;
                }
            }
        }
        return false;
    }


    bool CvVideoWriter_VFW::writeFrame( const IplImage* image )
    {
        bool result = false;
        CV_FUNCNAME( "CvVideoWriter_VFW::writeFrame" );

        __BEGIN__;

        if( !image )
            EXIT;

        if( !compressed && !createStreams( cvGetSize(image), image->nChannels > 1 ))
            EXIT;

        if( image->width != tempFrame->width || image->height != tempFrame->height )
            CV_ERROR( CV_StsUnmatchedSizes,
            "image size is different from the currently set frame size" );

        if( image->nChannels != tempFrame->nChannels ||
            image->depth != tempFrame->depth ||
            image->origin == 0 ||
            image->widthStep != cvAlign(image->width*image->nChannels*((image->depth & 255)/8), 4))
        {
            cvConvertImage( image, tempFrame, image->origin == 0 ? CV_CVTIMG_FLIP : 0 );
            image = (const IplImage*)tempFrame;
        }

        result = AVIStreamWrite( compressed, pos++, 1, image->imageData,
            image->imageSize, AVIIF_KEYFRAME, 0, 0 ) == AVIERR_OK;

        __END__;

        return result;
    }

    CvVideoWriter* cvCreateVideoWriter_VFW( const char* filename, int fourcc,
        double fps, CvSize frameSize, int isColor )
    {
        CvVideoWriter_VFW* writer = new CvVideoWriter_VFW;
        if( writer->open( filename, fourcc, fps, frameSize, isColor != 0 ))
            return writer;
        delete writer;
        return 0;
    }

#endif


