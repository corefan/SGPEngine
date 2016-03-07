#include "stdafx.h"

#include "ThumbnailGenerator.h"
#include ".\Render Interface\WorldEditorRenderInterface.h"
#include "..\SGPLibraryCode\SGPHeader.h"//engine header
#include "WorldEditorConfig.h"

ThumbnailGenerator::ThumbnailGenerator(CWnd* pWnd):m_Width(0),m_Height(0)
{
	m_pWnd=pWnd;
//	m_Image.Create(64,64,24);
}

BOOL ThumbnailGenerator::GenerateThumbnail(CString filePath,CString destPath,unsigned int imgWidth,unsigned int imgHeight)
{
	// get path in unicode
	BSTR wPath=filePath.AllocSysString();

	// register texture
	CSGPTextureManager* pTextureManager=WorldEditorRenderInterface::GetInstance()->GetRenderDevice()->GetTextureManager();
	uint32 TextureID=pTextureManager->registerTexture(String(wPath));
	if(TextureID==0)// the texture isn't supported by editor
	{
		SysFreeString(wPath);
		return FALSE;
	}

	CTextureResource* pTetxtureResource=pTextureManager->getTextureByID(TextureID);
	ISGPTexture* pTexture=pTetxtureResource->pSGPTexture;
	if(!pTexture->isTexture2D())
	{
		pTextureManager->unRegisterTextureByID(TextureID);
		SysFreeString(wPath);
		return FALSE;
	}
	
	SDimension2D textureSize=pTexture->getSize();
	uint32 mipLevel=pTexture->getMipMapLevels();
	uint32 width=textureSize.Width,height=textureSize.Height;
	uint32 currMip=0;
	uint32 i,j;
	for(i=1;i<mipLevel;++i)
	{
	/*	if(width/2>=64&&height/2>=64)
		{
			width/=2;
			height/=2;
			++currMip;
		}
		else break;*/
		if(width/2>=imgWidth&&height/2>=imgHeight)
		{
			width/=2;
			height/=2;
			++currMip;
		}
		else break;
	}

	if(width>m_Width||height>m_Height) BuildMemDC(width,height);
	unsigned char* pData=new unsigned char[width*height*4];
	pTexture->getMipmapData(pData,currMip,SGPTT_TEXTURE_2D);
	for(i=0;i<width;++i)
	{
		for(j=0;j<height;++j)
		{
			uint32 index=(j*width+i)*4;
			m_MemDC.SetPixel(i,j,RGB(pData[index+2],pData[index+1],pData[index]));
		}
	}
	delete pData;
	
	CImage image;
	image.Create(imgWidth,imgHeight,24);
//	HDC hDC=m_Image.GetDC();
	HDC hDC = image.GetDC();
	::SetStretchBltMode(hDC,HALFTONE);
	::StretchBlt(hDC,0,0,imgWidth,imgHeight,m_MemDC.m_hDC,0,0,width,height,SRCCOPY);
	image.ReleaseDC();
	image.Save(destPath,ImageFormatBMP);
//	m_Image.ReleaseDC();
//	m_Image.Save(destPath,ImageFormatBMP);

	pTextureManager->unRegisterTextureByID(TextureID);
	SysFreeString(wPath);
	return TRUE;
}

void ThumbnailGenerator::BuildMemDC(unsigned int width,unsigned int height)
{
	m_MemDC.DeleteDC();
	m_MemBmp.DeleteObject();

	CDC* pDC=m_pWnd->GetDC();
	m_MemDC.CreateCompatibleDC(pDC);
	m_MemBmp.CreateCompatibleBitmap(pDC,width,height);
	m_MemDC.SelectObject(&m_MemBmp);
	m_pWnd->ReleaseDC(pDC);
	m_Width=width;
	m_Height=height;
}

BOOL ThumbnailGenerator::GenerateGrassThumbnail(CString filePath,int xTile,int yTile)
{
	ASSERT(xTile>0&&yTile>0);
	// get path in unicode
	BSTR wPath=filePath.AllocSysString();

	// register texture
	CSGPTextureManager* pTextureManager=WorldEditorRenderInterface::GetInstance()->GetRenderDevice()->GetTextureManager();
	uint32 TextureID=pTextureManager->registerTexture(String(wPath));

	CTextureResource* pTetxtureResource=pTextureManager->getTextureByID(TextureID);
	ISGPTexture* pTexture=pTetxtureResource->pSGPTexture;
	if(!pTexture->isTexture2D())
	{
		pTextureManager->unRegisterTextureByID(TextureID);
		SysFreeString(wPath);
		m_pWnd->MessageBox("The grass texture isn't a 2D texture!","Terrain Grass",MB_OK|MB_ICONERROR);
		return FALSE;
	}
	
	SDimension2D textureSize=pTexture->getSize();
	uint32 mipLevel=pTexture->getMipMapLevels();
	uint32 width=textureSize.Width,height=textureSize.Height;
	uint32 i,j;

	if(width>m_Width||height>m_Height) BuildMemDC(width,height);
	unsigned char* pData=new unsigned char[width*height*4];
	pTexture->getMipmapData(pData,0,SGPTT_TEXTURE_2D);
	if(pTexture->hasAlpha())// whether have alpha channel
	{
		for(i=0;i<width;++i)
		{
			for(j=0;j<height;++j)
			{
				int startindex=(j*width+i)*4;
				float ratio=(float)pData[startindex+3]/255;
				int r=(int)(ratio*pData[startindex+2]/*+(1-ratio)*0*/);
				int g=(int)(ratio*pData[startindex+1]/*+(1-ratio)*0*/);
				int b=(int)(ratio*pData[startindex]/*+(1-ratio)*0*/);
				m_MemDC.SetPixel(i,j,RGB(r,g,b));
			}
		}
	}
	else
	{
		for(i=0;i<width;++i)
		{
			for(j=0;j<height;++j)
			{
				uint32 index=(j*width+i)*4;
				m_MemDC.SetPixel(i,j,RGB(pData[index+2],pData[index+1],pData[index]));
			}
		}
	}
	delete pData;

	CImage image;
	image.Create(64,64,24);
	int index = 0;
	for(j=0; j<(unsigned int)yTile; ++j)
	{
		for(i=0; i<(unsigned int)xTile; ++i)
		{
			CString sFilePath;
			sFilePath.Format("%s\\%d.bmp",WorldEditorConfig::GetInstance()->GetGrassTextureThumbnailDir(),++index);
			HDC hDC = image.GetDC();
			::SetStretchBltMode(hDC,HALFTONE);
			::StretchBlt(hDC,0,0,64,64,m_MemDC.m_hDC,i*width/xTile,j*height/yTile,width/xTile,height/yTile,SRCCOPY);
			image.ReleaseDC();
			image.Save(sFilePath,ImageFormatBMP);
		}
	}

	pTextureManager->unRegisterTextureByID(TextureID);
	SysFreeString(wPath);
	return TRUE;
}