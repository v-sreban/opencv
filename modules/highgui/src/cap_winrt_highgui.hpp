
#include <ppl.h>
#include <concrt.h>
#include <atomic>

//using namespace Platform;
//using namespace ::concurrency;
//using namespace ::Windows::Foundation;

// this is an OpenCV class
// stub
namespace cv
{
    class Mat
    {
    };
}


enum {
    HighguiBridge_OPEN_CAMERA = 300,
    HighguiBridge_CLOSE_CAMERA,
    HighguiBridge_UPDATE_IMAGE_ELEMENT,
    HighGuiAssist_SHOW_TRACKBAR
};


// singleton
class HighguiBridge
{
public:
    static HighguiBridge& getInstance()
    {
        static HighguiBridge instance;
        return instance;
    }
    void perform(int action) { reporter.report(action); }
    void setReporter(Concurrency::progress_reporter<int> pr) { reporter = pr; }

    // highgui video interface
    bool initializeDevice() {
        // store capture = ref new MediaCapture...
        // call init capture device async using deviceIndex
        // .then( [] { ...
        deviceReady = true;
        return true;
        // });
    }

    // highgui UI interface
    void createTrackbar( /* callbackOnChange */) {} // unhides trackbar and registers OpenCV callback
    void setTrackbarPos(int pos) {}                 // unhides trackbar and sets its position

    void imshow(cv::Mat matToShow) {}                   // shows Mat in the cvImage element
    // namedWindow                                  // no op: only one XAML window used
    // destroyWindow                                // no op
    // destroyAllWindows                            // no op
    void MoveWindow( /* int x, int y */) {}         // moves the cvImage element on MainPage
    void ResizeWindow( /* int w, int h */) {}       // resizes the cvImage element on MainPage

    void setMouseCallback( /* callback, userdata */) {}
    // calls OpenCV callback when mouse moves inside of cvImage element on MainPage

    int waitKey(int delay = 0) { return 0; }

    // setOpenGlDrawCallback                        // no op
    // setOpenGlContext                             // no op
    // updateWindow                                 // no op

    // end highgui UI interface

    // MediaCapture ^capture;
    int deviceIndex;

private:
    HighguiBridge() {
        deviceReady = false;
    };
    HighguiBridge(HighguiBridge const &);
    void operator=(const HighguiBridge &);

    Concurrency::progress_reporter<int> reporter;
    std::atomic<bool> deviceReady;
};
