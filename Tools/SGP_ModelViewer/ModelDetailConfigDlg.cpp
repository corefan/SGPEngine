// ModelDetailConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "ModelDetailConfigDlg.h"
#include "afxdialogex.h"

#include "../SGPLibraryCode/SGPHeader.h"//engine header
#include "ModelDetailPane.h"
#include "CommonMacroDef.h"
#include "ModelViewerRenderInterface.h"
#include "HelpFunction.h"

// CModelDetailConfigDlg 对话框

IMPLEMENT_DYNAMIC(CModelDetailConfigDlg, CDialogEx)

CModelDetailConfigDlg::CModelDetailConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CModelDetailConfigDlg::IDD, pParent)
{
	m_pTmpConfigSetting=NULL;
}

CModelDetailConfigDlg::~CModelDetailConfigDlg()
{
	PurgeConfigSetting();
}

void CModelDetailConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CModelDetailConfigDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_EXPAND,OnExpand)
	ON_BN_CLICKED(IDB_CONFIG_ADD,OnConfigAdd)
	ON_BN_CLICKED(IDB_CONFIG_SAVE,OnConfigSave)
	ON_BN_CLICKED(IDB_CONFIG_DELETE,OnConfigDelete)
	ON_BN_CLICKED(IDB_CONFIG_MODIFY,OnConfigModify)
	ON_LBN_SELCHANGE(IDC_CONFIG_LIST,OnConfigListSelChanged)
END_MESSAGE_MAP()


// CModelDetailConfigDlg 消息处理程序


BOOL CModelDetailConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetClientRect(m_rc);
	m_ExpandButton.SubclassDlgItem(IDC_EXPAND,this);
	EnableWindows(FALSE);
//	SetShowMode(FALSE);

	return TRUE;
}


BOOL CModelDetailConfigDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN)
			return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CModelDetailConfigDlg::SetShowMode(BOOL bExpand)
{
	m_ExpandButton.SetExpand(bExpand);
	if(bExpand)
	{
		SetWindowPos(NULL,0,0,m_rc.Width(),m_rc.Height(),SWP_NOMOVE);
	}
	else
	{
		SetWindowPos(NULL,0,0,m_rc.Width(),BUTTON_HEIGHT+1,SWP_NOMOVE);
	}
}

void CModelDetailConfigDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if(m_ExpandButton.m_hWnd!=NULL)
	{
		m_ExpandButton.MoveWindow(0,0,cx,BUTTON_HEIGHT);
	}
}

void CModelDetailConfigDlg::OnExpand()
{
	SetShowMode(!m_ExpandButton.m_bExpanded);
	((CModelDetailPane*)GetParent())->SetCtrlPos();
}

void CModelDetailConfigDlg::EnableWindows(BOOL bEnable)
{
	CWnd* pWnd=GetWindow(GW_CHILD);
	while(pWnd!=NULL)
	{
		pWnd->EnableWindow(bEnable);
		pWnd=pWnd->GetWindow(GW_HWNDNEXT);
	}
	m_ExpandButton.EnableWindow(TRUE);
}

void CModelDetailConfigDlg::OnConfigAdd()
{
}

void CModelDetailConfigDlg::OnConfigSave()
{
	if(!JudgeValidate(FALSE)) return ;

	SGPMF1ConfigSetting* pConfigSetting=new SGPMF1ConfigSetting;
	m_ConfigSetVector.push_back(pConfigSetting);
	CString strNew;
	strNew.Format("%d (new)",m_ConfigSetVector.size()-1);

	CListBox* pListBox=(CListBox*)GetDlgItem(IDC_CONFIG_LIST);
	pListBox->AddString(strNew);
	int nCurSel=m_ConfigSetVector.size()-1;
	pListBox->SetCurSel(nCurSel);
	
	CModelDetailPane* pDetailPane=(CModelDetailPane*)GetParent();
	ConfigSettingCopy(*(pDetailPane->m_pConfigSetting),*(m_ConfigSetVector[nCurSel]));
	OnConfigListSelChanged();
}

void CModelDetailConfigDlg::OnConfigDelete()
{
	CListBox* pListBox=(CListBox*)GetDlgItem(IDC_CONFIG_LIST);
	int nCurSel=pListBox->GetCurSel();
	if(nCurSel==0)
	{
		MessageBox("Default Config Setting can't be delete!","Config Setting",MB_OK|MB_ICONERROR);
		return ;
	}

	CString strInfo;
	strInfo.Format("Are you sure to delete Config Setting %d ?",nCurSel);
	if(MessageBox(strInfo,"Config Setting",MB_YESNO|MB_ICONQUESTION)==IDYES)
	{
		pListBox->SetCurSel(nCurSel-1);
		OnConfigListSelChanged();
		pListBox->DeleteString(nCurSel);
		std::vector<SGPMF1ConfigSetting*>::iterator iter=m_ConfigSetVector.begin();
		while(nCurSel-->0) ++iter;
		if(*iter!=NULL) delete *iter;
		m_ConfigSetVector.erase(iter);
	}
}

void CModelDetailConfigDlg::OnConfigModify()
{
	if(!JudgeValidate(TRUE)) return ;

	CModelDetailPane* pDetailPane=(CModelDetailPane*)GetParent();
	CListBox* pListBox=(CListBox*)GetDlgItem(IDC_CONFIG_LIST);
	int nCurSel=pListBox->GetCurSel();
	ConfigSettingCopy(*(pDetailPane->m_pConfigSetting),*(m_ConfigSetVector[nCurSel]));
}

BOOL CModelDetailConfigDlg::JudgeValidate(BOOL bAvoidSelf)
{
	CModelDetailPane* pDetailPane=(CModelDetailPane*)GetParent();
	SGPMF1ConfigSetting& currentConfig=*(pDetailPane->m_pConfigSetting);
	if(IsConfigSettingEmpty(currentConfig))
	{
		MessageBox("You can't save this config!\n\nBecause the config is the same to Default Config Setting!","Config Setting",MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	CListBox* pListBox=(CListBox*)GetDlgItem(IDC_CONFIG_LIST);
	int nCurSel=pListBox->GetCurSel();
	int nCount=pListBox->GetCount();
	for(int i=1;i<nCount;++i)
	{
		if(nCurSel==i&&bAvoidSelf) continue;
	//	if(CModelDetailPane::CompareConfigSetting(*(m_ConfigSetVector[i]),*(pDetailPane->m_pConfigSetting)))
		if(*(m_ConfigSetVector[i])==*(pDetailPane->m_pConfigSetting))
		{
			CString strInfo;
			strInfo.Format("You can't save this config!\n\nBecause the config is the same to Config Setting %d !",i);
			MessageBox(strInfo,"Config Setting",MB_OK|MB_ICONSTOP);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CModelDetailConfigDlg::IsConfigSettingEmpty(SGPMF1ConfigSetting& setting)
{
	if(setting.MeshConfigNum==0&&setting.ReplaceTextureConfigNum==0&&\
		setting.ParticleConfigNum==0&&setting.RibbonConfigNum==0)
		return TRUE;
	else return FALSE;
}

void CModelDetailConfigDlg::OnConfigListSelChanged()
{
	CListBox* pListBox=(CListBox*)GetDlgItem(IDC_CONFIG_LIST);

	int nCurSel=pListBox->GetCurSel();
	if(nCurSel!=0)
	{
		ModelViewerRenderInterface::GetInstance()->SetConfigSetting(m_ConfigSetVector[nCurSel]);
		GetDlgItem(IDB_CONFIG_MODIFY)->EnableWindow(TRUE);
	}
	else
	{
		ModelViewerRenderInterface::GetInstance()->SetConfigSetting(NULL);
		GetDlgItem(IDB_CONFIG_MODIFY)->EnableWindow(FALSE);
	}
	((CModelDetailPane*)GetParent())->LoadConfigSetting(m_ConfigSetVector[nCurSel]);
}

void CModelDetailConfigDlg::LoadConfigSetting(int nConfigCount,SGPMF1ConfigSetting* pSetting)
{
	EnableWindows();

	CListBox* pConfigList=(CListBox*)GetDlgItem(IDC_CONFIG_LIST);
	pConfigList->ResetContent();
	PurgeConfigSetting();

	pConfigList->AddString("Default");
	m_ConfigSetVector.push_back(NULL);
	if(nConfigCount!=0)
	{
		for(int i=0;i<nConfigCount;++i)
		{
			CString str;
			str.Format("%d",i+1);
			pConfigList->AddString(str);
			SGPMF1ConfigSetting* pConfigSetting=new SGPMF1ConfigSetting;
			ConfigSettingCopy(pSetting[i],*pConfigSetting);
			m_ConfigSetVector.push_back(pConfigSetting);
		}
	}
	pConfigList->SetCurSel(0);
	OnConfigListSelChanged();
}

void CModelDetailConfigDlg::PurgeConfigSetting()
{
	uint32 count=m_ConfigSetVector.size();
	for(uint32 i=1;i<count;++i)
		delete m_ConfigSetVector[i];
	m_ConfigSetVector.clear();
}

void CModelDetailConfigDlg::SaveConfigSetting()
{
	CSGPModelMF1* pModelMF1=ModelViewerRenderInterface::GetInstance()->GetModelMF1();
	if(pModelMF1==NULL) return ;
	m_MF1ConfigCount = pModelMF1->m_Header.m_iNumConfigs;
	m_pMF1ConfigSetting = pModelMF1->m_pConfigSetting;
	
	uint32 count=m_ConfigSetVector.size();
	ASSERT(count>0);
	
//	SGPMF1ConfigSetting* pConfigSetting=NULL;
	m_pTmpConfigSetting=NULL;
	if(count>1) m_pTmpConfigSetting=new SGPMF1ConfigSetting[count-1];
	pModelMF1->m_Header.m_iNumConfigs=count-1;
	pModelMF1->m_pConfigSetting=m_pTmpConfigSetting;

	for(uint32 i=1;i<count;++i)
		ConfigSettingCopy(*(m_ConfigSetVector[i]),m_pTmpConfigSetting[i-1]);
}

void CModelDetailConfigDlg::AfterSaveConfigSetting()
{
	if(m_pTmpConfigSetting!=NULL)
	{
		delete [] m_pTmpConfigSetting;
		m_pTmpConfigSetting=NULL;
	}

	CSGPModelMF1* pModelMF1=ModelViewerRenderInterface::GetInstance()->GetModelMF1();
	if(pModelMF1==NULL) return ;
	pModelMF1->m_Header.m_iNumConfigs = m_MF1ConfigCount;
	pModelMF1->m_pConfigSetting = m_pMF1ConfigSetting;
}

void CModelDetailConfigDlg::ModelClosed()
{
	CListBox* pListBox=(CListBox*)GetDlgItem(IDC_CONFIG_LIST);
	pListBox->ResetContent();
	PurgeConfigSetting();
	EnableWindows(FALSE);
}

BOOL CModelDetailConfigDlg::IsConfigSettingChanged()
{
	CSGPModelMF1* pModelMF1=ModelViewerRenderInterface::GetInstance()->GetModelMF1();
	if(pModelMF1!=NULL)
	{
		uint32 size=m_ConfigSetVector.size();
		if(size-1!=pModelMF1->m_Header.m_iNumConfigs) return TRUE;
		for(uint32 i=1;i<size;++i)
		{
			if(!(pModelMF1->m_pConfigSetting[i-1]==*(m_ConfigSetVector[i]))) return TRUE;
		}
	}
	return FALSE;
}
