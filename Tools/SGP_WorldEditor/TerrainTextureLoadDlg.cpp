// TerrainTextureLoadDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "TerrainTextureLoadDlg.h"
#include "afxdialogex.h"

#include "WorldEditorConfig.h"
#include "TerrainTextureDlg.h"
#include <io.h>
// CTerrainTextureLoadDlg 对话框

IMPLEMENT_DYNAMIC(CTerrainTextureLoadDlg, CDialogEx)

CTerrainTextureLoadDlg::CTerrainTextureLoadDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTerrainTextureLoadDlg::IDD, pParent),\
	m_ThumbnailGenerator(this)
{
	m_pTextureDlg=(CTerrainTextureDlg*)pParent;
}

CTerrainTextureLoadDlg::~CTerrainTextureLoadDlg()
{
}

void CTerrainTextureLoadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
}

BEGIN_MESSAGE_MAP(CTerrainTextureLoadDlg, CDialogEx)
	ON_MESSAGE(UM_UPDATE_TEXTURE_LIST,OnUpdateTextureList)
	ON_MESSAGE(WM_KICKIDLE,OnKickIdle)
END_MESSAGE_MAP()

// CTerrainTextureLoadDlg 消息处理程序

LRESULT CTerrainTextureLoadDlg::OnUpdateTextureList(WPARAM wParam,LPARAM lParam)
{
	// Update Control
	CString strText;
	strText.Format("%d/%d",m_index,m_FileCount);
	SetDlgItemText(IDC_FILENUM_INDICATOR,strText);

	m_Progress.SetPos(m_index);

	return 0;
}

/*DWORD WINAPI ThreadProc(__in  LPVOID lpParameter)
{
	CTerrainTextureLoadDlg* pLoadDlg=(CTerrainTextureLoadDlg*)lpParameter;
	pLoadDlg->ClearNoUseThumbnail();
	pLoadDlg->CreateThumbnails();
	pLoadDlg->PostMessage(WM_CLOSE);
	return 0;
}*/

BOOL CTerrainTextureLoadDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CFileFind finder;
	CString terrainTexturePath=WorldEditorConfig::GetInstance()->GetTerrainTextureDir();
	BOOL bFinder=finder.FindFile(terrainTexturePath+"\\*.*");
	m_FileCount=0;

	while(bFinder)
	{
		bFinder=finder.FindNextFile();
		if(finder.IsDots()) continue;
		m_FileCount++;
	}

	m_bExist=m_Finder.FindFile(terrainTexturePath+"\\*.*");
	m_index=0;
	m_Progress.SetRange(0,m_FileCount);

	m_ThumbnailGenerator.BuildMemDC(1024,1024);
	UpdateDlgCtrls();

//	CreateThread(NULL,0,ThreadProc,this,0,NULL);
	ClearNoUseThumbnail();

	return TRUE;
}

LRESULT CTerrainTextureLoadDlg::OnKickIdle(WPARAM wParam,LPARAM lParam)
{
/*	while(m_bExist)
	{
		m_bExist=m_Finder.FindNextFile();
		if(m_Finder.IsDots()) continue;

		CString fileName=m_Finder.GetFileName();
		CString thumbnailPath=fileName.Left(fileName.ReverseFind('.'))+".bmp";
		thumbnailPath=WorldEditorConfig::GetInstance()->GetTerrainTextureThumbnailDir()+"\\"+thumbnailPath;
		BOOL bExist=TRUE;
		if(_access((LPCTSTR)thumbnailPath,0)==-1)// don't exist
			bExist=m_ThumbnailGenerator.GenerateThumbnail(m_Finder.GetFilePath(),m_Finder.GetFileName());
		if(bExist)
		{
			CImage img;
			img.Load(thumbnailPath);
			if(!img.IsNull())
			{
				HBITMAP hBitmap=img.Detach();
				CBitmap* pBitmap=CBitmap::FromHandle(hBitmap);
				m_pTextureDlg->m_ImageList.Add(pBitmap,RGB(0,0,0));
				::DeleteObject(hBitmap);
				int currIndex=m_pTextureDlg->m_TextureList.GetItemCount();
			//	m_pTextureDlg->m_TextureList.InsertItem(currIndex,fileName,currIndex);
				m_pTextureDlg->m_TextureList.InsertItem(m_index,fileName,m_index);
			}
		}
		m_index++;
		SendMessage(UM_UPDATE_TEXTURE_LIST);
	}*/

//	return FALSE;
	if(m_bExist)
	{
	//	ClearNoUseThumbnail();
	//	for(int i=0;i<10;++i) CreateThumbnails();
		for(int i=0;i<10&&m_bExist;)
		{
			if(CreateThumbnails()) ++i;
		}
	}
	else
		PostMessage(WM_CLOSE);

	return TRUE;
}

BOOL CTerrainTextureLoadDlg::PreTranslateMessage(MSG* pMsg)
{
	//avoid ESC/OK to exit dialog
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN) return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CTerrainTextureLoadDlg::ClearNoUseThumbnail()
{
	CFileStatus status;
	CString textureFolder=WorldEditorConfig::GetInstance()->GetTerrainTextureDir();
	CFileFind finder;
	BOOL bFind=finder.FindFile(WorldEditorConfig::GetInstance()->GetTerrainTextureThumbnailDir()+"\\*.bmp");
	while(bFind)
	{
		bFind=finder.FindNextFile();
		if(finder.IsDots()) continue;

		CString thumbnailName=finder.GetFileTitle();
		int pos=thumbnailName.ReverseFind('-');
		if(pos==-1)
		{
			DeleteFile(finder.GetFilePath());
			continue;
		}
		CString textureName=thumbnailName.Left(pos);
		CString sTextureSize=thumbnailName.Right(thumbnailName.GetLength()-pos-1);
		int iTexSize=atoi(sTextureSize);
		CString texturePath=textureFolder+"\\"+textureName;	
		if(_access(texturePath,0)!=-1)
		{
			CFile::GetStatus(texturePath,status);
			if((int)status.m_size!=iTexSize)
				DeleteFile(finder.GetFilePath());
			continue;
		}
		else
		{
			DeleteFile(finder.GetFilePath());
		}
	}
}

BOOL CTerrainTextureLoadDlg::CreateThumbnails()
{
	CString thumbnailDir=WorldEditorConfig::GetInstance()->GetTerrainTextureThumbnailDir();
	CFileStatus status;
	BOOL bNewCreated=FALSE;
	if(m_bExist)
	{
		m_bExist=m_Finder.FindNextFile();
		if(m_Finder.IsDots()) /*continue*/return FALSE;

		CFile::GetStatus(m_Finder.GetFilePath(),status);
		CString thumbnailName;
		thumbnailName.Format("%s-%I64ubmp",m_Finder.GetFileName(),status.m_size);
		CString thumbnailPath=thumbnailDir+"\\"+thumbnailName;
		BOOL bExist=TRUE;
		if(_access(thumbnailPath,0)==-1)
		{
			bExist=m_ThumbnailGenerator.GenerateThumbnail(m_Finder.GetFilePath(),thumbnailPath);
			bNewCreated=TRUE;
		}
		if(bExist)
		{
			CImage img;
			img.Load(thumbnailPath);
			if(!img.IsNull())
			{
				HBITMAP hBitmap=img.Detach();
				CBitmap *pBitmap=CBitmap::FromHandle(hBitmap);
				int currTexIndex=m_pTextureDlg->m_TextureList.GetItemCount();
				int currImgIndex=m_pTextureDlg->m_ImageList.GetImageCount();
				m_pTextureDlg->m_ImageList.Add(pBitmap,RGB(0,0,0));
				m_pTextureDlg->m_TextureList.InsertItem(currTexIndex,m_Finder.GetFileName(),currImgIndex);
				m_pTextureDlg->m_TextureList.SetItemData(currTexIndex,(int)status.m_size);
				::DeleteObject(hBitmap);
			}
		}
		m_index++;
		UpdateDlgCtrls();
	}
	return bNewCreated;
}

void CTerrainTextureLoadDlg::UpdateDlgCtrls()
{
	// Update Control
	CString strText;
	strText.Format("%d/%d",m_index,m_FileCount);
	SetDlgItemText(IDC_FILENUM_INDICATOR,strText);

	m_Progress.SetPos(m_index);
}