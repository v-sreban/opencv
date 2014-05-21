REM usage: run from github directory 

call CopyOcvIncHHPP NuGetPackageWinRT ..
call CopyOcvLibWinRT . NuGetPackageWinRT
call CopyOcvDllWinRT . NuGetPackageWinRT


