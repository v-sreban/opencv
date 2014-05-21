REM usage: run from github directory 

call CopyOcvIncHHPP .\NuGetPackageWP8 ..
call CopyOcvLibWP8 . \NuGetPackageWP8
call CopyOcvDllWP8 . \NuGetPackageWP8

call CopyOcvIncHHPP .\NuGetPackageWinRT ..
call CopyOcvLibWinRT . \NuGetPackageWinRT
call CopyOcvDllWinRT . \NuGetPackageWinRT

