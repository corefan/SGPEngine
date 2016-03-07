// .\Setting Pane\SettingDlg.cpp : 实现文件
//

#include "..\stdafx.h"
#include "..\WorldEditor.h"
#include "SettingDlg.h"
#include "afxdialogex.h"

#include "..\WorldEditorConfig.h"
#include "..\SGPLibraryCode\SGPHeader.h"

// CSettingDlg 对话框

IMPLEMENT_DYNAMIC(CSettingDlg, CDialogEx)

CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettingDlg::IDD, pParent)
{

}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSettingDlg, CDialogEx)
END_MESSAGE_MAP()

// CSettingDlg 消息处理程序

BOOL CSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	WorldEditorConfig* pConfig=WorldEditorConfig::GetInstance();

	CheckDlgButton(IDC_SHOW_CHUNKGRID,pConfig->m_bShowChunkGrid);
	CheckDlgButton(IDC_SHOW_CHUNK,pConfig->m_bShowChunk);
	CheckDlgButton(IDC_SHOW_WIREFRAME,pConfig->m_bShowWireframe);
	CheckDlgButton(IDC_SHOW_WATER,pConfig->m_bShowWater);
	CheckDlgButton(IDC_SHOW_OBJECTS,pConfig->m_bShowObjects);
	CheckDlgButton(IDC_SHOW_LIGHTS,pConfig->m_bShowLights);

	return TRUE;
}

BOOL CSettingDlg::PreTranslateMessage(MSG* pMsg)
{
	//avoid ESC/OK to exit dialog
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN) return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CSettingDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WorldEditorConfig* pConfig=WorldEditorConfig::GetInstance();
	switch(LOWORD(wParam))
	{
	case IDC_SHOW_CHUNKGRID:
		pConfig->m_bShowChunkGrid=IsDlgButtonChecked(IDC_SHOW_CHUNKGRID);
		break;
	case IDC_SHOW_CHUNK:
		pConfig->m_bShowChunk=IsDlgButtonChecked(IDC_SHOW_CHUNK);
		break;
	case IDC_SHOW_WIREFRAME:
		pConfig->m_bShowWireframe=IsDlgButtonChecked(IDC_SHOW_WIREFRAME);
		break;
	case IDC_SHOW_WATER:
		pConfig->m_bShowWater=IsDlgButtonChecked(IDC_SHOW_WATER);
		break;
	case IDC_SHOW_LIGHTS:
		pConfig->m_bShowLights=IsDlgButtonChecked(IDC_SHOW_LIGHTS);
		break;
	case IDC_SHOW_OBJECTS:
		pConfig->m_bShowObjects=IsDlgButtonChecked(IDC_SHOW_OBJECTS);
		break;
	}

	return CDialogEx::OnCommand(wParam, lParam);
}
