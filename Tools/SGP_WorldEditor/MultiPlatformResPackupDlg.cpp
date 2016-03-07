// MultiPlatformResPackupDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "MultiPlatformResPackupDlg.h"
#include "afxdialogex.h"

#include "WorldEditorConfig.h"
#include "FolderDialog.h"
#include "FileConvert.h"

CString PixelFmtArray[]=
{
	"PVRTC 2bpp",
	"PVRTC 4bpp",
/*	"PVRTCII 2bpp",
	"PVRTCII 4bpp"*/
};

EPVRTPixelFormat pixelFmtArray[]=
{
	ePVRTPF_PVRTCI_2bpp_RGB,
	ePVRTPF_PVRTCI_4bpp_RGB,
/*	ePVRTPF_PVRTCII_2bpp,
	ePVRTPF_PVRTCII_4bpp*/
};

CString CompressQualityArray[]=
{
	"Fastest",
	"Fast",
	"Normal",
	"High",
	"Best"
};

pvrtexture::ECompressorQuality qualityArray[]=
{
	pvrtexture::ePVRTCFastest,
	pvrtexture::ePVRTCFast,
	pvrtexture::ePVRTCNormal,
	pvrtexture::ePVRTCHigh,
	pvrtexture::ePVRTCBest
};

// CMultiPlatformResPackupDlg 对话框

IMPLEMENT_DYNAMIC(CMultiPlatformResPackupDlg, CDialogEx)

CMultiPlatformResPackupDlg::CMultiPlatformResPackupDlg(CPackupResProgressDlg* pPackupResDlg,CWnd* pParent /*=NULL*/)
	: CDialogEx(CMultiPlatformResPackupDlg::IDD, pParent)
{
	m_pPackupResDlg=pPackupResDlg;
}

CMultiPlatformResPackupDlg::~CMultiPlatformResPackupDlg()
{
}

void CMultiPlatformResPackupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLATFORM_COMBO, m_PlatformCombo);
	DDX_Control(pDX, IDC_PIXEL_FORMAT_TYPE, m_PixelFormatCombo);
	DDX_Control(pDX, IDC_COMPRESSION_QUALITY, m_CompressQualityCombo);
	DDX_Control(pDX, IDC_PACKUP_TARGET_TAB, m_TargetTab);
}

BEGIN_MESSAGE_MAP(CMultiPlatformResPackupDlg, CDialogEx)
	ON_BN_CLICKED(IDB_BROWSE,OnBrowseFolder)
	ON_BN_CLICKED(IDB_PACKUP_RESOURCE,OnPackupResource)
	ON_BN_CLICKED(IDB_BROWSE_MAP,OnBrowseMap)
	ON_BN_CLICKED(IDB_BROWSE_FOLDER,OnBrowseResFolder)
	ON_BN_CLICKED(IDB_RUN_EMULATION,OnRunEmulation)
	ON_NOTIFY(TCN_SELCHANGE, IDC_PACKUP_TARGET_TAB,OnTcnSelchangePackupTargetTab)
END_MESSAGE_MAP()

// CMultiPlatformResPackupDlg 消息处理程序

BOOL CMultiPlatformResPackupDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set Initial Value
	m_PlatformCombo.AddString("iPhone");
	m_PlatformCombo.AddString("Android");
	m_PlatformCombo.SetCurSel(0);

	for(uint32 i=0;i<sizeof(PixelFmtArray)/sizeof(CString);++i)
		m_PixelFormatCombo.AddString(PixelFmtArray[i]);
	m_PixelFormatCombo.SetCurSel(0);

	for(uint32 i=0;i<sizeof(CompressQualityArray)/sizeof(CString);++i)
		m_CompressQualityCombo.AddString(CompressQualityArray[i]);
	m_CompressQualityCombo.SetCurSel(2);

	CString path=WorldEditorConfig::GetInstance()->GetDataFileDir();
	path+="2";
	CheckCreateDirectory(path);
	SetDlgItemText(IDE_DEST_PATH,path);

	SetDlgItemText(IDE_FOLDER_PATH,WorldEditorConfig::GetInstance()->GetDataFileDir()+"\\SkeletonMesh");

	m_TargetTab.InsertItem(0,"Packup Map");
	m_TargetTab.InsertItem(1,"Packup Folder");

	ChangeTab();
	GetDlgItem(IDB_RUN_EMULATION)->EnableWindow(FALSE);

	return TRUE;
}

BOOL CMultiPlatformResPackupDlg::PreTranslateMessage(MSG* pMsg)
{
	//avoid ESC/OK to exit dialog
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN) return TRUE;

	UINT nID = ::GetDlgCtrlID(pMsg->hwnd);
	if(nID==IDE_DEST_PATH||nID==IDE_MAP_PATH||nID==IDE_FOLDER_PATH)
	{
		if(pMsg->message == WM_CHAR) return TRUE;
		if(pMsg->message>=WM_KEYFIRST&&pMsg->message<=WM_KEYLAST) return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CMultiPlatformResPackupDlg::OnBrowseFolder()
{
	CString path;
	GetDlgItemText(IDE_DEST_PATH,path);
	CFolderDialog dlg(m_hWnd,"Choose destination folder:",path);
	if(dlg.DoModal()==IDOK)
		SetDlgItemText(IDE_DEST_PATH,dlg.GetPath());
}

void CMultiPlatformResPackupDlg::OnBrowseMap()
{
	TCHAR strFilter[] = _T("MAP Files (*.map)|*.map||");
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,strFilter,this);
	dlg.m_ofn.lpstrInitialDir = (char*)(LPCTSTR)WorldEditorConfig::GetInstance()->GetWorldMapDir();
	if(dlg.DoModal()==IDOK)
	{
		CString mapPath=dlg.GetPathName();
		if(FileInFolder(mapPath,WorldEditorConfig::GetInstance()->GetDataFileDir()))
			SetDlgItemText(IDE_MAP_PATH,mapPath);
		else
			MessageBox("Please select a map file in work directory!","WorldEditor",MB_OK|MB_ICONERROR);
	}
}

void CMultiPlatformResPackupDlg::OnBrowseResFolder()
{
	CString path;
	GetDlgItemText(IDE_FOLDER_PATH,path);
	CFolderDialog dlg(m_hWnd,"Choose resource folder:",path);
	if(dlg.DoModal()==IDOK)
		SetDlgItemText(IDE_FOLDER_PATH,dlg.GetPath());
}

void CMultiPlatformResPackupDlg::OnPackupResource()
{
	CString srcDir=WorldEditorConfig::GetInstance()->GetDataFileDir();

	CPackupResProgressDlg processDlg;

	int nTab=m_TargetTab.GetCurSel();
	if(nTab==0)
	{
		CString mapPath;
		GetDlgItemText(IDE_MAP_PATH,mapPath);
		if(!PathFileExists(mapPath))
		{
			MessageBox("Please select a map file first!","WorldEditor",MB_OK|MB_ICONSTOP);
			return ;
		}
		CString relativeMapPath=GetRelativePath(mapPath,WorldEditorConfig::GetInstance()->GetDataFileDir());
	//	m_pPackupResDlg->m_MapPath=relativeMapPath;
	//	m_pPackupResDlg->m_nMode=0;
		processDlg.m_MapPath=relativeMapPath;
		processDlg.m_nMode=0;
	}
	else if(nTab==1)
	{
		CString folderPath;
		GetDlgItemText(IDE_FOLDER_PATH,folderPath);
		if(!PathFileExists(folderPath))
		{
			MessageBox("The folder path don't exist!","WorldEditor",MB_OK|MB_ICONSTOP);
			return ;
		}
		if(!FileInFolder(folderPath,srcDir))
		{
			MessageBox("The folder path must in Data folder!","WorldEditor",MB_OK|MB_ICONSTOP);
			return ;
		}
	//	m_pPackupResDlg->m_FolderPath = folderPath;
	//	m_pPackupResDlg->m_nMode = 1;
		processDlg.m_FolderPath = folderPath;
		processDlg.m_nMode = 1;
	}

	CString destDir;
	GetDlgItemText(IDE_DEST_PATH,destDir);

//	m_pPackupResDlg->m_srcDir=srcDir;
//	m_pPackupResDlg->m_destDir=destDir;
	processDlg.m_srcDir=srcDir;
	processDlg.m_destDir=destDir;
	// Pixel Format
	CString pixelFormat;
	m_PixelFormatCombo.GetLBText(m_PixelFormatCombo.GetCurSel(),pixelFormat);
	uint32 index;
	for(index=0;index<sizeof(PixelFmtArray)/sizeof(CString);++index)
		if(PixelFmtArray[index]==pixelFormat) break;
//	m_pPackupResDlg->m_PixelFormat=pixelFmtArray[index];
	processDlg.m_PixelFormat=pixelFmtArray[index];

	// Compress Quality
	CString compressQuality;
	m_CompressQualityCombo.GetLBText(m_CompressQualityCombo.GetCurSel(),compressQuality);
	for(index=0;index<sizeof(CompressQualityArray)/sizeof(CString);++index)
		if(CompressQualityArray[index]==compressQuality) break;
//	m_pPackupResDlg->m_CompressQuality=qualityArray[index];
	processDlg.m_CompressQuality=qualityArray[index];
	
	processDlg.DoModal();

	if(processDlg.m_nMode==0)
	{
		m_RelativeMapPath=processDlg.m_MapPath;
		GetDlgItem(IDB_RUN_EMULATION)->EnableWindow(TRUE);
	}

//	EndDialog(IDOK);
}

#define SHOWMODE(n) (n)?SW_SHOW:SW_HIDE

void CMultiPlatformResPackupDlg::OnTcnSelchangePackupTargetTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	ChangeTab();

	*pResult = 0;
}

void CMultiPlatformResPackupDlg::ChangeTab()
{
	int nTab=m_TargetTab.GetCurSel();
	GetDlgItem(IDC_MAP_FILE_STATIC)->ShowWindow(SHOWMODE(nTab==0));
	GetDlgItem(IDE_MAP_PATH)->ShowWindow(SHOWMODE(nTab==0));
	GetDlgItem(IDB_BROWSE_MAP)->ShowWindow(SHOWMODE(nTab==0));
	GetDlgItem(IDB_RUN_EMULATION)->ShowWindow(SHOWMODE(nTab==0));
	GetDlgItem(IDC_FOLDER_STATIC)->ShowWindow(SHOWMODE(nTab==1));
	GetDlgItem(IDE_FOLDER_PATH)->ShowWindow(SHOWMODE(nTab==1));
	GetDlgItem(IDB_BROWSE_FOLDER)->ShowWindow(SHOWMODE(nTab==1));
}

void CMultiPlatformResPackupDlg::OnRunEmulation()
{
	CString strPath=WorldEditorConfig::GetInstance()->GetExeDir()+"\\OpenGLES2\\OGLES2_Win32PCEmulation.exe";
	if(!PathFileExists(strPath))
	{
		MessageBox(strPath+" don't exist!","WorldEditor",MB_OK|MB_ICONSTOP);
		return ;
	}
	CString cmdLine="\""+strPath+"\" "+m_RelativeMapPath;
	WinExec(cmdLine,SW_SHOW);
}