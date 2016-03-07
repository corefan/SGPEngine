// PackupResProgressDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "PackupResProgressDlg.h"
#include "afxdialogex.h"

#include "FileConvert.h"

// CPackupResProgressDlg 对话框

IMPLEMENT_DYNAMIC(CPackupResProgressDlg, CDialogEx)

CPackupResProgressDlg::CPackupResProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPackupResProgressDlg::IDD, pParent)
{
	m_bReady=FALSE;
}

CPackupResProgressDlg::~CPackupResProgressDlg()
{
}

void CPackupResProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
}

BEGIN_MESSAGE_MAP(CPackupResProgressDlg, CDialogEx)
	ON_MESSAGE(WM_KICKIDLE,OnKickIdle)
END_MESSAGE_MAP()

// CPackupResProgressDlg 消息处理程序

BOOL CPackupResProgressDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

//	GetDlgItem(IDC_INFO)->SetWindowText("Process "+m_MapPath);
	GetDlgItem(IDC_INFO)->SetWindowText("");
	m_srcPathVec.clear();

	return TRUE;
}

BOOL CPackupResProgressDlg::PreTranslateMessage(MSG* pMsg)
{
	//avoid ESC/OK to exit dialog
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_RETURN||pMsg->wParam==VK_ESCAPE) return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

LRESULT CPackupResProgressDlg::OnKickIdle(WPARAM wParam,LPARAM lParam)
{
	UpdateCtrls();
	if(m_bReady)
	{
		GetDlgItem(IDC_INFO)->SetWindowText(CString("Process ")+m_srcPathVec[m_nIndex]);
		if(m_nIndex<m_nCount)
			ImageConvert(m_srcPathVec[m_nIndex],m_srcDir,m_destDir,m_PixelFormat,m_CompressQuality);
		m_nIndex++;
		if(m_nIndex>=m_nCount) SendMessage(WM_CLOSE);
	}
	else
	{	
		GatherInformation();
		m_nCount=m_srcPathVec.size();
		m_nIndex=0;
		m_bReady=TRUE;
		m_Progress.SetRange(0,m_nCount);
		m_Progress.SetPos(0);
	}

	return TRUE;
}

void CPackupResProgressDlg::GatherInformation()
{
	m_srcPathVec.clear();
	if(m_nMode==0)
		MapFileGetInfo(m_MapPath,m_srcDir,m_destDir,m_srcPathVec);
	else if(m_nMode==1)
		ProcessFolder(m_FolderPath);
}

void CPackupResProgressDlg::UpdateCtrls()
{
	if(m_bReady)
	{
		m_Progress.SetPos(m_nIndex);
		CString strCurrRatio;
		strCurrRatio.Format("%d/%d",m_nIndex,m_nCount);
		GetDlgItem(IDC_CURR_RATIO)->SetWindowText(strCurrRatio);
	}
	else
	{
		GetDlgItem(IDC_CURR_RATIO)->SetWindowText("Gathering Information...");
	}
}

void CPackupResProgressDlg::ProcessFolder(CString strPath)
{
	CFileFind finder;
	BOOL bFind=finder.FindFile(strPath+"\\*.*");
	while(bFind)
	{
		bFind=finder.FindNextFile();
		if(finder.IsDots()) continue;
		if(finder.IsDirectory()) ProcessFolder(finder.GetFilePath());
		else
		{
			CString filePath=finder.GetFilePath();
			CString fileExt=PathFindExtension(filePath);
			fileExt.MakeLower();
			if(fileExt==".mf1")
			{
				CString relativeSrcPath=GetRelativePath(filePath,WorldEditorConfig::GetInstance()->GetDataFileDir());
				MF1FileGetInfo(relativeSrcPath,m_srcDir,m_destDir,m_srcPathVec);
			}
			else if(fileExt==".map")
			{
				CString relativeSrcPath=GetRelativePath(filePath,WorldEditorConfig::GetInstance()->GetDataFileDir());
				MapFileGetInfo(relativeSrcPath,m_srcDir,m_destDir,m_srcPathVec);
			}
		}
	}
}