#pragma once

#include <vector>

bool DDSImageConvert(CString sourceFile,CString destFile,EPVRTPixelFormat pixelFormat,pvrtexture::ECompressorQuality compressQuality);

bool TGAImageConvert(CString sourceFile,CString destFile,EPVRTPixelFormat pixelFormat,pvrtexture::ECompressorQuality compressQuality);

bool ImageConvert(CString relativeSrcPath,CString srcDir,CString destDir,EPVRTPixelFormat pixelFormat,pvrtexture::ECompressorQuality compressQuality);

/*bool ImageConvert(CString relativeSrcPath,CString& relativeDestPath,CString srcDir,CString destDir);

bool MF1FileConvert(CString relativeSrcPath,CString srcDir,CString destDir);

bool MapFileConvert(CString relativeSrcPath,CString srcDir,CString destDir);*/

bool ImageGetInfo(CString relativeSrcPath,CString& relativeDestPath,CString srcDir,CString destDir);
bool MF1FileGetInfo(CString relativeSrcPath,CString srcDir,CString destDir,std::vector<CString>& srcTexVec);
bool BF1FileGetInfo(CString relativeSrcPath,CString srcDir,CString destDir);
bool MapFileGetInfo(CString relativeSrcPath,CString srcDir,CString destDir,std::vector<CString>& srcTexVec);