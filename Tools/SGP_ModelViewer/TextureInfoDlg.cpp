// DDSInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "TextureInfoDlg.h"
#include "afxdialogex.h"

#include "ModelViewerRenderInterface.h"
#include "../SGPLibraryCode/SGPHeader.h"//engine header
#include "CommonMacroDef.h"
using namespace sgp;

#define BORDER_COLOR_R 212//77//28
#define BORDER_COLOR_G 214//141//81
#define BORDER_COLOR_B 216//171//128

IMPLEMENT_DYNAMIC(CTextureInfoDlg, CDialogEx)

CTextureInfoDlg::CTextureInfoDlg(CString strPath,CString strFile,CWnd* pParent /*=NULL*/)
	: CDialogEx(CTextureInfoDlg::IDD, pParent)
{
	m_TexturePath=strPath;
	m_FileName=strFile;
	m_bShowLog=TRUE;
	m_TextureID=0;
	m_pData=NULL;
}

CTextureInfoDlg::~CTextureInfoDlg()
{
}

void CTextureInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTextureInfoDlg, CDialogEx)
	ON_MESSAGE(WM_KICKIDLE,OnKickIdle)
	ON_WM_NCHITTEST()
	ON_BN_CLICKED(IDB_IMAGEINFO_CLOSE,OnOK)
	ON_BN_CLICKED(IDB_IMAGEINFO_CLOSE2,OnOK)
	ON_WM_ERASEBKGND()
	ON_CBN_SELCHANGE(IDC_MIPMAP_LEVEL,OnMipmapLevelChanged)
	ON_CBN_SELCHANGE(IDC_COLOR_CHANNEL,OnChannelChanged)
	ON_CBN_SELCHANGE(IDC_CUBEMAP_FACE,OnCubemapFaceChanged)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CTextureInfoDlg 消息处理程序

LRESULT CTextureInfoDlg::OnKickIdle(WPARAM wParam,LPARAM lParam)
{
	ModelViewerRenderInterface::GetInstance()->Render();
	return TRUE;
	return FALSE;
}

BOOL CTextureInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CenterWindow();

	SetDlgItemText(IDC_FILE_PATH,m_FileName);

	m_ImageDlg.Create(IDD_SHOW_IMAGE_DLG,this);
	CRect rect,rc;
	GetClientRect(rect);
	rc=rect;
	rc.DeflateRect(10,120,10,130);
	m_ImageDlg.MoveWindow(rc);
	m_ImageDlg.ShowWindow(SW_SHOW);

	m_CubemapComboBox.SubclassDlgItem(IDC_CUBEMAP_FACE,this);
	m_MipmapComboBox.SubclassDlgItem(IDC_MIPMAP_LEVEL,this);
	m_ChannelComboBox.SubclassDlgItem(IDC_COLOR_CHANNEL,this);
	m_ButtonClose1.SubclassDlgItem(IDB_IMAGEINFO_CLOSE,this);
	m_ButtonClose1.SetHaveText(TRUE);
	m_ButtonClose2.SubclassDlgItem(IDB_IMAGEINFO_CLOSE2,this);
	m_ButtonClose2.m_nImageType=CloseDlgButton;
	m_ButtonClose2.m_bRound=TRUE;
	m_ButtonClose2.m_BkColor=RGB(BORDER_COLOR_R,BORDER_COLOR_G,BORDER_COLOR_B);
	m_ButtonClose2.m_bCustomBkColor=TRUE;
	m_ButtonClose2.MoveWindow(rect.right-28,rect.top+1,26,26);

	m_TitleFont.CreatePointFont(120,"微软雅黑");
	GetDlgItem(IDC_FILE_PATH)->SetFont(&m_TitleFont);

	LoadTexture();

	return TRUE;  // return TRUE unless you set the focus to a control
}

LRESULT CTextureInfoDlg::OnNcHitTest(CPoint point)
{
//	return HTCAPTION;

	return CDialogEx::OnNcHitTest(point);
}

void CTextureInfoDlg::LoadTexture()
{
	// get path in unicode
	WCHAR wPath[MAX_PATH];
	wmemset(wPath,0,MAX_PATH);
	MultiByteToWideChar(CP_ACP,0,(LPCTSTR)m_TexturePath,m_TexturePath.GetLength(),wPath,MAX_PATH);

	// register texture
	CSGPTextureManager* pTextureManager=ModelViewerRenderInterface::GetInstance()->GetRenderDevice()->GetTextureManager();
	m_TextureID=pTextureManager->registerTexture(String(wPath));

	if(m_TextureID==0)
	{
		MessageBox("The texture format isn't supported!","Model Viewer",MB_OK|MB_ICONERROR);
		OnCancel();
	}

	CTextureResource* pTextureResource=pTextureManager->getTextureByID(m_TextureID);
	m_pTexture=pTextureResource->pSGPTexture;

	// get size info
	SDimension2D textureSize=m_pTexture->getSize();

	//set type info
	CString sTextureType;
	if(m_pTexture->isTexture2D()) sTextureType=_T("Texture 2D");
	else if(m_pTexture->isTexture3D()) sTextureType=_T("Texture 3D");
	else if(m_pTexture->isCubeMap()) sTextureType=_T("Cubemap");
	SetDlgItemText(IDC_IMAGE_TYPE,sTextureType);

	if(sTextureType!=_T("Cubemap"))
	{
		m_CubemapComboBox.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CUBEMAP_FACE_STATIC)->ShowWindow(SW_HIDE);
	}
	else
	{
		m_CubemapComboBox.AddString(_T("Positive X"));
		m_CubemapComboBox.AddString(_T("Negative X"));
		m_CubemapComboBox.AddString(_T("Positive Y"));
		m_CubemapComboBox.AddString(_T("Negative Y"));
		m_CubemapComboBox.AddString(_T("Positive Z"));
		m_CubemapComboBox.AddString(_T("Negative Z"));
		m_CubemapComboBox.SetCurSel(0);
		m_CubemapComboBox.GetLBText(m_CubemapComboBox.GetCurSel(),m_sCubemapFace);
	}

	// get mipmap level
	uint32 mipLevel=m_pTexture->getMipMapLevels();
	m_MipmapComboBox.AddString(_T("Level 0"));
	for(uint32 level=1;level<mipLevel;++level)
	{
		CString str;
		str.Format(_T("Level %d"),level);
		m_MipmapComboBox.AddString(str);
	}
	m_MipmapComboBox.SetCurSel(0);
	m_MipmapComboBox.GetLBText(m_MipmapComboBox.GetCurSel(),m_sMipmapLevel);

	// set channel info
	if(m_pTexture->hasAlpha())
	{
		m_ChannelComboBox.AddString(_T("Alpha Channel Only"));
		m_ChannelComboBox.AddString(_T("RGB Channel Only"));
		m_ChannelComboBox.AddString(_T("RGBA Channel"));
		m_ChannelComboBox.SetCurSel(1);
	}
	else
	{
		m_ChannelComboBox.AddString(_T("RGB Channel"));
		m_ChannelComboBox.SetCurSel(0);
	}
	m_ChannelComboBox.GetLBText(m_ChannelComboBox.GetCurSel(),m_sChannel);

	// set pixel format info
	SetPixelFormatInfo(wPath);

	// create memory bitmap
	CDC* pDC=m_ImageDlg.GetDC();
	m_MemDC.CreateCompatibleDC(pDC);
	m_MemBmp.CreateCompatibleBitmap(pDC,textureSize.Width,textureSize.Height);
	m_MemDC.SelectObject(&m_MemBmp);
	m_ImageDlg.ReleaseDC(pDC);

	UpdateImage();

}

void CTextureInfoDlg::UnLoadTexture()
{
	CSGPTextureManager* pTextureManager=ModelViewerRenderInterface::GetInstance()->GetRenderDevice()->GetTextureManager();
	if(m_TextureID!=0) pTextureManager->unRegisterTextureByID(m_TextureID);

/*	if(m_bShowLog)
		ModelViewerRenderInterface::GetInstance()->ShowInfoLog(_T("unRegister Texture: ")+m_FileName);*/
}

void CTextureInfoDlg::OnOK()
{
	UnLoadTexture();
	m_MemDC.DeleteDC();
	m_MemBmp.DeleteObject();
	if(m_pData != NULL)
	{
		free( m_pData );
		m_pData = NULL;
	}

	CDialogEx::OnOK();
}

BOOL CTextureInfoDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)
		return TRUE;

	if(pMsg->message==WM_LBUTTONDOWN)
	{
		int ctrlID=::GetDlgCtrlID(pMsg->hwnd);
		static int idGroup[]={IDC_FILE_PATH,IDC_TEXTURE_SIZE_STATIC,IDC_IMAGE_SIZE,IDC_TEXTURE_TYPE_STATIC,\
			IDC_IMAGE_TYPE,IDC_PIXEL_FORMAT,IDC_PIXELS_FORMAT,IDC_MIPMAP_LEVEL_STATIC,IDC_COLOR_CHANNEL_STATIC,\
			IDC_CUBEMAP_FACE_STATIC};

		for(int i=0;i<sizeof(idGroup)/sizeof(int);++i)
		{
			if(idGroup[i]==ctrlID)
			{
				PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(pMsg->pt.x,pMsg->pt.y));
				break;
			}
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CTextureInfoDlg::DrawTexture(CDC* pDC)
{
	pDC->BitBlt(0,0,m_Width,m_Height,&m_MemDC,0,0,SRCCOPY);
}

void CTextureInfoDlg::UpdateImage()
{
	// display waiting cursor
	CWaitCursor cursor;

	if(m_pData!=NULL)
	{
		free( m_pData );
		m_pData = NULL;
	}

	m_Width=m_pTexture->getSize().Width;
	m_Height=m_pTexture->getSize().Height;

	int i,j,startindex;
	int nMipmap=m_MipmapComboBox.GetCurSel();
	for(i=0;i<nMipmap;++i)
	{
		m_Width>>=1;
		m_Height>>=1;
	}

	// set size info
	CString str;
	str.Format(_T("%d * %d"),m_Width,m_Height);
	SetDlgItemText(IDC_IMAGE_SIZE,str);

	// locate memory
	if(m_pTexture->isTexture3D())// temporary use
		m_pData = (unsigned char*)malloc(sizeof(unsigned char) * (m_Width*m_Height*4*m_MipmapDepthVector[nMipmap]));
	else
		m_pData = (unsigned char*)malloc(sizeof(unsigned char) * (m_Width*m_Height*4));

	if(m_pTexture->isTexture2D())//texture 2d
		m_pTexture->getMipmapData(m_pData,nMipmap,SGPTT_TEXTURE_2D);
	else if(m_pTexture->isTexture3D())
		m_pTexture->getMipmapData(m_pData,nMipmap,SGPTT_TEXTURE_3D);
	else if(m_pTexture->isCubeMap())
	{
		switch(m_CubemapComboBox.GetCurSel())
		{
		case 0:
			m_pTexture->getMipmapData(m_pData,nMipmap,SGPTT_TEXTURE_CUBE_MAP_POSITIVE_X);
			break;
		case 1:
			m_pTexture->getMipmapData(m_pData,nMipmap,SGPTT_TEXTURE_CUBE_MAP_NEGATIVE_X);
			break;
		case 2:
			m_pTexture->getMipmapData(m_pData,nMipmap,SGPTT_TEXTURE_CUBE_MAP_POSITIVE_Y);
			break;
		case 3:
			m_pTexture->getMipmapData(m_pData,nMipmap,SGPTT_TEXTURE_CUBE_MAP_NEGATIVE_Y);
			break;
		case 4:
			m_pTexture->getMipmapData(m_pData,nMipmap,SGPTT_TEXTURE_CUBE_MAP_POSITIVE_Z);
			break;
		case 5:
			m_pTexture->getMipmapData(m_pData,nMipmap,SGPTT_TEXTURE_CUBE_MAP_NEGATIVE_Z);
			break;
		}
	}

	if(m_sChannel==_T("RGBA Channel"))
	{
		for(i=0;i<m_Width;++i)
		{
			for(j=0;j<m_Height;++j)
			{
				startindex=(j*m_Width+i)*4;
				float ratio=(float)m_pData[startindex+3]/255;
				int r=(int)(ratio*m_pData[startindex+2]/*+(1-ratio)*0*/);
				int g=(int)(ratio*m_pData[startindex+1]/*+(1-ratio)*0*/);
				int b=(int)(ratio*m_pData[startindex]/*+(1-ratio)*0*/);
				m_MemDC.SetPixel(i,j,RGB(r,g,b));
			}
		}
	}
	else if(m_sChannel==_T("RGB Channel Only")||m_sChannel==_T("RGB Channel"))
	{
		for(i=0;i<m_Width;++i)
		{
			for(j=0;j<m_Height;++j)
			{
				startindex=(j*m_Width+i)*4;
				m_MemDC.SetPixel(i,j,RGB(m_pData[startindex+2],m_pData[startindex+1],m_pData[startindex]));
			}
		}
	}
	else if(m_sChannel==_T("Alpha Channel Only"))
	{
		for(i=0;i<m_Width;++i)
		{
			for(j=0;j<m_Height;++j)
			{
				startindex=(j*m_Width+i)*4+3;
				m_MemDC.SetPixel(i,j,RGB(m_pData[startindex],m_pData[startindex],m_pData[startindex]));
			}
		}
	}

	if(m_pData!=NULL)
	{
		free( m_pData );
		m_pData=NULL;
	}

	m_ImageDlg.SetScrollSize(m_Width,m_Height);
}

void CTextureInfoDlg::OnMipmapLevelChanged()
{
	CString sMipmapLevel;
	m_MipmapComboBox.GetLBText(m_MipmapComboBox.GetCurSel(),sMipmapLevel);
	if(m_sMipmapLevel!=sMipmapLevel)
	{
		m_sMipmapLevel=sMipmapLevel;
		UpdateImage();
	}	
}

void CTextureInfoDlg::OnChannelChanged()
{
	CString sChannel;
	m_ChannelComboBox.GetLBText(m_ChannelComboBox.GetCurSel(),sChannel);
	if(m_sChannel!=sChannel)
	{
		m_sChannel=sChannel;
		UpdateImage();
	}
}

void CTextureInfoDlg::OnCubemapFaceChanged()
{
	CString sCubemapFace;
	m_CubemapComboBox.GetLBText(m_CubemapComboBox.GetCurSel(),sCubemapFace);
	if(m_sCubemapFace!=sCubemapFace)
	{
		m_sCubemapFace=sCubemapFace;
		UpdateImage();
	}
}

HBRUSH CTextureInfoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	int nID=pWnd->GetDlgCtrlID();

	if(nID==IDC_FILE_PATH)
	{
		pDC->SetTextColor(RGB(0,0,0));
		pDC->SetBkMode(TRANSPARENT);
		if(m_BorderBrush.m_hObject==NULL)
			m_BorderBrush.CreateSolidBrush(RGB(BORDER_COLOR_R,BORDER_COLOR_G,BORDER_COLOR_B));
		return m_BorderBrush;
	}

	if(nID==IDC_IMAGE_SIZE||nID==IDC_IMAGE_TYPE||nID==IDC_PIXELS_FORMAT||\
		nID==IDC_MIPMAP_LEVEL||nID==IDC_COLOR_CHANNEL||nID==IDC_CUBEMAP_FACE)
	{
		pDC->SetTextColor(RGB(0,0,255));
	}

	return hbr;
}


void CTextureInfoDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	CDialogEx::OnLButtonDown(nFlags, point);
}

void CTextureInfoDlg::SetPixelFormatInfo(WCHAR* wPath)
{
	CSGPTextureManager* pTextureManager=ModelViewerRenderInterface::GetInstance()->GetRenderDevice()->GetTextureManager();
	ISGPImage* pImage=pTextureManager->createImageFromFile(String(wPath));

	// can't create image
	if(pImage==NULL) return ;

	CString sPixelFormat=_T("UNKNOWN");

	if(pImage->IsDDSImage())// dds image
	{
		SGPImageDDS* pImageDDS=(SGPImageDDS*)pImage;
		int32 internalFormat=pImageDDS->getInternalFormat();
		switch(internalFormat)
		{
		case GL_RGBA8:
			sPixelFormat=_T("A8R8G8B8");
			break;
		case GL_RGB8:
			sPixelFormat=_T("R8G8B8");
			break;
		case GL_RGB5_A1:
			sPixelFormat=_T("A1R5G5B5");
			break;
		case GL_RGB5:
			sPixelFormat=_T("R5G6B5");
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			sPixelFormat=_T("DXT1");
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
			sPixelFormat=_T("DXT3");
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			sPixelFormat=_T("DXT5");
			break;
		default:
			break;
		}

		// store texture3d depth info
		uint32 mipmapCount=pImageDDS->getNumberOfMipmaps();
		for(uint32 i=0;i<mipmapCount;++i)
			m_MipmapDepthVector.push_back((int)pImageDDS->getMipmapDepth(i));
	}
	else// tga image
	{
		SGP_PIXEL_FORMAT pixelFormat=pImage->getColorFormat();
		switch(pixelFormat)
		{
		case SGPPF_A1R5G5B5:
			sPixelFormat=_T("A1R5G5B5");
			break;
		case SGPPF_R5G6B5:
			sPixelFormat=_T("R5G6B5");
			break;
		case SGPPF_R8G8B8:
			sPixelFormat=_T("R8G8B8");
			break;
		case SGPPF_A8R8G8B8:
			sPixelFormat=_T("A8R8G8B8");
			break;
		case SGPPF_A4R4G4B4:
			sPixelFormat=_T("A4R4G4B4");
			break;
		case SGPPF_R16F:
			sPixelFormat=_T("R16F");
			break;
		case SGPPF_G16R16F:
			sPixelFormat=_T("G16R16F");
			break;
		case SGPPF_A16B16G16R16F:
			sPixelFormat=_T("A16B16G16R16F");
			break;
		case SGPPF_R32F:
			sPixelFormat=_T("R32F");
			break;
		case SGPPF_G32R32F:
			sPixelFormat=_T("G32R32F");
			break;
		case SGPPF_A32B32G32R32F:
			sPixelFormat=_T("A32B32G32R32F");
			break;
		default:
			break;
		}
	}

	SetDlgItemText(IDC_PIXELS_FORMAT,sPixelFormat);
	delete [] pImage;
}


void CTextureInfoDlg::OnPaint()
{
	CDialogEx::OnPaint();

	COLORREF borderColor=RGB(BORDER_COLOR_R,BORDER_COLOR_G,BORDER_COLOR_B);

	CDC* pDC=GetDC();
	CRect rc;
	GetClientRect(rc);
	CBrush brush;
	brush.CreateSolidBrush(borderColor);
	pDC->FrameRect(rc,&brush);
	rc.DeflateRect(1,1);
	pDC->FrameRect(rc,&brush);

	pDC->FillSolidRect(rc.left,rc.top,rc.Width(),28,borderColor);

	ReleaseDC(pDC);
}
