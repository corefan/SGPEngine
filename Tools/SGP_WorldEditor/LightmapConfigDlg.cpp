// LightmapConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "LightmapConfigDlg.h"
#include "afxdialogex.h"

#include "WorldEditorConfig.h"
#include "Render Interface\WorldEditorRenderInterface.h"
#include "..\SGPLibraryCode\SGPHeader.h"
#include "FolderDialog.h"
#include "LightmapBuildDlg.h"
// CLightmapConfigDlg 对话框

CLightmapConfigDlg* CLightmapConfigDlg::s_pLightmapConfigDlg=NULL;

IMPLEMENT_DYNAMIC(CLightmapConfigDlg, CDialogEx)

CLightmapConfigDlg::CLightmapConfigDlg(CLightMapBuildDlg* pBuildDlg,CWnd* pParent /*=NULL*/)
	: CDialogEx(CLightmapConfigDlg::IDD, pParent),m_pBuildDlg(pBuildDlg)
{

}

CLightmapConfigDlg::~CLightmapConfigDlg()
{
}

void CLightmapConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM_LIGHTMAP_SIZE, m_CustomTexSizeCombo);
}

BEGIN_MESSAGE_MAP(CLightmapConfigDlg, CDialogEx)
	ON_BN_CLICKED(IDB_BROWSE_FOLDER,OnBrowseTexFolder)
	ON_BN_CLICKED(IDOK, &CLightmapConfigDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_DEFAULT_TEXSIZE,OnDefaultTexSize)
	ON_BN_CLICKED(IDC_CUSTOM_TEXSIZE,OnCustomTexSize)
END_MESSAGE_MAP()

// CLightmapConfigDlg 消息处理程序

BOOL CLightmapConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	s_pLightmapConfigDlg=this;

	// Set Default Value
	SetDlgItemInt(IDE_SAMPLE_COUNT,CSGPLightMapGenConfig::getInstance()->m_iLightMap_Sample_Count);
	CString strText;
	strText.Format("%.3f",CSGPLightMapGenConfig::getInstance()->m_fLightMap_Collision_Offset);
	SetDlgItemText(IDE_COLLISION_OFFSET,strText);
	strText.Format("%.3f",CSGPLightMapGenConfig::getInstance()->m_fLightMap_AO_Distance);
	SetDlgItemText(IDE_AO_DISTANCE,strText);
	CString strWorldName = WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getWorldName().toUTF8().getAddress();
	strText.Format("%s\\Lightmap\\%s",WorldEditorConfig::GetInstance()->GetDataFileDir(),strWorldName);
	if(!PathIsDirectory(strText)) ::CreateDirectory(strText,NULL);
	SetDlgItemText(IDE_TEXTURE_FOLDER,strText);
	
	CheckDlgButton(IDC_TARGET_TERRAIN,BST_CHECKED);
	CheckDlgButton(IDC_TARGET_SELECTED_OBJ,BST_CHECKED);
	CheckDlgButton(IDC_DEFAULT_TEXSIZE,BST_CHECKED);
	GetDlgItem(IDC_CUSTOM_LIGHTMAP_SIZE)->EnableWindow(FALSE);

	uint32 texSize=DEFAULT_LIGHTMAP_MINSIZE;
	while(texSize<=DEFAULT_LIGHTMAP_MAXSIZE)
	{
		CString strItem;
		strItem.Format("%u X %u",texSize,texSize);
		m_CustomTexSizeCombo.AddString(strItem);
		texSize*=2;
	}
	m_CustomTexSizeCombo.SetCurSel(3);

	return TRUE;
}

BOOL CLightmapConfigDlg::PreTranslateMessage(MSG* pMsg)
{
	//avoid ESC/OK to exit dialog
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN) return TRUE;
	}

	UINT nID = ::GetDlgCtrlID(pMsg->hwnd);
	if(nID==IDE_TEXTURE_FOLDER)
	{
		if(pMsg->message == WM_CHAR) return TRUE;
		if(pMsg->message>=WM_KEYFIRST&&pMsg->message<=WM_KEYLAST) return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CLightmapConfigDlg::OnBrowseTexFolder()
{
	CString sDefaultPath;
	GetDlgItemText(IDE_TEXTURE_FOLDER,sDefaultPath);
	if(!PathIsDirectory(sDefaultPath)) sDefaultPath.Empty();
	CFolderDialog dlg(m_hWnd,"Browse Texture Folder",sDefaultPath);
	if(dlg.DoModal()==IDOK) SetDlgItemText(IDE_TEXTURE_FOLDER,dlg.GetPath());
}

void CLightmapConfigDlg::OnBnClickedOk()
{
	if(IsDlgButtonChecked(IDC_TARGET_TERRAIN)) m_pBuildDlg->m_bIncludeTerrain=true;
	else m_pBuildDlg->m_bIncludeTerrain=false;
	if(IsDlgButtonChecked(IDC_TARGET_SELECTED_OBJ)) m_pBuildDlg->m_bIncludeSeledObjects=true;
	else m_pBuildDlg->m_bIncludeSeledObjects=false;
	
	m_pBuildDlg->m_SampleCount=GetDlgItemInt(IDE_SAMPLE_COUNT);
	CString strText;
	GetDlgItemText(IDE_COLLISION_OFFSET,strText);
	m_pBuildDlg->m_fCollisionOffset=(float)atof(strText);
	GetDlgItemText(IDE_AO_DISTANCE,strText);
	m_pBuildDlg->m_fAODistance=(float)atof(strText);
	GetDlgItemText(IDE_TEXTURE_FOLDER,m_pBuildDlg->m_TextureFolder);
	if(IsDlgButtonChecked(IDC_DEFAULT_TEXSIZE)) m_pBuildDlg->m_bDefaultLightmapSize=true;
	else
	{
		m_pBuildDlg->m_bDefaultLightmapSize=false;
		int nCurSel = m_CustomTexSizeCombo.GetCurSel();
		m_pBuildDlg->m_TextureWidth=DEFAULT_LIGHTMAP_MINSIZE;
		for(int i=0;i<nCurSel;++i) m_pBuildDlg->m_TextureWidth*=2;
		m_pBuildDlg->m_TextureHeight=m_pBuildDlg->m_TextureWidth;
	}

	CSGPLightMapGenConfig::getInstance()->m_iLightMap_Sample_Count=m_pBuildDlg->m_SampleCount;
	CSGPLightMapGenConfig::getInstance()->m_fLightMap_Collision_Offset=m_pBuildDlg->m_fCollisionOffset;
	CSGPLightMapGenConfig::getInstance()->m_fLightMap_AO_Distance=m_pBuildDlg->m_fAODistance;

	CDialogEx::OnOK();
}

void CLightmapConfigDlg::OnDefaultTexSize()
{
	GetDlgItem(IDC_CUSTOM_LIGHTMAP_SIZE)->EnableWindow(IsDlgButtonChecked(IDC_CUSTOM_TEXSIZE));
}

void CLightmapConfigDlg::OnCustomTexSize()
{
	GetDlgItem(IDC_CUSTOM_LIGHTMAP_SIZE)->EnableWindow(IsDlgButtonChecked(IDC_CUSTOM_TEXSIZE));
}