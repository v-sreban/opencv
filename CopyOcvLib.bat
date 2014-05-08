md %2\addons\ofxOpenCV\libs\opencv\lib\winrt
md %2\addons\ofxOpenCV\libs\opencv\lib\winrt\Debug
md %2\addons\ofxOpenCV\libs\opencv\lib\winrt\Release

copy %1\binWinRT\ARM\Debug\zlib\zlib.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Debug /y
copy %1\binWinRT\ARM\Debug\opencv_calib3d\opencv_calib3d.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Debug /y
copy %1\binWinRT\ARM\Debug\opencv_contrib\opencv_contrib.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Debug /y
copy %1\binWinRT\ARM\Debug\opencv_core\opencv_core.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Debug /y
copy %1\binWinRT\ARM\Debug\opencv_features2d\opencv_features2d.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Debug /y
copy %1\binWinRT\ARM\Debug\opencv_flann\opencv_flann.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Debug /y
copy %1\binWinRT\ARM\Debug\opencv_imgproc\opencv_imgproc.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Debug /y
copy %1\binWinRT\ARM\Debug\opencv_legacy\opencv_legacy.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Debug /y
copy %1\binWinRT\ARM\Debug\opencv_ml\opencv_ml.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Debug /y
copy %1\binWinRT\ARM\Debug\opencv_objdetect\opencv_objdetect.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Debug /y
copy %1\binWinRT\ARM\Debug\opencv_photo\opencv_photo.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Debug /y
copy %1\binWinRT\ARM\Debug\opencv_stitching\opencv_stitching.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Debug /y
copy %1\binWinRT\ARM\Debug\opencv_video\opencv_video.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Debug /y
copy %1\binWinRT\ARM\Debug\opencv_videostab\opencv_videostab.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Debug /y

REM - Until PS1 file issue fixed:
copy %1\binWinRT\modules\legacy\ARM\Debug\opencv_legacy\opencv_legacy.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Debug /y
copy %1\binWinRT\modules\contrib\ARM\Debug\opencv_contrib\opencv_contrib.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Debug /y


copy opencv-pr\binWinRT\ARM\Release\zlib\zlib.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Release /y
copy opencv-pr\binWinRT\ARM\Release\opencv_calib3d\opencv_calib3d.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Release /y
copy opencv-pr\binWinRT\ARM\Release\opencv_contrib\opencv_contrib.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Release /y
copy opencv-pr\binWinRT\ARM\Release\opencv_core\opencv_core.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Release /y
copy opencv-pr\binWinRT\ARM\Release\opencv_features2d\opencv_features2d.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Release /y
copy opencv-pr\binWinRT\ARM\Release\opencv_flann\opencv_flann.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Release /y
copy opencv-pr\binWinRT\ARM\Release\opencv_imgproc\opencv_imgproc.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Release /y
copy opencv-pr\binWinRT\ARM\Release\opencv_legacy\opencv_legacy.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Release /y
copy opencv-pr\binWinRT\ARM\Release\opencv_ml\opencv_ml.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Release /y
copy opencv-pr\binWinRT\ARM\Release\opencv_objdetect\opencv_objdetect.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Release /y
copy opencv-pr\binWinRT\ARM\Release\opencv_photo\opencv_photo.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Release /y
copy opencv-pr\binWinRT\ARM\Release\opencv_stitching\opencv_stitching.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Release /y
copy opencv-pr\binWinRT\ARM\Release\opencv_video\opencv_video.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Release /y
copy opencv-pr\binWinRT\ARM\Release\opencv_videostab\opencv_videostab.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Release /y

copy opencv-pr\binWinRT\modules\legacy\ARM\Release\opencv_legacy\opencv_legacy.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Release /y
copy opencv-pr\binWinRT\modules\contrib\ARM\Release\opencv_contrib\opencv_contrib.lib openframeworks\addons\ofxOpenCV\libs\opencv\lib\winrt\Release /y


