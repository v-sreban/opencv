<#
Copyright © Microsoft Open Technologies, Inc.
All Rights Reserved        
Licensed under the Apache License, Version 2.0 (the "License"); 
you may not use this file except in compliance with the License. 

You may obtain a copy of the License at 
http://www.apache.org/licenses/LICENSE-2.0 
 
THIS CODE IS PROVIDED ON AN *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, 
FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT. 
 
See the Apache 2 License for the specific language governing permissions and limitations under the License.
#>

. ".\scripts\createsln.ps1"
. ".\scripts\converterutils.ps1"
. ".\scripts\winrtconverter.ps1"

#$WorkingDir = (Get-Location).Path
$WorkingDir = $PSScriptRoot
$InputDir = join-path $WorkingDir "..\bin" -Resolve
$WinRTOutputDir = $WorkingDir
$WP8OutputDir = join-path $WorkingDir "..\binWP8"

Function Convert($OutputDir, $platform)
{
    Write-Host "Converting " $platform " projects..."

    $includehack = GetDirectoryName($OutputDir)


    $zlib = "3rdparty\zlib\zlib.vcxproj"
    $jpeg = "3rdparty\libjpeg\libjpeg.vcxproj"
    $jasper = "3rdparty\libjasper\libjasper.vcxproj"
    $tiff = "3rdparty\libtiff\libtiff.vcxproj"
    $png = "3rdparty\libpng\libpng.vcxproj"
    $core = "modules\core\opencv_core.vcxproj"
    $imgproc = "modules\imgproc\opencv_imgproc.vcxproj"
    $flann = "modules\flann\opencv_flann.vcxproj"
    $photo = "modules\photo\opencv_photo.vcxproj"
    $calib3d = "modules\calib3d\opencv_calib3d.vcxproj"
    $ml = "modules\ml\opencv_ml.vcxproj"
    $objdetect = "modules\objdetect\opencv_objdetect.vcxproj"
    $video = "modules\video\opencv_video.vcxproj"
    $videostab = "modules\videostab\opencv_videostab.vcxproj"
    $features2d = "modules\features2d\opencv_features2d.vcxproj"
    $stitching = "modules\stitching\opencv_stitching.vcxproj"

    $projects = ($zlib, $jpeg, $tiff, $jasper, $png, $core, $imgproc, $flann, $photo, $calib3d, $ml, $objdetect, $video, $videostab, $features2d, $stitching)

    foreach($project in $projects)
    {

        $in = join-path $InputDir $project
        $out = join-path $OutputDir $project
        try
        {
            Write-Host ("Converting: " + $project)
            WinrtConverter $in $out $platform $includehack >> $null
        }
        catch
        {
            Write-Error ("Did not convert " + $project)
            Write-Error ("Error: " + $_)
        }
    }

    try
    {
        $allProjects = @()
        Copy-Item (join-path $InputDir "cvconfig.h") (join-path $OutputDir "cvconfig.h")

        $dir = join-path $OutputDir "opencv2"
        New-Item -ItemType Directory -Force -Path $dir >> $null
        Copy-Item (join-path $InputDir "opencv2\opencv_modules.hpp") (join-path $OutputDir "opencv2\opencv_modules.hpp")

        $allProjects += $zlibProject = join-path $OutputDir -childpath $zlib
        Copy-Item (join-path $InputDir "3rdparty\zlib\zconf.h") (join-path $OutputDir "3rdparty\zlib\zconf.h")

        $allProjects += $jpegProject = join-path $OutputDir -childpath  $jpeg
        $allProjects += $jasperProject = join-path $OutputDir -childpath $jasper
        $openexrProject = join-path $OutputDir -childpath "3rdparty\openexr\IlmImf.vcxproj"

        $allProjects += $tiffProject = join-path $OutputDir -childpath $tiff
        ReplaceText $tiffProject "tif_win32.c" "tif_unix.c"
        ReplaceText ($tiffProject + ".filters") "tif_win32.c" "tif_unix.c"
        Copy-Item (join-path $InputDir "3rdparty\libtiff\tif_config.h") (join-path $OutputDir "3rdparty\libtiff\tif_config.h")

        $allProjects += $pngProject = join-path $OutputDir -childpath $png
        $xml = LoadXmlFile($pngProject)
        AddPreprocessorDefine $xml "HAVE_WINRT"
        $xml.Save($pngProject)

        #opencv_core.vcxproj

        $allProjects += $coreProject = join-path $OutputDir -childpath $core
        $inputProject = join-path $InputDir -childpath $core
        $inDir = Split-Path -parent $inputProject
        $outDir = Split-Path -parent $coreProject
        $xml = LoadXmlFile($coreProject)
        Copy-Item (join-path $inDir "opencv_core_pch.cpp") (join-path $outDir "opencv_core_pch.cpp") 
        Copy-Item (join-path $InputDir "modules\core\opencl_kernels.cpp") (join-path $OutputDir "modules\core\opencl_kernels.cpp") 
        Copy-Item (join-path $InputDir "modules\core\opencl_kernels.hpp") (join-path $OutputDir "modules\core\opencl_kernels.hpp")
        Copy-Item (join-path $InputDir "modules\core\version_string.inc") (join-path $OutputDir "modules\core\version_string.inc")
 
        #opencv_flann.vcxproj
        $allProjects += $flannProject = join-path $OutputDir -childpath $flann
        Copy-Item (join-path $InputDir "modules\flann\opencv_flann_pch.cpp") (join-path $OutputDir "modules\flann\opencv_flann_pch.cpp")
 
        #opencv_imgproc.vcxproj
        $allProjects += $imgprocProject = join-path $OutputDir -childpath $imgproc
        Copy-Item (join-path $InputDir "modules\imgproc\opencv_imgproc_pch.cpp") (join-path $OutputDir "modules\imgproc\opencv_imgproc_pch.cpp")
        Copy-Item (join-path $InputDir "modules\imgproc\opencl_kernels.cpp") (join-path $OutputDir "modules\imgproc\opencl_kernels.cpp") 
        Copy-Item (join-path $InputDir "modules\imgproc\opencl_kernels.hpp") (join-path $OutputDir "modules\imgproc\opencl_kernels.hpp")

        $allProjects += $photoProject = join-path $OutputDir -childpath $photo
        Copy-Item (join-path $InputDir "modules\photo\opencv_photo_pch.cpp") (join-path $OutputDir "modules\photo\opencv_photo_pch.cpp") 
        Copy-Item (join-path $InputDir "modules\photo\opencl_kernels.cpp") (join-path $OutputDir "modules\photo\opencl_kernels.cpp") 
        Copy-Item (join-path $InputDir "modules\photo\opencl_kernels.hpp") (join-path $OutputDir "modules\photo\opencl_kernels.hpp")

        $allProjects += $calib3dProject = join-path $OutputDir -childpath $calib3d
        Copy-Item (join-path $InputDir "modules\calib3d\opencv_calib3d_pch.cpp") (join-path $OutputDir "modules\calib3d\opencv_calib3d_pch.cpp") 
        Copy-Item (join-path $InputDir "modules\calib3d\opencl_kernels.cpp") (join-path $OutputDir "modules\calib3d\opencl_kernels.cpp") 
        Copy-Item (join-path $InputDir "modules\calib3d\opencl_kernels.hpp") (join-path $OutputDir "modules\calib3d\opencl_kernels.hpp")

        $allProjects += $mlProject = join-path $OutputDir -childpath $ml
        Copy-Item (join-path $InputDir "modules\ml\opencv_ml_pch.cpp") (join-path $OutputDir "modules\ml\opencv_ml_pch.cpp")

        #opencv_objdetect.vcxproj
        $allProjects += $objdetectProject = join-path $OutputDir -childpath $objdetect
        Copy-Item (join-path $InputDir "modules\objdetect\opencv_objdetect_pch.cpp") (join-path $OutputDir "modules\objdetect\opencv_objdetect_pch.cpp")
        Copy-Item (join-path $InputDir "modules\objdetect\opencl_kernels.cpp") (join-path $OutputDir "modules\objdetect\opencl_kernels.cpp") 
        Copy-Item (join-path $InputDir "modules\objdetect\opencl_kernels.hpp") (join-path $OutputDir "modules\objdetect\opencl_kernels.hpp")

        #opencv_video.vcxproj
        $allProjects += $videoProject = join-path $OutputDir -childpath $video
        Copy-Item (join-path $InputDir "modules\video\opencv_video_pch.cpp") (join-path $OutputDir "modules\video\opencv_video_pch.cpp")
        Copy-Item (join-path $InputDir "modules\video\opencl_kernels.cpp") (join-path $OutputDir "modules\video\opencl_kernels.cpp") 
        Copy-Item (join-path $InputDir "modules\video\opencl_kernels.hpp") (join-path $OutputDir "modules\video\opencl_kernels.hpp")

        #opencv_videostab.vcxproj
        $allProjects += $videostabProject = join-path $OutputDir -childpath $videostab
        Copy-Item (join-path $InputDir "modules\videostab\opencv_videostab_pch.cpp") (join-path $OutputDir "modules\videostab\opencv_videostab_pch.cpp")
 
        #opencv_features2d.vcxproj
        $allProjects += $features2dProject = join-path $OutputDir -childpath $features2d
        Copy-Item (join-path $InputDir "modules\features2d\opencv_features2d_pch.cpp") (join-path $OutputDir "modules\features2d\opencv_features2d_pch.cpp") 
        Copy-Item (join-path $InputDir "modules\features2d\opencl_kernels.cpp") (join-path $OutputDir "modules\features2d\opencl_kernels.cpp") 
        Copy-Item (join-path $InputDir "modules\features2d\opencl_kernels.hpp") (join-path $OutputDir "modules\features2d\opencl_kernels.hpp")

        #opencv_stitching.vcxproj
        $allProjects += $stitchingProject = join-path $OutputDir -childpath $stitching
        Copy-Item (join-path $InputDir "modules\stitching\opencv_stitching_pch.cpp") (join-path $OutputDir "modules\stitching\opencv_stitching_pch.cpp") 
        Copy-Item (join-path $InputDir "modules\stitching\opencl_kernels.cpp") (join-path $OutputDir "modules\stitching\opencl_kernels.cpp") 
        Copy-Item (join-path $InputDir "modules\stitching\opencl_kernels.hpp") (join-path $OutputDir "modules\stitching\opencl_kernels.hpp")

        #create libpng sln and project references
        $pngDir = Split-Path -parent $pngProject
        AddProjectReference $pngProject $zlibProject >> $null
        $output = join-path $pngDir "libpng.sln"
        CreateSolutionFile $output $platform ($pngProject, $zlibProject)

        #create libtiff sln and project references
        $tiffDir = Split-Path -parent $tiffProject
        AddProjectReference $tiffProject $zlibProject >> $null
        $output = join-path $tiffDir "libtiff.sln"
        CreateSolutionFile $output $platform ($tiffProject, $zlibProject)

        #create opencv_core sln and project references
        $coreDir = Split-Path -parent $coreProject
        AddProjectReference $coreProject $zlibProject >> $null
        $output = join-path $coreDir "opencv_core.sln"
        CreateSolutionFile $output $platform ($coreProject, $zlibProject)

        #create opencv_flann sln and project references
        $flannDir = Split-Path -parent $flannProject
        AddProjectReference $flannProject $coreProject >> $null
        $output = join-path $flannDir "opencv_flann.sln"
        CreateSolutionFile $output $platform ($flannProject, $coreProject, $zlibProject)

        #create opencv_imgproc sln and project references
        $imgprocDir = Split-Path -parent $imgprocProject
        AddProjectReference $imgprocProject $coreProject >> $null
        $output = join-path $imgprocDir "opencv_imgproc.sln"
        CreateSolutionFile $output $platform ($imgprocProject, $coreProject, $zlibProject)

        #create opencv_photo sln and project references
        $photoDir = Split-Path -parent $photoProject
        AddProjectReference $photoProject ($coreProject, $imgprocProject) >> $null
        $output = join-path $photoDir "opencv_photo.sln"
        CreateSolutionFile $output $platform ($photoProject, $imgprocProject, $coreProject, $zlibProject)

        #create opencv_features2d sln and project references
        $features2dDir = Split-Path -parent $features2dProject
        AddProjectReference $features2dProject ($coreProject,  $imgprocProject, $flannProject) >> $null
        $output = join-path $features2dDir "features2d.sln"
        CreateSolutionFile $output $platform ($features2dProject, $coreProject, $imgprocProject, $flannProject, $zlibProject, $pngProject, $tiffProject, $jpegProject)

        #create opencv_calib3d sln and project references
        $calib3dDir = Split-Path -parent $calib3dProject
        AddProjectReference $calib3dProject ($coreProject, $imgprocProject, $flannProject, $features2dProject) >> $null
        $output = join-path $calib3dDir "opencv_calib3d.sln"
        CreateSolutionFile $output $platform ($features2dProject, $coreProject, $imgprocProject, $flannProject, $zlibProject, $pngProject, $tiffProject, $jpegProject)

        #create opencv_ml sln and project references
        $mlDir = Split-Path -parent $mlProject
        AddProjectReference $mlProject $coreProject >> $null
        $output = join-path $mlDir "opencv_ml.sln"
        CreateSolutionFile $output $platform ($mlProject, $coreProject, $zlibProject)

        #create opencv_objdetect sln and project references
        $objdetectDir = Split-Path -parent $objdetectProject
        AddProjectReference $objdetectProject  ($coreProject, $imgprocProject, $mlProject) >> $null
        $output = join-path $objdetectDir "opencv_objdetect.sln"
        CreateSolutionFile $output $platform ($objdetectProject, $coreProject, $imgprocProject, $mlProject, $zlibProject)

        #create opencv_video sln and project references
        $videoDir = Split-Path -parent $videoProject
        AddProjectReference $videoProject  ($coreProject, $imgprocProject) >> $null
        $output = join-path $videoDir "opencv_video.sln"
        CreateSolutionFile $output $platform ($videoProject, $coreProject, $imgprocProject, $zlibProject)

        #create opencv_videostab sln and project references
        $videostabDir = Split-Path -parent $videostabProject
        AddProjectReference $videostabProject  ($coreProject, $calib3dProject,$features2dProject, $flannProject, $imgprocProject, $photoProject, $videoProject) >> $null
        $output = join-path $videostabDir "opencv_videostab.sln"
        CreateSolutionFile $output $platform ($videostabProject, $coreProject, $calib3dProject,$features2dProject, $flannProject, $imgprocProject, $photoProject, $videoProject, $zlibProject)

        #create opencv_stitching sln and project references
        $stitchingDir = Split-Path -parent $stitchingProject
        AddProjectReference $stitchingProject ($coreProject, $calib3dProject, $imgprocProject, $flannProject, $features2dProject, $mlProject, $objdetectProject) >> $null
        $output = join-path $stitchingDir "opencv_stitching.sln"
        CreateSolutionFile $output $platform ($stitchingProject, $coreProject, $calib3dProject, $imgprocProject, $flannProject, $features2dProject, $mlProject, $objdetectProject, $zlibProject)

        #create opencv sln 
        $output = join-path $OutputDir "opencv.sln"
        CreateSolutionFile $output $platform ($allProjects)
 
        Write-Host "Conversion complete."
    }
    catch
    {
        Write-Error ("Error: " + $_)
    }
}

#convert Win32 OpenCV projects to WinRT and WP8 
Convert $WinRTOutputDir "winrt"
Convert $WP8OutputDir "wp8"


# SIG # Begin signature block
# MIIasQYJKoZIhvcNAQcCoIIaojCCGp4CAQExCzAJBgUrDgMCGgUAMGkGCisGAQQB
# gjcCAQSgWzBZMDQGCisGAQQBgjcCAR4wJgIDAQAABBAfzDtgWUsITrck0sYpfvNR
# AgEAAgEAAgEAAgEAAgEAMCEwCQYFKw4DAhoFAAQUR9PUzI7UkYoxvQ2LbyEJBLXR
# AKqgghWMMIIEwzCCA6ugAwIBAgITMwAAAEyh6E3MtHR7OwAAAAAATDANBgkqhkiG
# 9w0BAQUFADB3MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4G
# A1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSEw
# HwYDVQQDExhNaWNyb3NvZnQgVGltZS1TdGFtcCBQQ0EwHhcNMTMxMTExMjIxMTMx
# WhcNMTUwMjExMjIxMTMxWjCBszELMAkGA1UEBhMCVVMxEzARBgNVBAgTCldhc2hp
# bmd0b24xEDAOBgNVBAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jw
# b3JhdGlvbjENMAsGA1UECxMETU9QUjEnMCUGA1UECxMebkNpcGhlciBEU0UgRVNO
# OkMwRjQtMzA4Ni1ERUY4MSUwIwYDVQQDExxNaWNyb3NvZnQgVGltZS1TdGFtcCBT
# ZXJ2aWNlMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAsdj6GwYrd6jk
# lF18D+Z6ppLuilQdpPmEdYWXzMtcltDXdS3ZCPtb0u4tJcY3PvWrfhpT5Ve+a+i/
# ypYK3EbxWh4+AtKy4CaOAGR7vjyT+FgyeYfSGl0jvJxRxA8Q+gRYtRZ2buy8xuW+
# /K2swUHbqs559RyymUGneiUr/6t4DVg6sV5Q3mRM4MoVKt+m6f6kZi9bEAkJJiHU
# Pw0vbdL4d5ADbN4UEqWM5zYf9IelsEEXb+NNdGbC/aJxRjVRzGsXUWP6FZSSml9L
# KLrmFkVJ6Sy1/ouHr/ylbUPcpjD6KSjvmw0sXIPeEo1qtNtx71wUWiojKP+BcFfx
# jAeaE9gqUwIDAQABo4IBCTCCAQUwHQYDVR0OBBYEFLkNrbNN9NqfGrInJlUNIETY
# mOL0MB8GA1UdIwQYMBaAFCM0+NlSRnAK7UD7dvuzK7DDNbMPMFQGA1UdHwRNMEsw
# SaBHoEWGQ2h0dHA6Ly9jcmwubWljcm9zb2Z0LmNvbS9wa2kvY3JsL3Byb2R1Y3Rz
# L01pY3Jvc29mdFRpbWVTdGFtcFBDQS5jcmwwWAYIKwYBBQUHAQEETDBKMEgGCCsG
# AQUFBzAChjxodHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20vcGtpL2NlcnRzL01pY3Jv
# c29mdFRpbWVTdGFtcFBDQS5jcnQwEwYDVR0lBAwwCgYIKwYBBQUHAwgwDQYJKoZI
# hvcNAQEFBQADggEBAAmKTgav6O2Czx0HftcqpyQLLa+aWyR/lHEMVYgkGlIVY+KQ
# TQVKmEqc++GnbWhVgrkp6mmpstXjDNrR1nolN3hnHAz72ylaGpc4KjlWRvs1gbnk
# PUZajuT8dTdYWUmLTts8FZ1zUkvreww6wi3Bs5tSLeA1xbnBV7PoPaE8RPIjFh4K
# qlk3J9CVUl6ofz9U8IHh3Jq9ZdV49vdMObvd4NY3DpGah4xz53FkUvc+A9jGzXK4
# NDSYW4zT9Qim63jGUaANDm/0azxAGmAWLKkGUp0cE5DObwIe6nucs/b4l2DyZdHR
# H4c6wXXwQo167Yxysnv7LIq0kUdU4i5pzBZUGlkwggT2MIID3qADAgECAhMzAAAA
# sYcLAdKYyt7cAAEAAACxMA0GCSqGSIb3DQEBBQUAMHkxCzAJBgNVBAYTAlVTMRMw
# EQYDVQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVN
# aWNyb3NvZnQgQ29ycG9yYXRpb24xIzAhBgNVBAMTGk1pY3Jvc29mdCBDb2RlIFNp
# Z25pbmcgUENBMB4XDTEzMDEyNDIyMzM0MFoXDTE0MDQyNDIyMzM0MFowgY0xCzAJ
# BgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25k
# MR4wHAYDVQQKExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xDTALBgNVBAsTBE1PUFIx
# KDAmBgNVBAMTH01pY3Jvc29mdCBPcGVuIFRlY2hub2xvZ2llcyBJbmMwggEiMA0G
# CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCkVodaEkoE75vSJ4V7ym0w54qQCXrL
# 886QuQjJG+Qmn2TW/1CeMnFdRHKR0N7wGDVts9WoKxRABHiJo8onaAUg75S0JSc+
# Rv37fMaQ5Vf6OgH7XGZc+yMzcNUA2ucufmq5CXLezxcPEGZE1rRVmvuMzupXOJmM
# XkSRJ+k16CE2Z49jTNTw2xHt/eGszOPxU2AAzehCc5ZFNS8sm42egnxniQxVck37
# MK/tqZx1RS8VjukL01H4PYr35nKkWU3UHUo4nv8M8QR4kQUfDMH1xnkNP64oG6rq
# 5lo45kEWP9esz9uWHlycdNyCaMunpyUDSC3Sq8vEi93mTfci/j6wrop1AgMBAAGj
# ggFgMIIBXDATBgNVHSUEDDAKBggrBgEFBQcDAzAdBgNVHQ4EFgQUpgtP/aOj5EBJ
# 6d/gwZxEexA2fyMwUQYDVR0RBEowSKRGMEQxDTALBgNVBAsTBE1PUFIxMzAxBgNV
# BAUTKjMxNjA0K2I3NDk1MTMxLTQ2NWMtNGM0Yy1hMmRhLTcxN2FkMWM0NGY5YzAf
# BgNVHSMEGDAWgBTLEejK0rQWWAHJNy4zFha5TJoKHzBWBgNVHR8ETzBNMEugSaBH
# hkVodHRwOi8vY3JsLm1pY3Jvc29mdC5jb20vcGtpL2NybC9wcm9kdWN0cy9NaWND
# b2RTaWdQQ0FfMDgtMzEtMjAxMC5jcmwwWgYIKwYBBQUHAQEETjBMMEoGCCsGAQUF
# BzAChj5odHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20vcGtpL2NlcnRzL01pY0NvZFNp
# Z1BDQV8wOC0zMS0yMDEwLmNydDANBgkqhkiG9w0BAQUFAAOCAQEAaYnhEYD4B2Vt
# /JyTUb6Uq6aX88HEndmVgx5l7aBEL4w+xhRqXPllaoXkrEV4RrN1ShOuiD1kTbOe
# XEMniPZXEJJDu30sTMHTrkqIoKvZ6Jx7Vg0ery18dFL/32E+Ar78zp/FN7gxHeBl
# 9MVsg8uortjzcSgG8AgCoyfo8DN8cvLo6l4DdXCAROqb+DkAXJCj8BnDouG/eNBZ
# lp3ujhH9SDzqoe6TScf3WYBuGVkANNXlY03NT2r0VUo6MZ9588KCLO4aRkY0hlEB
# 4L7I/m9rk5zD9JPproTNQifg2hoXi/F+sTS0miax+cygdj0g2ixs+1qt/PdxRsRf
# eKIMdi9anzCCBbwwggOkoAMCAQICCmEzJhoAAAAAADEwDQYJKoZIhvcNAQEFBQAw
# XzETMBEGCgmSJomT8ixkARkWA2NvbTEZMBcGCgmSJomT8ixkARkWCW1pY3Jvc29m
# dDEtMCsGA1UEAxMkTWljcm9zb2Z0IFJvb3QgQ2VydGlmaWNhdGUgQXV0aG9yaXR5
# MB4XDTEwMDgzMTIyMTkzMloXDTIwMDgzMTIyMjkzMloweTELMAkGA1UEBhMCVVMx
# EzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1vbmQxHjAcBgNVBAoT
# FU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEjMCEGA1UEAxMaTWljcm9zb2Z0IENvZGUg
# U2lnbmluZyBQQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCycllc
# GTBkvx2aYCAgQpl2U2w+G9ZvzMvx6mv+lxYQ4N86dIMaty+gMuz/3sJCTiPVcgDb
# NVcKicquIEn08GisTUuNpb15S3GbRwfa/SXfnXWIz6pzRH/XgdvzvfI2pMlcRdyv
# rT3gKGiXGqelcnNW8ReU5P01lHKg1nZfHndFg4U4FtBzWwW6Z1KNpbJpL9oZC/6S
# dCnidi9U3RQwWfjSjWL9y8lfRjFQuScT5EAwz3IpECgixzdOPaAyPZDNoTgGhVxO
# VoIoKgUyt0vXT2Pn0i1i8UU956wIAPZGoZ7RW4wmU+h6qkryRs83PDietHdcpRee
# jcsRj1Y8wawJXwPTAgMBAAGjggFeMIIBWjAPBgNVHRMBAf8EBTADAQH/MB0GA1Ud
# DgQWBBTLEejK0rQWWAHJNy4zFha5TJoKHzALBgNVHQ8EBAMCAYYwEgYJKwYBBAGC
# NxUBBAUCAwEAATAjBgkrBgEEAYI3FQIEFgQU/dExTtMmipXhmGA7qDFvpjy82C0w
# GQYJKwYBBAGCNxQCBAweCgBTAHUAYgBDAEEwHwYDVR0jBBgwFoAUDqyCYEBWJ5fl
# JRP8KuEKU5VZ5KQwUAYDVR0fBEkwRzBFoEOgQYY/aHR0cDovL2NybC5taWNyb3Nv
# ZnQuY29tL3BraS9jcmwvcHJvZHVjdHMvbWljcm9zb2Z0cm9vdGNlcnQuY3JsMFQG
# CCsGAQUFBwEBBEgwRjBEBggrBgEFBQcwAoY4aHR0cDovL3d3dy5taWNyb3NvZnQu
# Y29tL3BraS9jZXJ0cy9NaWNyb3NvZnRSb290Q2VydC5jcnQwDQYJKoZIhvcNAQEF
# BQADggIBAFk5Pn8mRq/rb0CxMrVq6w4vbqhJ9+tfde1MOy3XQ60L/svpLTGjI8x8
# UJiAIV2sPS9MuqKoVpzjcLu4tPh5tUly9z7qQX/K4QwXaculnCAt+gtQxFbNLeNK
# 0rxw56gNogOlVuC4iktX8pVCnPHz7+7jhh80PLhWmvBTI4UqpIIck+KUBx3y4k74
# jKHK6BOlkU7IG9KPcpUqcW2bGvgc8FPWZ8wi/1wdzaKMvSeyeWNWRKJRzfnpo1hW
# 3ZsCRUQvX/TartSCMm78pJUT5Otp56miLL7IKxAOZY6Z2/Wi+hImCWU4lPF6H0q7
# 0eFW6NB4lhhcyTUWX92THUmOLb6tNEQc7hAVGgBd3TVbIc6YxwnuhQ6MT20OE049
# fClInHLR82zKwexwo1eSV32UjaAbSANa98+jZwp0pTbtLS8XyOZyNxL0b7E8Z4L5
# UrKNMxZlHg6K3RDeZPRvzkbU0xfpecQEtNP7LN8fip6sCvsTJ0Ct5PnhqX9Guwdg
# R2VgQE6wQuxO7bN2edgKNAltHIAxH+IOVN3lofvlRxCtZJj/UBYufL8FIXrilUEn
# acOTj5XJjdibIa4NXJzwoq6GaIMMai27dmsAHZat8hZ79haDJLmIz2qoRzEvmtzj
# cT3XAH5iR9HOiMm4GPoOco3Boz2vAkBq/2mbluIQqBC0N1AI1sM9MIIGBzCCA++g
# AwIBAgIKYRZoNAAAAAAAHDANBgkqhkiG9w0BAQUFADBfMRMwEQYKCZImiZPyLGQB
# GRYDY29tMRkwFwYKCZImiZPyLGQBGRYJbWljcm9zb2Z0MS0wKwYDVQQDEyRNaWNy
# b3NvZnQgUm9vdCBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkwHhcNMDcwNDAzMTI1MzA5
# WhcNMjEwNDAzMTMwMzA5WjB3MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGlu
# Z3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBv
# cmF0aW9uMSEwHwYDVQQDExhNaWNyb3NvZnQgVGltZS1TdGFtcCBQQ0EwggEiMA0G
# CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCfoWyx39tIkip8ay4Z4b3i48WZUSNQ
# rc7dGE4kD+7Rp9FMrXQwIBHrB9VUlRVJlBtCkq6YXDAm2gBr6Hu97IkHD/cOBJjw
# icwfyzMkh53y9GccLPx754gd6udOo6HBI1PKjfpFzwnQXq/QsEIEovmmbJNn1yjc
# RlOwhtDlKEYuJ6yGT1VSDOQDLPtqkJAwbofzWTCd+n7Wl7PoIZd++NIT8wi3U21S
# tEWQn0gASkdmEScpZqiX5NMGgUqi+YSnEUcUCYKfhO1VeP4Bmh1QCIUAEDBG7bfe
# I0a7xC1Un68eeEExd8yb3zuDk6FhArUdDbH895uyAc4iS1T/+QXDwiALAgMBAAGj
# ggGrMIIBpzAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQWBBQjNPjZUkZwCu1A+3b7
# syuwwzWzDzALBgNVHQ8EBAMCAYYwEAYJKwYBBAGCNxUBBAMCAQAwgZgGA1UdIwSB
# kDCBjYAUDqyCYEBWJ5flJRP8KuEKU5VZ5KShY6RhMF8xEzARBgoJkiaJk/IsZAEZ
# FgNjb20xGTAXBgoJkiaJk/IsZAEZFgltaWNyb3NvZnQxLTArBgNVBAMTJE1pY3Jv
# c29mdCBSb290IENlcnRpZmljYXRlIEF1dGhvcml0eYIQea0WoUqgpa1Mc1j0BxMu
# ZTBQBgNVHR8ESTBHMEWgQ6BBhj9odHRwOi8vY3JsLm1pY3Jvc29mdC5jb20vcGtp
# L2NybC9wcm9kdWN0cy9taWNyb3NvZnRyb290Y2VydC5jcmwwVAYIKwYBBQUHAQEE
# SDBGMEQGCCsGAQUFBzAChjhodHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20vcGtpL2Nl
# cnRzL01pY3Jvc29mdFJvb3RDZXJ0LmNydDATBgNVHSUEDDAKBggrBgEFBQcDCDAN
# BgkqhkiG9w0BAQUFAAOCAgEAEJeKw1wDRDbd6bStd9vOeVFNAbEudHFbbQwTq86+
# e4+4LtQSooxtYrhXAstOIBNQmd16QOJXu69YmhzhHQGGrLt48ovQ7DsB7uK+jwoF
# yI1I4vBTFd1Pq5Lk541q1YDB5pTyBi+FA+mRKiQicPv2/OR4mS4N9wficLwYTp2O
# awpylbihOZxnLcVRDupiXD8WmIsgP+IHGjL5zDFKdjE9K3ILyOpwPf+FChPfwgph
# jvDXuBfrTot/xTUrXqO/67x9C0J71FNyIe4wyrt4ZVxbARcKFA7S2hSY9Ty5Zliz
# LS/n+YWGzFFW6J1wlGysOUzU9nm/qhh6YinvopspNAZ3GmLJPR5tH4LwC8csu89D
# s+X57H2146SodDW4TsVxIxImdgs8UoxxWkZDFLyzs7BNZ8ifQv+AeSGAnhUwZuhC
# El4ayJ4iIdBD6Svpu/RIzCzU2DKATCYqSCRfWupW76bemZ3KOm+9gSd0BhHudiG/
# m4LBJ1S2sWo9iaF2YbRuoROmv6pH8BJv/YoybLL+31HIjCPJZr2dHYcSZAI9La9Z
# j7jkIeW1sMpjtHhUBdRBLlCslLCleKuzoJZ1GtmShxN1Ii8yqAhuoFuMJb+g74TK
# IdbrHk/Jmu5J4PcBZW+JC33Iacjmbuqnl84xKf8OxVtc2E0bodj6L54/LlUWa8kT
# o/0xggSPMIIEiwIBATCBkDB5MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGlu
# Z3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBv
# cmF0aW9uMSMwIQYDVQQDExpNaWNyb3NvZnQgQ29kZSBTaWduaW5nIFBDQQITMwAA
# ALGHCwHSmMre3AABAAAAsTAJBgUrDgMCGgUAoIGoMBkGCSqGSIb3DQEJAzEMBgor
# BgEEAYI3AgEEMBwGCisGAQQBgjcCAQsxDjAMBgorBgEEAYI3AgEVMCMGCSqGSIb3
# DQEJBDEWBBQ4G3LpspQoGMK3X9SiC6Q9K/O9vjBIBgorBgEEAYI3AgEMMTowOKAO
# gAwATwBwAGUAbgBDAFahJoAkaHR0cHM6Ly9naXRodWIuY29tL01TT3BlblRlY2gv
# b3BlbmN2MA0GCSqGSIb3DQEBAQUABIIBADQg03yh4XYeRmNRvoWdrBgpsQv+KyNH
# 86l8BnnZDK3dmiql7uyKUPF1qre/4lkO14BXwVzI8Z+apkg+HJZDE480gHGTlK+q
# YYwWaVB0BLhccgsNsLMF2zrmiwJhHayJUPD2bdH6YWAttb/YQ32uxdb7ry8pTIf+
# c0xdiPK2pn6fjfiQTOPTmhvULzLc24Tf8MwZuyaoh0BPcJb6BkaC18oej2GQrHvz
# ZASZ8IORUjY6LBX/S6QuwmASf/f9N0WX9r6bFstEpNhW8wVi62U5kjD0WyBVN1Cm
# qxg7T0ADK5hK07GD288fgD9HMdZCvIxM/6bJH8sLmo/ySWVsYLHiDCyhggIoMIIC
# JAYJKoZIhvcNAQkGMYICFTCCAhECAQEwgY4wdzELMAkGA1UEBhMCVVMxEzARBgNV
# BAgTCldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jv
# c29mdCBDb3Jwb3JhdGlvbjEhMB8GA1UEAxMYTWljcm9zb2Z0IFRpbWUtU3RhbXAg
# UENBAhMzAAAATKHoTcy0dHs7AAAAAABMMAkGBSsOAwIaBQCgXTAYBgkqhkiG9w0B
# CQMxCwYJKoZIhvcNAQcBMBwGCSqGSIb3DQEJBTEPFw0xNDAzMTkxNjQ5NTFaMCMG
# CSqGSIb3DQEJBDEWBBTUf6AgrgZlMxC1qYFCPC/gHpFwtzANBgkqhkiG9w0BAQUF
# AASCAQCxeictcmGf+1m8PWvm1cHpMWI9PWg50QPWqexZkKreRsg604YzZaevHjwP
# JC6WwflCSukaXk1kcARlN1F2/NTwKK5kb/v2BqGd342BibwxVP9G34byYACSCOqS
# S8gKdfI33zRpZnyG6AE7vtTh99s6Dd5sNNpY8bYbtS725wQNh/T9Erk7JVuaRSkP
# 4yb1y0KbvcpClOYp06bSuZLeLU7jNZGPMr9SZ8eX+3lXYt+gxjgdTfCwdhW1/iEf
# FRrw7At1qoCgwMKdI61cSEnNFsXxrtxexjMVVEJfgx1TBEVf5fE59lRxeruma0UJ
# HYGzMkoZ+IRk7gnnBUxB6KBbOdpI
# SIG # End signature block
