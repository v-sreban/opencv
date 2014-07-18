#include <pch.h>
#include <ppltasks.h>
#include <collection.h>
#include <vector>
#include <algorithm>


#include <opencv2\imgproc\types_c.h>
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace concurrency;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

namespace OpenCVLib
{
    public ref class WinRTComponent sealed
    {
    public:
        IAsyncOperation<IVectorView<unsigned char>^>^ ProcessAsync(IVectorView<unsigned char>^ input, int width, int height)
        {
            // Synchronously copy input data from host to device
            int size = input->Size;

            cv::Mat mat(width, height, CV_8UC4);

            return create_async([=]() -> IVectorView<unsigned char>^
            {

                // convert to grayscale
                cv::Mat intermediateMat;
                cv::cvtColor(mat, intermediateMat, CV_RGB2GRAY);

                // convert to BGRA
                cv::cvtColor(intermediateMat, mat, CV_GRAY2BGRA);

                std::vector<unsigned char> vec = std::vector<unsigned char>(size);
                vec.assign(mat.data, mat.data + size);

                // Return the outputs as a VectorView<float>
                return ref new Platform::Collections::VectorView<unsigned char>(vec);
            });
        }

    };
}

#if 0
// OpenCVComponent.cpp
#include "pch.h"
#include "OpenCVComponent.h"
#include <algorithm>
#include <collection.h>

#include <opencv2\imgproc\types_c.h>
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace OpenCVComponent;
using namespace Platform;
using namespace Windows::Foundation::Collections;

WindowsPhoneRuntimeComponent::WindowsPhoneRuntimeComponent()
{
}


Windows::Foundation::Collections::IVector<int>^ WindowsPhoneRuntimeComponent::Process(IVector<int>^ vec, int width, int height)
{
    // create a matrix the size and type of the image
    cv::Mat mat(width, height, CV_8UC4);

    std::copy(begin(vec), end(vec), mat.data);

    // convert to grayscale
    cv::Mat intermediateMat;
    cv::cvtColor(mat, intermediateMat, CV_RGB2GRAY);

    // convert to BGRA
    cv::cvtColor(intermediateMat, mat, CV_GRAY2BGRA);

    int end = width * height * 4;
    IVector<int>^ vec = ref new IVector<int>();

    for (int i = 0; i < end; i++)
    {
        vec->Append(mat.data[i]);
    }

    return vec;
}
#endif