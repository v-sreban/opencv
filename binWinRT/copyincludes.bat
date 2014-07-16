

rm -rf opencv2

set IN_DIR=.\..\modules\
set OUT_DIR=.\opencv2\

xcopy "..\bin\opencv2" "%OUT_DIR%" /eiycq
xcopy "%IN_DIR%calib3d\include\opencv2" "%OUT_DIR%" /eiycq
xcopy "%IN_DIR%contrib\include\opencv2" "%OUT_DIR%" /eiycq
xcopy "%IN_DIR%core\include\opencv2" "%OUT_DIR%" /eiycq
xcopy "%IN_DIR%features2d\include\opencv2" "%OUT_DIR%" /eiycq
xcopy "%IN_DIR%flann\include\opencv2" "%OUT_DIR%" /eiycq
xcopy "%IN_DIR%imgproc\include\opencv2" "%OUT_DIR%" /eiycq
xcopy "%IN_DIR%legacy\include\opencv2" "%OUT_DIR%" /eiycq
xcopy "%IN_DIR%ml\include\opencv2" "%OUT_DIR%" /eiycq
xcopy "%IN_DIR%objdetect\include\opencv2" "%OUT_DIR%" /eiycq
xcopy "%IN_DIR%photo\include\opencv2" "%OUT_DIR%" /eiycq
xcopy "%IN_DIR%shape\include\opencv2" "%OUT_DIR%" /eiycq
xcopy "%IN_DIR%stitching\include\opencv2" "%OUT_DIR%" /eiycq
xcopy "%IN_DIR%ts\include\opencv2" "%OUT_DIR%" /eiycq
xcopy "%IN_DIR%video\include\opencv2" "%OUT_DIR%" /eiycq
xcopy "%IN_DIR%videostab\include\opencv2" "%OUT_DIR%" /eiycq






