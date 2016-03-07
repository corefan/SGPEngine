// NewMapConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "NewMapConfigDlg.h"
#include "afxdialogex.h"

#include "HelpFunction.h"
#include "WorldEditorConfig.h"
#include ".\Render Interface\WorldEditorRenderInterface.h"
#include "..\SGPLibraryCode\SGPHeader.h"
#include "NewMapLoadingDlg.h"
#include "FolderDialog.h"

// CNewMapConfigDlg 对话框

IMPLEMENT_DYNAMIC(CNewMapConfigDlg, CDialogEx)

CNewMapConfigDlg::CNewMapConfigDlg(CNewMapLoadingDlg* loadingDlg,CWnd* pParent /*=NULL*/)
	: CDialogEx(CNewMapConfigDlg::IDD, pParent)
{
	m_loadingDlg = loadingDlg;
}

CNewMapConfigDlg::~CNewMapConfigDlg()
{
}

void CNewMapConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNewMapConfigDlg, CDialogEx)
	ON_BN_CLICKED(IDB_BROWSE_LAYER0_TEXTURE,OnBrowseLayer0Texture)
	ON_BN_CLICKED(IDB_BROWSE_MAP_FOLDER,OnBrowseMapFolder)
END_MESSAGE_MAP()

// CNewMapConfigDlg 消息处理程序

BOOL CNewMapConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CComboBox* pComboBox=(CComboBox*)GetDlgItem(IDC_WORLD_SIZE);
	pComboBox->AddString("Small (16X16)");
	pComboBox->AddString("Middle (32X32)");
	pComboBox->AddString("Large (64X64)");
	pComboBox->SetCurSel(0);

	SetDlgItemText(IDE_MAX_TERRAIN_HEIGHT,"128");
	m_Layer0TexturePath="Texture\\terrain\\slope.dds";
	SetDlgItemText(IDE_LAYER0_TEXTURE_PATH,m_Layer0TexturePath);

	CheckDlgButton(IDC_USE_PERLIN_NOISE,BST_CHECKED);
	GetDlgItem(IDE_MAP_FOLDER)->SetWindowText(WorldEditorConfig::GetInstance()->GetWorldMapDir());

	return TRUE;
}

BOOL CNewMapConfigDlg::PreTranslateMessage(MSG* pMsg)
{
	//avoid ESC/OK to exit dialog
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)
		return TRUE;

	UINT nID = ::GetDlgCtrlID(pMsg->hwnd);
	if(nID==IDE_LAYER0_TEXTURE_PATH||nID==IDE_MAP_FOLDER)
	{
		if(pMsg->message == WM_CHAR) return TRUE;
		if(pMsg->message>=WM_KEYFIRST&&pMsg->message<=WM_KEYLAST) return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CNewMapConfigDlg::OnBrowseLayer0Texture()
{
	TCHAR szFilter[] = _T("DDS Files (*.dds)|*.dds|TGA Files (*.tga)|*.tga||");
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,szFilter);
	dlg.m_ofn.lpstrInitialDir = (char*)(LPCTSTR)WorldEditorConfig::GetInstance()->GetDataFileDir();
	if(dlg.DoModal()==IDOK)
	{
		if(!FileInFolder(dlg.GetPathName(),WorldEditorConfig::GetInstance()->GetDataFileDir()))
		{
			MessageBox("The texture must be in Data directory!","WorldEditor",MB_OK|MB_ICONERROR);
			return ;
		}
		m_Layer0TexturePath = GetRelativePath(dlg.GetPathName(),WorldEditorConfig::GetInstance()->GetDataFileDir());
		SetDlgItemText(IDE_LAYER0_TEXTURE_PATH,m_Layer0TexturePath);
	}
}

void CNewMapConfigDlg::OnBrowseMapFolder()
{
	CFolderDialog dlg(m_hWnd,"Choose folder to save map:",WorldEditorConfig::GetInstance()->GetWorldMapDir());
	if(dlg.DoModal()==IDOK)
	{
		GetDlgItem(IDE_MAP_FOLDER)->SetWindowText(dlg.GetPath());
	}
}

void CNewMapConfigDlg::CreateMap()
{
	GetDlgItemText(IDE_WORLD_NAME,m_loadingDlg->m_MapName);
	GetDlgItemText(IDE_MAP_FOLDER,m_loadingDlg->m_MapFolder);

	CComboBox* pMapSize=(CComboBox*)GetDlgItem(IDC_WORLD_SIZE);
	SGP_TERRAIN_SIZE terrainSize;
	switch(pMapSize->GetCurSel())
	{
	case 0:
		terrainSize = SGPTS_SMALL;
		break;
	case 1:
		terrainSize = SGPTS_MEDIUM;
		break;
	case 2:
		terrainSize = SGPTS_LARGE;
		break;
	}
	m_loadingDlg->m_TerrainSize = terrainSize;
	m_loadingDlg->m_TerrainMaxHeight = GetDlgItemInt(IDE_MAX_TERRAIN_HEIGHT);	
	if(IsDlgButtonChecked(IDC_USE_PERLIN_NOISE)) m_loadingDlg->m_bUsePerlinNoise = true;
	else m_loadingDlg->m_bUsePerlinNoise = false;

	m_loadingDlg->m_Layer0TexPath = m_Layer0TexturePath;
}

void CNewMapConfigDlg::OnOK()
{
	CreateMap();
	CDialogEx::OnOK();
}
