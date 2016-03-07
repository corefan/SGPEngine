// SkydomeSettingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "SkydomeSettingDlg.h"
#include "afxdialogex.h"

#include "SceneEditorDlg.h"
#include "WorldEditorConfig.h"
#include "HelpFunction.h"
#include ".\Render Interface\WorldEditorRenderInterface.h"
#include "..\SGPLibraryCode\SGPHeader.h"
#include "WorldMapManager.h"

// CSkydomeSettingDlg 对话框

CSkydomeSettingDlg* CSkydomeSettingDlg::s_pSkydomeDlg=NULL;

IMPLEMENT_DYNAMIC(CSkydomeSettingDlg, CDialogEx)

CSkydomeSettingDlg::CSkydomeSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSkydomeSettingDlg::IDD, pParent)
{

}

CSkydomeSettingDlg::~CSkydomeSettingDlg()
{
}

void CSkydomeSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDB_EXPAND,m_ExpandButton);
}

BEGIN_MESSAGE_MAP(CSkydomeSettingDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDB_EXPAND,OnExpand)
	ON_BN_CLICKED(IDB_CHOOSE_MF1_PATH,OnChooseMF1)
	ON_BN_CLICKED(IDB_CHOOSE_CLOUD_TEXTURE,OnChooseCloudTexture)
	ON_BN_CLICKED(IDB_CHOOSE_NOISE_TEXTURE,OnChooseNoiseTexture)
	ON_BN_CLICKED(IDB_APPLY_SETTING,OnApplySetting)
END_MESSAGE_MAP()

// CSkydomeSettingDlg 消息处理程序

BOOL CSkydomeSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetClientRect(m_rc);
	SetExpand(FALSE);
	InitDefaultValue();

	s_pSkydomeDlg = this;

	return TRUE;
}

void CSkydomeSettingDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if(m_ExpandButton.m_hWnd != NULL)
		m_ExpandButton.SetWindowPos(NULL,0,0,cx,20,SWP_SHOWWINDOW);
}

void CSkydomeSettingDlg::OnExpand()
{
	SetExpand(!m_ExpandButton.m_bExpanded);
	((CSceneEditorDlg*)GetParent())->SetCtrlPos();
}

void CSkydomeSettingDlg::SetExpand(BOOL bExpand)
{
	m_ExpandButton.SetExpand(bExpand);
	if(bExpand) SetWindowPos(NULL,0,0,m_rc.Width(),m_rc.Height(),SWP_NOMOVE|SWP_SHOWWINDOW);
	else SetWindowPos(NULL,0,0,m_rc.Width(),20+1,SWP_NOMOVE);
}

void CSkydomeSettingDlg::OnChooseMF1()
{
	TCHAR szFilter[] = _T("Model Files (*.mf1)|*.mf1||");
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,szFilter);
	dlg.m_ofn.lpstrInitialDir = (char*)(LPCTSTR)WorldEditorConfig::GetInstance()->GetDataFileDir();
	if(dlg.DoModal() == IDOK)
	{
		if(!FileInFolder(dlg.GetPathName(),WorldEditorConfig::GetInstance()->GetDataFileDir()))
		{
			MessageBox("The model resource must be in Data directory!","WorldEditor",MB_OK|MB_ICONERROR);
			return ;
		}
		CString mf1Path = GetRelativePath(dlg.GetPathName(),WorldEditorConfig::GetInstance()->GetDataFileDir());
		SetDlgItemText(IDE_MF1_PATH,mf1Path);
	}
}

void CSkydomeSettingDlg::OnChooseCloudTexture()
{
	TCHAR szFilter[] = _T("DDS Files (*.dds)|*.dds|TGA Files (*.tga)|*.tga||");
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,szFilter);
	dlg.m_ofn.lpstrInitialDir = (char*)(LPCTSTR)WorldEditorConfig::GetInstance()->GetDataFileDir();
	if(dlg.DoModal() == IDOK)
	{
		if(!FileInFolder(dlg.GetPathName(),WorldEditorConfig::GetInstance()->GetDataFileDir()))
		{
			MessageBox("The texture must be in Data directory!","WorldEditor",MB_OK|MB_ICONERROR);
			return ;
		}
		CString texPath = GetRelativePath(dlg.GetPathName(),WorldEditorConfig::GetInstance()->GetDataFileDir());
		SetDlgItemText(IDE_CLOUD_TEXTURE_PATH,texPath);
	}
}

void CSkydomeSettingDlg::OnChooseNoiseTexture()
{
	TCHAR szFilter[] = _T("DDS Files (*.dds)|*.dds|TGA Files (*.tga)|*.tga||");
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,szFilter);
	dlg.m_ofn.lpstrInitialDir = (char*)(LPCTSTR)WorldEditorConfig::GetInstance()->GetDataFileDir();
	if(dlg.DoModal() == IDOK)
	{
		if(!FileInFolder(dlg.GetPathName(),WorldEditorConfig::GetInstance()->GetDataFileDir()))
		{
			MessageBox("The texture must be in Data directory!","WorldEditor",MB_OK|MB_ICONERROR);
			return ;
		}
		CString texPath = GetRelativePath(dlg.GetPathName(),WorldEditorConfig::GetInstance()->GetDataFileDir());
		SetDlgItemText(IDE_NOISE_TEXTURE_PATH,texPath);
	}
}

void CSkydomeSettingDlg::OnApplySetting()
{
	// Check validate
	CString str;
	GetDlgItemText(IDE_MF1_PATH,str);
	if(str.IsEmpty())
	{
		MessageBox("Please set model resource firstly!","Skydome Setting",MB_OK|MB_ICONERROR);
		return ;
	}
	GetDlgItemText(IDE_CLOUD_TEXTURE_PATH,str);
	if(str.IsEmpty())
	{
		MessageBox("Please set cloud texture firstly!","Skydome Setting",MB_OK|MB_ICONERROR);
		return ;
	}
	GetDlgItemText(IDE_NOISE_TEXTURE_PATH,str);
	if(str.IsEmpty())
	{
		MessageBox("Please set noise texture firstly!","Skydome Setting",MB_OK|MB_ICONERROR);
		return ;
	}

	WorldEditorRenderInterface* pRenderInterface = WorldEditorRenderInterface::GetInstance();

	// delete old Skydome
	CSGPSkyDome* pSkydome = NULL;
	if((pSkydome=(CSGPSkyDome*)(pRenderInterface->GetWorldSystemManager()->getSkydome()))!=NULL)
	{
		pRenderInterface->GetWorldSystemManager()->releaseSkydome();
		delete pSkydome;
	}

	// create skydome
	CString sMF1Path;
	GetDlgItemText(IDE_MF1_PATH,sMF1Path);
	BSTR mf1Path = sMF1Path.AllocSysString();
	pRenderInterface->GetWorldSystemManager()->createSkydome(String(mf1Path));
	SysFreeString(mf1Path);

	// set skydome texture
	CString cloudTexPath,noiseTexPath;
	GetDlgItemText(IDE_CLOUD_TEXTURE_PATH,cloudTexPath);
	GetDlgItemText(IDE_NOISE_TEXTURE_PATH,noiseTexPath);
	BSTR cloudPath = cloudTexPath.AllocSysString();
	BSTR noisePath = noiseTexPath.AllocSysString();
	pRenderInterface->GetWorldSystemManager()->setSkydomeTexture(String(cloudPath),String(noisePath));
	SysFreeString(cloudPath);
	SysFreeString(noisePath);

	// set skydome attributes
//	pSkydome = pRenderInterface->GetSkydomeObject();
	pSkydome = (CSGPSkyDome*)(pRenderInterface->GetWorldSystemManager()->getSkydome());
	ASSERT(pSkydome!=NULL);
	GetDlgItemText(IDE_MODEL_SCALE,str);
	pSkydome->SetScale((float)atof(str));//scale
	GetDlgItemText(IDE_CLOUD_MOVE_SPEED_X,str);
	pSkydome->m_coludMoveSpeed_x = (float)atof(str);
	GetDlgItemText(IDE_CLOUD_MOVE_SPEED_Z,str);
	pSkydome->m_coludMoveSpeed_z = (float)atof(str);
	GetDlgItemText(IDE_NOISE_TRXTURE_SCALE,str);
	pSkydome->m_coludNoiseScale = (float)atof(str);
	GetDlgItemText(IDE_CLOUD_BRIGHTNESS,str);
	pSkydome->m_coludBrightness = (float)atof(str);

	CSGPHoffmanPreethemScatter& scatter = pSkydome->GetScatter();
	GetDlgItemText(IDE_HGG_FUNCTION,str);
	scatter.m_fHGgFunction = (float)atof(str);
	GetDlgItemText(IDE_INSCATTER_MULTIPLIER,str);
	scatter.m_fInscatteringMultiplier = (float)atof(str);
	GetDlgItemText(IDE_BETARAY_MULTIPLIER,str);
	scatter.m_fBetaRayMultiplier = (float)atof(str);
	GetDlgItemText(IDE_BETAMIE_MULTIPLIER,str);
	scatter.m_fBetaMieMultiplier = (float)atof(str);
	GetDlgItemText(IDE_SUN_INTENSITY,str);
	scatter.m_fSunIntensity = (float)atof(str);
	GetDlgItemText(IDE_TURBITITY,str);
	scatter.m_fTurbitity = (float)atof(str);
	scatter.calculateScatteringConstants();

	GetDlgItemText(IDE_SUN_POSITION,str);
	float fSunPosition=(float)atof(str);
	WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->setWorldSunPosition(fSunPosition);
	if(fSunPosition!=pRenderInterface->GetWorldSystemManager()->getWorldSun()->m_fSunPosition)
		WorldMapManager::GetInstance()->NotifyRefreshCollSet();
}

void CSkydomeSettingDlg::InitDefaultValue()
{
	CSGPSkyDome tmpSkydome;
	CString strText;
	strText.Format("%.6f",tmpSkydome.GetScale());
	SetDlgItemText(IDE_MODEL_SCALE,strText);
	strText.Format("%.6f",tmpSkydome.m_coludMoveSpeed_x);
	SetDlgItemText(IDE_CLOUD_MOVE_SPEED_X,strText);
	strText.Format("%.6f",tmpSkydome.m_coludMoveSpeed_z);
	SetDlgItemText(IDE_CLOUD_MOVE_SPEED_Z,strText);
	strText.Format("%.6f",tmpSkydome.m_coludNoiseScale);
	SetDlgItemText(IDE_NOISE_TRXTURE_SCALE,strText);
	strText.Format("%.6f",tmpSkydome.m_coludBrightness);
	SetDlgItemText(IDE_CLOUD_BRIGHTNESS,strText);
	// Atmosphere
	strText.Format("%.6f",tmpSkydome.GetScatter().m_fHGgFunction);
	SetDlgItemText(IDE_HGG_FUNCTION,strText);
	strText.Format("%.6f",tmpSkydome.GetScatter().m_fInscatteringMultiplier);
	SetDlgItemText(IDE_INSCATTER_MULTIPLIER,strText);
	strText.Format("%.6f",tmpSkydome.GetScatter().m_fBetaRayMultiplier);
	SetDlgItemText(IDE_BETARAY_MULTIPLIER,strText);
	strText.Format("%.6f",tmpSkydome.GetScatter().m_fBetaMieMultiplier);
	SetDlgItemText(IDE_BETAMIE_MULTIPLIER,strText);
	strText.Format("%.6f",tmpSkydome.GetScatter().m_fSunIntensity);
	SetDlgItemText(IDE_SUN_INTENSITY,strText);
	strText.Format("%.6f",tmpSkydome.GetScatter().m_fTurbitity);
	SetDlgItemText(IDE_TURBITITY,strText);
	
	CSGPWorldSun tmpWorldSun;
	strText.Format("%.2f",tmpWorldSun.m_fSunPosition);
	SetDlgItemText(IDE_SUN_POSITION,strText);

/*	// set test value
	SetDlgItemText(IDE_MF1_PATH,"skydome\\skydome.mf1");
	SetDlgItemText(IDE_CLOUD_TEXTURE_PATH,"skydome\\cloud001.dds");
	SetDlgItemText(IDE_NOISE_TEXTURE_PATH,"skydome\\perturb001.dds");*/
}

BOOL CSkydomeSettingDlg::PreTranslateMessage(MSG* pMsg)
{
	UINT nID = ::GetDlgCtrlID(pMsg->hwnd);
	if(nID==IDE_MF1_PATH||nID==IDE_CLOUD_TEXTURE_PATH||nID==IDE_NOISE_TEXTURE_PATH)
	{
		if(pMsg->message == WM_CHAR) return TRUE;
		if(pMsg->message>=WM_KEYFIRST&&pMsg->message<=WM_KEYLAST) return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CSkydomeSettingDlg::EnableAllCtrls(BOOL bEnable)
{
	CWnd* pWnd = GetWindow(GW_CHILD);
	while(pWnd != NULL)
	{
		if(pWnd->GetDlgCtrlID()!=IDB_EXPAND)
			pWnd->EnableWindow(bEnable);
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}
}

void CSkydomeSettingDlg::LoadAttributeFromMap()
{
	CString strText;
	CSGPSkyDome* pSkydome=(CSGPSkyDome*)(WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getSkydome());
	if(pSkydome!=NULL)
	{
		strText = pSkydome->m_MF1FileName.toUTF8().getAddress();
		SetDlgItemText(IDE_MF1_PATH,strText);
		strText = pSkydome->m_cloudTextureFileName.toUTF8().getAddress();
		SetDlgItemText(IDE_CLOUD_TEXTURE_PATH,strText);
		strText = pSkydome->m_noiseTextureFileName.toUTF8().getAddress();
		SetDlgItemText(IDE_NOISE_TEXTURE_PATH,strText);
		strText.Format("%.6f",pSkydome->GetScale());
		SetDlgItemText(IDE_MODEL_SCALE,strText);
		strText.Format("%.6f",pSkydome->m_coludMoveSpeed_x);
		SetDlgItemText(IDE_CLOUD_MOVE_SPEED_X,strText);
		strText.Format("%.6f",pSkydome->m_coludMoveSpeed_z);
		SetDlgItemText(IDE_CLOUD_MOVE_SPEED_Z,strText);
		strText.Format("%.6f",pSkydome->m_coludNoiseScale);
		SetDlgItemText(IDE_NOISE_TRXTURE_SCALE,strText);
		strText.Format("%.6f",pSkydome->m_coludBrightness);
		SetDlgItemText(IDE_CLOUD_BRIGHTNESS,strText);
		// Atmosphere
		strText.Format("%.6f",pSkydome->GetScatter().m_fHGgFunction);
		SetDlgItemText(IDE_HGG_FUNCTION,strText);
		strText.Format("%.6f",pSkydome->GetScatter().m_fInscatteringMultiplier);
		SetDlgItemText(IDE_INSCATTER_MULTIPLIER,strText);
		strText.Format("%.6f",pSkydome->GetScatter().m_fBetaRayMultiplier);
		SetDlgItemText(IDE_BETARAY_MULTIPLIER,strText);
		strText.Format("%.6f",pSkydome->GetScatter().m_fBetaMieMultiplier);
		SetDlgItemText(IDE_BETAMIE_MULTIPLIER,strText);
		strText.Format("%.6f",pSkydome->GetScatter().m_fSunIntensity);
		SetDlgItemText(IDE_SUN_INTENSITY,strText);
		strText.Format("%.6f",pSkydome->GetScatter().m_fTurbitity);
		SetDlgItemText(IDE_TURBITITY,strText);
	}
	CSGPWorldSun* pWorldSun=(CSGPWorldSun*)(WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getWorldSun());
	if(pWorldSun!=NULL)
	{
		strText.Format("%.2f",pWorldSun->m_fSunPosition);
		SetDlgItemText(IDE_SUN_POSITION,strText);
	}
}