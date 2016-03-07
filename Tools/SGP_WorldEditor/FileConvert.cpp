#include "stdafx.h"

#include "FileConvert.h"
#include "PVRTextureUtilities.h"
#include "HelpFunction.h"
#include "..\SGPLibraryCode\SGPHeader.h"
#include <set>

bool DDSImageConvert(CString sourceFile,CString destFile,EPVRTPixelFormat pixelFormat,pvrtexture::ECompressorQuality compressQuality)
{
	if(pixelFormat==ePVRTPF_PVRTCI_2bpp_RGB||pixelFormat==ePVRTPF_PVRTCI_4bpp_RGB)
	{
		// register texture
		CSGPTextureManager* pTextureManager=WorldEditorRenderInterface::GetInstance()->GetRenderDevice()->GetTextureManager();
		uint32 texID=pTextureManager->registerTexture(String(LPCTSTR(sourceFile)));
		if(texID==0) return false;
		CTextureResource* pTextureResource=pTextureManager->getTextureByID(texID);
		ISGPTexture* pTexture=pTextureResource->pSGPTexture;
		if(pTexture->hasAlpha())
		{
			if(pixelFormat==ePVRTPF_PVRTCI_2bpp_RGB) pixelFormat=ePVRTPF_PVRTCI_2bpp_RGBA;
			if(pixelFormat==ePVRTPF_PVRTCI_4bpp_RGB) pixelFormat=ePVRTPF_PVRTCI_2bpp_RGBA;
		}
		pTextureManager->unRegisterTextureByID(texID);
	}

	using namespace pvrtexture;
	CPVRTexture cTexture((const char*)(LPCTSTR)sourceFile);
	if(!Transcode(cTexture,pixelFormat,ePVRTVarTypeUnsignedByteNorm,ePVRTCSpacelRGB,compressQuality)) return false;
	cTexture.saveFile(LPCTSTR(destFile));
	return true;
}

bool TGAImageConvert(CString sourceFile,CString destFile,EPVRTPixelFormat pixelFormat,pvrtexture::ECompressorQuality compressQuality)
{
	// register texture
	CSGPTextureManager* pTextureManager=WorldEditorRenderInterface::GetInstance()->GetRenderDevice()->GetTextureManager();
	uint32 texID=pTextureManager->registerTexture(String(LPCTSTR(sourceFile)));
	if(texID==0) return false;
	CTextureResource* pTextureResource=pTextureManager->getTextureByID(texID);
	ISGPTexture* pTexture=pTextureResource->pSGPTexture;
	if(!pTexture->isTexture2D())
	{
		pTextureManager->unRegisterTextureByID(texID);
		return false;
	}
	if(pTexture->hasAlpha())
	{
		if(pixelFormat==ePVRTPF_PVRTCI_2bpp_RGB) pixelFormat=ePVRTPF_PVRTCI_2bpp_RGBA;
		if(pixelFormat==ePVRTPF_PVRTCI_4bpp_RGB) pixelFormat=ePVRTPF_PVRTCI_2bpp_RGBA;
	}

	// get size info
	SDimension2D textureSize=pTexture->getSize();
	uint32 texWidth=textureSize.Width,texHeight=textureSize.Height;

	uint32* pData=new uint32[texWidth*texHeight];
	pTexture->getMipmapData(pData,0,SGPTT_TEXTURE_2D);

	using namespace pvrtexture;
	PixelType argbType;
	argbType=PixelType('b','g','r','a',8,8,8,8);
	CPVRTextureHeader cHeader(argbType.PixelTypeID,texHeight,texWidth);
	CPVRTexture cTexture(cHeader,pData);
	if(!Transcode(cTexture,pixelFormat,ePVRTVarTypeUnsignedByteNorm,ePVRTCSpacelRGB,compressQuality))
	{
		delete [] pData;
		pTextureManager->unRegisterTextureByID(texID);
		return false;
	}
	cTexture.saveFile((LPCTSTR)destFile);

	delete [] pData;
	pTextureManager->unRegisterTextureByID(texID);
	return true;
}

bool ImageConvert(CString relativeSrcPath,CString srcDir,CString destDir,EPVRTPixelFormat pixelFormat,pvrtexture::ECompressorQuality compressQuality)
{
	CString strExt=PathFindExtension(relativeSrcPath);
	if(strExt.IsEmpty()) return false;
	CString relativeDestPath=relativeSrcPath.Left(relativeSrcPath.ReverseFind('.'))+".pvr";
	strExt.MakeLower();
	if(strExt==".dds") return DDSImageConvert(srcDir+"\\"+relativeSrcPath,destDir+"\\"+relativeDestPath,pixelFormat,compressQuality);
	else if(strExt==".tga") return TGAImageConvert(srcDir+"\\"+relativeSrcPath,destDir+"\\"+relativeDestPath,pixelFormat,compressQuality);
	else return false;
}

/*bool ImageConvert(CString relativeSrcPath,CString& relativeDestPath,CString srcDir,CString destDir)
{
	CString srcPath=srcDir+"\\"+relativeSrcPath;
	if(!PathFileExists(srcPath)) return false;
	int pos=relativeSrcPath.ReverseFind('.');
	if(pos==-1) return false;
	relativeDestPath=relativeSrcPath.Left(pos)+".pvr";
	CString destPath=destDir+"\\"+relativeDestPath;
	pos=destPath.ReverseFind('\\');
	if(pos==-1) return false;
	CString destFolder;
	destFolder=destPath.Left(pos);
	if(!CheckCreateDirectory(destFolder)) return false;

	CString fileExt=PathFindExtension(relativeSrcPath);
	fileExt.MakeLower();
	if(fileExt==".dds") return DDSImageConvert(srcPath,destPath);
	else if(fileExt==".tga") return TGAImageConvert(srcPath,destPath);
	else return false;
}

bool MF1FileConvert(CString relativeSrcPath,CString srcDir,CString destDir)
{
	CString srcPath=srcDir+"\\"+relativeSrcPath;
	CString destPath=destDir+"\\"+relativeSrcPath;
	if(!PathFileExists(srcPath)) return false;
	int pos=destPath.ReverseFind('\\');
	if(pos==-1) return false;
	CString destFolder=destPath.Left(pos);
	if(!CheckCreateDirectory(destFolder)) return false;
	if(!CopyFile(srcPath,destPath,FALSE)) return false;

	bool bSuccess;
	CSGPModelMF1* pModelMF1=NULL;
	uint8* rawMemory=CSGPModelMF1::LoadMF1(pModelMF1,String(""),String(LPCTSTR(destPath)));
	if(pModelMF1!=NULL)
	{
		// Skin
		for(uint32 i=0;i<pModelMF1->m_Header.m_iNumSkins;++i)
		{
			SGPMF1Skin& skin=pModelMF1->m_pSkins[i];
			CString relativeSrcPath1=skin.m_cName;
			CString relativeDestPath1;
			if(ImageConvert(relativeSrcPath1,relativeDestPath1,srcDir,destDir))
				strcpy_s(skin.m_cName,sizeof(skin.m_cName),(LPCTSTR)relativeDestPath1);
		}

		// Config Setting
		for(uint32 i=0;i<pModelMF1->m_Header.m_iNumConfigs;++i)
		{
			SGPMF1ConfigSetting& configSetting=pModelMF1->m_pConfigSetting[i];
			for(uint32 j=0;j<configSetting.ReplaceTextureConfigNum;++j)
			{
				SGPMF1ConfigSetting::ReplaceTextureConfig& texConfig=configSetting.pReplaceTextureConfigList[j];
				CString relativeSrcPath1=texConfig.TexFilename;
				CString relativeDestPath1;
				if(ImageConvert(relativeSrcPath1,relativeDestPath1,srcDir,destDir))
					strcpy_s(texConfig.TexFilename,sizeof(texConfig.TexFilename),(LPCTSTR)relativeDestPath1);
			}
		}

		// Particle Setting
		for(uint32 i=0;i<pModelMF1->m_Header.m_iNumParticles;++i)
		{
			ParticleSystemParam& systemParam=pModelMF1->m_pParticleEmitter[i].m_SystemParam;
			for(uint32 j=0;j<systemParam.m_groupCount;++j)
			{
				ParticleRenderParam& renderParam=systemParam.m_pGroupParam[j].m_RenderParam;
				if(renderParam.m_type==Render_Point)
				{
					ParticlePointRenderData& pointData=renderParam.m_pointData;
					CString relativeSrcPath1=pointData.m_texPath;
					CString relativeDestPath1;
					if(ImageConvert(relativeSrcPath1,relativeDestPath1,srcDir,destDir))
						strcpy_s(pointData.m_texPath,sizeof(pointData.m_texPath),(LPCTSTR)relativeDestPath1);
				}
				else if(renderParam.m_type==Render_Quad)
				{
					ParticleQuadRenderData& quadData=renderParam.m_quadData;
					CString relativeSrcPath1=quadData.m_texPath;
					CString relativeDestPath1;
					if(ImageConvert(relativeSrcPath1,relativeDestPath1,srcDir,destDir))
						strcpy_s(quadData.m_texPath,sizeof(quadData.m_texPath),(LPCTSTR)relativeDestPath1);
				}
			}
		}
		bSuccess=pModelMF1->SaveMF1(String(""),String(LPCTSTR(destPath)));
	}
	if(rawMemory!=NULL) delete [] rawMemory;

	return bSuccess;
}

bool MapFileConvert(CString relativeSrcPath,CString srcDir,CString destDir)
{
	CString srcPath=srcDir+"\\"+relativeSrcPath;
	CString destPath=destDir+"\\"+relativeSrcPath;
	if(!PathFileExists(srcPath)) return false;
	int pos=destPath.ReverseFind('\\');
	if(pos==-1) return false;
	CString destFolder=destPath.Left(pos);
	if(!CheckCreateDirectory(destFolder)) return false;
	if(!CopyFile(srcPath,destPath,FALSE)) return false;

	bool bSuccess=true;
	CSGPWorldMap* pWorldMap=NULL;
	uint8* rawMemory=CSGPWorldMap::LoadWorldMap(pWorldMap,String(""),String((LPCTSTR)destPath));
	if(pWorldMap!=NULL)
	{
		// chunk texture file name
		for(uint32 i=0;i<pWorldMap->m_Header.m_iChunkTextureNameNum;++i)
		{
			SGPWorldMapChunkTextureNameTag& texNameTag=pWorldMap->m_pChunkTextureNames[i];
			CString relativeSrcPath1=texNameTag.m_ChunkTextureFileName;
			CString relativeDestPath1;
			if(ImageConvert(relativeSrcPath1,relativeDestPath1,srcDir,destDir))
				strcpy_s(texNameTag.m_ChunkTextureFileName,sizeof(texNameTag.m_ChunkTextureFileName),(LPCTSTR)relativeDestPath1);
		}
		// scene object
		std::set<CString> mf1PathVector;
		for(uint32 i=0;i<pWorldMap->m_Header.m_iSceneObjectNum;++i)
		{
			ISGPObject& obj=pWorldMap->m_pSceneObject[i];
			CString mf1Name=obj.m_MF1FileName;
			mf1PathVector.insert(mf1Name);
		}
		std::set<CString>::iterator iter;
		for(iter=mf1PathVector.begin();iter!=mf1PathVector.end();++iter)
		{
			CString mf1RelativePath=*iter;
			if(!MF1FileConvert(mf1RelativePath,srcDir,destDir))
				AfxGetMainWnd()->MessageBox(CString("Convert ")+mf1RelativePath+CString(" failed!"));
		}
		// water
		SGPWorldMapWaterTag& waterTag=pWorldMap->m_WaterSettingData;
		if(waterTag.m_bHaveWater)
		{
			CString relativeSrcPath1=waterTag.m_WaterWaveTextureName;
			CString relativeDestPath1;
			if(ImageConvert(relativeSrcPath1,relativeDestPath1,srcDir,destDir))
				strcpy_s(waterTag.m_WaterWaveTextureName,sizeof(waterTag.m_WaterWaveTextureName),(LPCTSTR)relativeDestPath1);
		}
		// grass
		SGPWorldMapGrassTag& grassTag=pWorldMap->m_GrassData;
		if(grassTag.m_nChunkGrassClusterNum>0)
		{
			CString relativeSrcPath1=grassTag.m_GrassTextureName;
			CString relativeDestPath1;
			if(ImageConvert(relativeSrcPath1,relativeDestPath1,srcDir,destDir))
				strcpy_s(grassTag.m_GrassTextureName,sizeof(grassTag.m_GrassTextureName),(LPCTSTR)relativeDestPath1);
		}
		// skydome
		SGPWorldMapSunSkyTag& skydomeTag=pWorldMap->m_SkydomeData;
		if(skydomeTag.m_bHaveSkydome)
		{
			CString mf1RelativePath=skydomeTag.m_SkydomeMF1FileName;
			if(!MF1FileConvert(mf1RelativePath,srcDir,destDir))
				AfxGetMainWnd()->MessageBox(CString("Convert ")+mf1RelativePath+CString(" failed!"));
			CString relativeSrcPath1=skydomeTag.m_CloudTextureFileName;
			CString relativeDestPath1;
			if(ImageConvert(relativeSrcPath1,relativeDestPath1,srcDir,destDir))
				strcpy_s(skydomeTag.m_CloudTextureFileName,sizeof(skydomeTag.m_CloudTextureFileName),(LPCTSTR)relativeDestPath1);
			relativeSrcPath1=skydomeTag.m_NoiseTextureFileName;
			if(ImageConvert(relativeSrcPath1,relativeDestPath1,srcDir,destDir))
				strcpy_s(skydomeTag.m_NoiseTextureFileName,sizeof(skydomeTag.m_NoiseTextureFileName),(LPCTSTR)relativeDestPath1);
		}

		bSuccess = pWorldMap->SaveWorldMap(String(""),String(LPCTSTR(destPath)));
	}
	if(rawMemory!=NULL) delete [] rawMemory;
	return bSuccess;
}*/

bool ImageGetInfo(CString relativeSrcPath,CString& relativeDestPath,CString srcDir,CString destDir)
{
	CString srcPath=srcDir+"\\"+relativeSrcPath;
	if(!PathFileExists(srcPath)) return false;
	int pos=relativeSrcPath.ReverseFind('.');
	if(pos==-1) return false;
	relativeDestPath=relativeSrcPath.Left(pos)+".pvr";
	CString destPath=destDir+"\\"+relativeDestPath;
	pos=destPath.ReverseFind('\\');
	if(pos==-1) return false;
	CString destFolder;
	destFolder=destPath.Left(pos);
	if(!CheckCreateDirectory(destFolder)) return false;

	CString fileExt=PathFindExtension(relativeSrcPath);
	fileExt.MakeLower();
	if(fileExt==".dds") return true;
	else if(fileExt==".tga") return true;
	else return false;
}

bool MF1FileGetInfo(CString relativeSrcPath,CString srcDir,CString destDir,std::vector<CString>& srcTexVec)
{
	CString srcPath=srcDir+"\\"+relativeSrcPath;
	CString destPath=destDir+"\\"+relativeSrcPath;
	if(!PathFileExists(srcPath)) return false;
	int pos=destPath.ReverseFind('\\');
	if(pos==-1) return false;
	CString destFolder=destPath.Left(pos);
	if(!CheckCreateDirectory(destFolder)) return false;
	if(!CopyFile(srcPath,destPath,FALSE)) return false;

	bool bSuccess = false;
	CSGPModelMF1* pModelMF1=NULL;
	uint8* rawMemory=CSGPModelMF1::LoadMF1(pModelMF1,String(""),String(LPCTSTR(destPath)));
	if(pModelMF1!=NULL)
	{
		// Header File Name
		CString strFileName=pModelMF1->m_Header.m_cFilename;
		ReversePathSeparator(strFileName);
		strcpy_s(pModelMF1->m_Header.m_cFilename,strFileName);
		// Skin
		for(uint32 i=0;i<pModelMF1->m_Header.m_iNumSkins;++i)
		{
			SGPMF1Skin& skin=pModelMF1->m_pSkins[i];
			CString relativeSrcPath1=skin.m_cName;
			CString relativeDestPath1;
			if(ImageGetInfo(relativeSrcPath1,relativeDestPath1,srcDir,destDir))
			{
				if(!ContainString(srcTexVec,relativeSrcPath1)) srcTexVec.push_back(relativeSrcPath1);
				ReversePathSeparator(relativeDestPath1);
				strcpy_s(skin.m_cName,sizeof(skin.m_cName),(LPCTSTR)relativeDestPath1);
			}
		}
		// Bone & Skeleton Anim filename
		for(uint32 i=0;i<pModelMF1->m_Header.m_iNumBoneAnimFile;++i)
		{
			SGPMF1BoneFileName& boneFileName=pModelMF1->m_pBoneFileNames[i];
			CString strFileName=boneFileName.m_cBoneFileName;
			BF1FileGetInfo(strFileName,srcDir,destDir);
			ReversePathSeparator(strFileName);
			strcpy_s(boneFileName.m_cBoneFileName,strFileName);
		}
		// Config Setting
		for(uint32 i=0;i<pModelMF1->m_Header.m_iNumConfigs;++i)
		{
			SGPMF1ConfigSetting& configSetting=pModelMF1->m_pConfigSetting[i];
			for(uint32 j=0;j<configSetting.ReplaceTextureConfigNum;++j)
			{
				SGPMF1ConfigSetting::ReplaceTextureConfig& texConfig=configSetting.pReplaceTextureConfigList[j];
				CString relativeSrcPath1=texConfig.TexFilename;
				CString relativeDestPath1;
				if(ImageGetInfo(relativeSrcPath1,relativeDestPath1,srcDir,destDir))
				{
					if(!ContainString(srcTexVec,relativeSrcPath1)) srcTexVec.push_back(relativeSrcPath1);
					ReversePathSeparator(relativeDestPath1);
					strcpy_s(texConfig.TexFilename,sizeof(texConfig.TexFilename),(LPCTSTR)relativeDestPath1);
				}
			}
		}

		// Particle Setting
		for(uint32 i=0;i<pModelMF1->m_Header.m_iNumParticles;++i)
		{
			ParticleSystemParam& systemParam=pModelMF1->m_pParticleEmitter[i].m_SystemParam;
			for(uint32 j=0;j<systemParam.m_groupCount;++j)
			{
				ParticleRenderParam& renderParam=systemParam.m_pGroupParam[j].m_RenderParam;
				if(renderParam.m_type==Render_Point)
				{
					ParticlePointRenderData& pointData=renderParam.m_pointData;
					CString relativeSrcPath1=pointData.m_texPath;
					CString relativeDestPath1;
					if(ImageGetInfo(relativeSrcPath1,relativeDestPath1,srcDir,destDir))
					{
						if(!ContainString(srcTexVec,relativeSrcPath1)) srcTexVec.push_back(relativeSrcPath1);
						ReversePathSeparator(relativeDestPath1);
						strcpy_s(pointData.m_texPath,sizeof(pointData.m_texPath),(LPCTSTR)relativeDestPath1);
					}
				}
				else if(renderParam.m_type==Render_Quad)
				{
					ParticleQuadRenderData& quadData=renderParam.m_quadData;
					CString relativeSrcPath1=quadData.m_texPath;
					CString relativeDestPath1;
					if(ImageGetInfo(relativeSrcPath1,relativeDestPath1,srcDir,destDir))
					{
						if(!ContainString(srcTexVec,relativeSrcPath1)) srcTexVec.push_back(relativeSrcPath1);
						ReversePathSeparator(relativeDestPath1);
						strcpy_s(quadData.m_texPath,sizeof(quadData.m_texPath),(LPCTSTR)relativeDestPath1);	
					}
				}
			}
		}
		bSuccess=pModelMF1->SaveMF1(String(""),String(LPCTSTR(destPath)));
	}
	if(rawMemory!=NULL) delete [] rawMemory;

	return bSuccess;
}

bool BF1FileGetInfo(CString relativeSrcPath,CString srcDir,CString destDir)
{
	CString srcPath=srcDir+"\\"+relativeSrcPath;
	CString destPath=destDir+"\\"+relativeSrcPath;
	if(!PathFileExists(srcPath)) return false;

	CString destFolder=FileFolderFromPath(destPath);
	if(!CheckCreateDirectory(destFolder)) return false;
//	if(!CopyFile(srcPath,destPath,FALSE)) return false;

	String AbsolutePath((const char*)(srcPath));
	ScopedPointer<FileInputStream> BF1FileStream( File(AbsolutePath).createInputStream() );
	if( BF1FileStream == nullptr ) return false;
	uint32 iFileSize=(uint32)BF1FileStream->getTotalLength();
	uint8* ucpBuffer=new uint8 [iFileSize];
	BF1FileStream->read(ucpBuffer, iFileSize);
	SGPMF1BoneHeader *pBoneHeader=(SGPMF1BoneHeader*)ucpBuffer;
	CString boneFilePath=pBoneHeader->m_cFilename;
	ReversePathSeparator(boneFilePath);
	strcpy_s(pBoneHeader->m_cFilename,boneFilePath);

	String AbsoluteDestPath((const char*)(destPath));
	ScopedPointer<FileOutputStream> fileStream( File(AbsoluteDestPath).createOutputStream() );
	if (fileStream != nullptr && fileStream->openedOk() )
	{
		fileStream->setPosition(0);
		fileStream->truncate();
		fileStream->write(ucpBuffer,iFileSize);
		fileStream->flush();
	}
	delete [] ucpBuffer;
	return true;
}

bool MapFileGetInfo(CString relativeSrcPath,CString srcDir,CString destDir,std::vector<CString>& srcTexVec)
{
	CString srcPath=srcDir+"\\"+relativeSrcPath;
	CString destPath=destDir+"\\"+relativeSrcPath;
	if(!PathFileExists(srcPath)) return false;
	int pos=destPath.ReverseFind('\\');
	if(pos==-1) return false;
	CString destFolder=destPath.Left(pos);
	if(!CheckCreateDirectory(destFolder)) return false;
	if(!CopyFile(srcPath,destPath,FALSE)) return false;

	bool bSuccess=true;
	CSGPWorldMap* pWorldMap=NULL;
	uint8* rawMemory=CSGPWorldMap::LoadWorldMap(pWorldMap,String(""),String((LPCTSTR)destPath));
	if(pWorldMap!=NULL)
	{
		// Map File Name
		CString mapName=pWorldMap->m_Header.m_cFilename;
		ReversePathSeparator(mapName);
		strcpy_s(pWorldMap->m_Header.m_cFilename,mapName);
		// chunk texture file name
		for(uint32 i=0;i<pWorldMap->m_Header.m_iChunkTextureNameNum;++i)
		{
			SGPWorldMapChunkTextureNameTag& texNameTag=pWorldMap->m_pChunkTextureNames[i];
			CString relativeSrcPath1=texNameTag.m_ChunkTextureFileName;
			CString relativeDestPath1;
			if(ImageGetInfo(relativeSrcPath1,relativeDestPath1,srcDir,destDir))
			{
				if(!ContainString(srcTexVec,relativeSrcPath1)) srcTexVec.push_back(relativeSrcPath1);
				ReversePathSeparator(relativeDestPath1);
				strcpy_s(texNameTag.m_ChunkTextureFileName,sizeof(texNameTag.m_ChunkTextureFileName),relativeDestPath1);
			}
		}
		// alpha blend texture
		uint8* pAlphaBlendData=(uint8*)pWorldMap->m_WorldChunkAlphaTextureData;
		for(uint32 i=0;i<pWorldMap->m_Header.m_iChunkAlphaTextureSize*pWorldMap->m_Header.m_iChunkAlphaTextureSize;++i)
		{
			uint8 tmp=pAlphaBlendData[4*i+0];
			pAlphaBlendData[4*i+0]=pAlphaBlendData[4*i+2];
			pAlphaBlendData[4*i+2]=tmp;
		}
		// color minimap texture
		uint8* pColorMinimapData=(uint8*)pWorldMap->m_WorldChunkColorMiniMapTextureData;
		for(uint32 i=0;i<pWorldMap->m_Header.m_iChunkColorminiMapSize*pWorldMap->m_Header.m_iChunkColorminiMapSize;++i)
		{
			uint8 tmp=pColorMinimapData[4*i+0];
			pColorMinimapData[4*i+0]=pColorMinimapData[4*i+2];
			pColorMinimapData[4*i+2]=tmp;
		}
		// scene object
		std::set<CString> mf1PathVector;
		for(uint32 i=0;i<pWorldMap->m_Header.m_iSceneObjectNum;++i)
		{
			ISGPObject& obj=pWorldMap->m_pSceneObject[i];
			CString mf1Name=obj.m_MF1FileName;
			mf1PathVector.insert(mf1Name);
			ReversePathSeparator(mf1Name);
			strcpy_s(obj.m_MF1FileName,mf1Name);
		}
		std::set<CString>::iterator iter;
		for(iter=mf1PathVector.begin();iter!=mf1PathVector.end();++iter)
		{
			CString mf1RelativePath=*iter;
			if(!MF1FileGetInfo(mf1RelativePath,srcDir,destDir,srcTexVec))
				AfxGetMainWnd()->MessageBox(CString("Convert ")+mf1RelativePath+CString(" failed!"));
		}
		// water
		SGPWorldMapWaterTag& waterTag=pWorldMap->m_WaterSettingData;
		if(waterTag.m_bHaveWater)
		{
			CString relativeSrcPath1=waterTag.m_WaterWaveTextureName;
			CString relativeDestPath1;
			if(ImageGetInfo(relativeSrcPath1,relativeDestPath1,srcDir,destDir))
			{
				ReversePathSeparator(relativeDestPath1);
				strcpy_s(waterTag.m_WaterWaveTextureName,sizeof(waterTag.m_WaterWaveTextureName),relativeDestPath1);
				if(!ContainString(srcTexVec,relativeSrcPath1)) srcTexVec.push_back(relativeSrcPath1);
			}
		}
		// grass
		SGPWorldMapGrassTag& grassTag=pWorldMap->m_GrassData;
		if(grassTag.m_nChunkGrassClusterNum>0)
		{
			CString relativeSrcPath1=grassTag.m_GrassTextureName;
			CString relativeDestPath1;
			if(ImageGetInfo(relativeSrcPath1,relativeDestPath1,srcDir,destDir))
			{
				if(!ContainString(srcTexVec,relativeSrcPath1)) srcTexVec.push_back(relativeSrcPath1);
				ReversePathSeparator(relativeDestPath1);
				strcpy_s(grassTag.m_GrassTextureName,sizeof(grassTag.m_GrassTextureName),(LPCTSTR)relativeDestPath1);
			}
		}
		// skydome
		SGPWorldMapSunSkyTag& skydomeTag=pWorldMap->m_SkydomeData;
		if(skydomeTag.m_bHaveSkydome)
		{
			CString mf1RelativePath=skydomeTag.m_SkydomeMF1FileName;
			if(!MF1FileGetInfo(mf1RelativePath,srcDir,destDir,srcTexVec))
				AfxGetMainWnd()->MessageBox(CString("Convert ")+mf1RelativePath+CString(" failed!"));
			ReversePathSeparator(mf1RelativePath);
			strcpy_s(skydomeTag.m_SkydomeMF1FileName,mf1RelativePath);

			CString relativeSrcPath1=skydomeTag.m_CloudTextureFileName;
			CString relativeDestPath1;
			if(ImageGetInfo(relativeSrcPath1,relativeDestPath1,srcDir,destDir))
			{
				if(!ContainString(srcTexVec,relativeSrcPath1)) srcTexVec.push_back(relativeSrcPath1);
				ReversePathSeparator(relativeDestPath1);
				strcpy_s(skydomeTag.m_CloudTextureFileName,sizeof(skydomeTag.m_CloudTextureFileName),(LPCTSTR)relativeDestPath1);
			}
			relativeSrcPath1=skydomeTag.m_NoiseTextureFileName;
			if(ImageGetInfo(relativeSrcPath1,relativeDestPath1,srcDir,destDir))
			{
				if(!ContainString(srcTexVec,relativeSrcPath1)) srcTexVec.push_back(relativeSrcPath1);
				ReversePathSeparator(relativeDestPath1);
				strcpy_s(skydomeTag.m_NoiseTextureFileName,sizeof(skydomeTag.m_NoiseTextureFileName),(LPCTSTR)relativeDestPath1);
			}
		}

		// lightmap
		CString lightmapFolder=WorldEditorConfig::GetInstance()->GetDataFileDir()+CString("\\Lightmap\\")+FileTitleFromPath(pWorldMap->m_Header.m_cFilename);
		CFileFind finder;
		BOOL bFind=finder.FindFile(lightmapFolder+"\\*.dds");
		while(bFind)
		{
			bFind=finder.FindNextFile();
			CString relativeSrcPath1=GetRelativePath(finder.GetFilePath(),WorldEditorConfig::GetInstance()->GetDataFileDir());
			CString relativeDestPath1;
			if(ImageGetInfo(relativeSrcPath1,relativeDestPath1,srcDir,destDir))
			{
				if(!ContainString(srcTexVec,relativeSrcPath1)) srcTexVec.push_back(relativeSrcPath1);
			}
		}
		bSuccess = pWorldMap->SaveWorldMap(String(""),String(LPCTSTR(destPath)));
	}
	if(rawMemory!=NULL) delete [] rawMemory;
	return bSuccess;
}