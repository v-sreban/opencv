md %1 %2
md %1\opencv
copy %2\%3\include\opencv\*.h %1\opencv /y
copy %2\%3\include\opencv\*.hpp %1\opencv /y
md %1\opencv2
md %1\opencv2\calib3d
copy %2\%3\modules\calib3d\include\opencv2\calib3d\*.h %1\opencv2\calib3d /y
copy %2\%3\modules\calib3d\include\opencv2\calib3d\*.hpp %1\opencv2\calib3d /y
md %1\opencv2\core
copy %2\%3\modules\core\include\opencv2\core.hpp %1\opencv2 /y
copy %2\%3\modules\core\include\opencv2\core\*.h %1\opencv2\core /y
copy %2\%3\modules\core\include\opencv2\core\*.hpp %1\opencv2\core /y
md %1\opencv2\features2d
copy %2\%3\modules\features2d\include\opencv2\features2d.hpp %1\opencv2 /y
copy %2\%3\modules\features2d\include\opencv2\features2d\*.h %1\opencv2\features2d /y
copy %2\%3\modules\features2d\include\opencv2\features2d\*.hpp %1\opencv2\features2d /y
md %1\opencv2\flann
copy %2\%3\modules\flann\include\opencv2\flann\*.h %1\opencv2\flann /y
copy %2\%3\modules\flann\include\opencv2\flann\*.hpp %1\opencv2\flann /y
md %1\opencv2\gpu
copy %2\%3\modules\gpu\include\opencv2\gpu\*.h %1\opencv2\gpu /y
copy %2\%3\modules\gpu\include\opencv2\gpu\*.hpp %1\opencv2\gpu /y
md %1\opencv2\highgui
copy %2\%3\modules\highgui\include\opencv2\highgui\*.h %1\opencv2\highgui /y
copy %2\%3\modules\highgui\include\opencv2\highgui\*.hpp %1\opencv2\highgui /y
md %1\opencv2\imgproc
copy %2\%3\modules\imgproc\include\opencv2\imgproc\*.h %1\opencv2\imgproc /y
copy %2\%3\modules\imgproc\include\opencv2\imgproc\*.hpp %1\opencv2\imgproc /y
md %1\opencv2\legacy
copy %2\%3\modules\legacy\include\opencv2\legacy.hpp %1\opencv2 /y
copy %2\%3\modules\legacy\include\opencv2\legacy\*.h %1\opencv2\legacy /y
copy %2\%3\modules\legacy\include\opencv2\legacy\*.hpp %1\opencv2\legacy /y
md %1\opencv2\ml
copy %2\%3\modules\ml\include\opencv2\ml.hpp %1\opencv2 /y
copy %2\%3\modules\ml\include\opencv2\ml\*.h %1\opencv2\ml /y
copy %2\%3\modules\ml\include\opencv2\ml\*.hpp %1\opencv2\ml /y
md %1\opencv2\objdetect
copy %2\%3\modules\objdetect\include\opencv2\objdetect\*.h %1\opencv2\objdetect /y
copy %2\%3\modules\objdetect\include\opencv2\objdetect\*.hpp %1\opencv2\objdetect /y
md %1\opencv2\ts
copy %2\%3\modules\ts\include\opencv2\ts\*.h %1\opencv2\ts /y
copy %2\%3\modules\ts\include\opencv2\ts\*.hpp %1\opencv2\ts /y
md %1\opencv2\photo
copy %2\%3\modules\photo\include\opencv2\photo\*.h %1\opencv2\photo /y
copy %2\%3\modules\photo\include\opencv2\photo\*.hpp %1\opencv2\photo /y
md %1\opencv2\video
copy %2\%3\modules\video\include\opencv2\video\*.h %1\opencv2\video /y
copy %2\%3\modules\video\include\opencv2\video\*.hpp %1\opencv2\video /y