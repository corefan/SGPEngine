// ParticleSystemDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "ParticleSystemDlg.h"
#include "afxdialogex.h"

#include "CommonMacroDef.h"
#include "ParticleConfigDlg.h"
#include "../SGPLibraryCode/SGPHeader.h"

#include "ParticleGroupGridProperty.h"
#include "ParticleCheckGridProperty.h"
#include "ParticleEntryGridProperty.h"
#include "ParticleEntrysGridProperty.h"
#include "ParticleEmitterGridProperty.h"
#include "ParticleEmittersGridProperty.h"
#include "ParticleModifierGridProperty.h"
#include "ParticleModifiersGridProperty.h"
#include "ParticleMenuGridProperty.h"
#include "ParticleGroupRenameDlg.h"

#include "MainFrame.h"
#include "ParticleConfigPane.h"
#include "ParticleCtrlMsgDef.h"
// CParticleSystemDlg 对话框

IMPLEMENT_DYNAMIC(CParticleSystemDlg, CDialogEx)

CParticleSystemDlg::CParticleSystemDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CParticleSystemDlg::IDD, pParent)
{
	m_NewGroupCount=0;
}

CParticleSystemDlg::~CParticleSystemDlg()
{
}

void CParticleSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CParticleSystemDlg, CDialogEx)
	ON_BN_CLICKED(IDB_SYSTEM_SETTING, &CParticleSystemDlg::OnBnClickedSystemSetting)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDB_PARTICLE_ADD_GROUP,OnBnClickedParticleAddGroup)
	ON_MESSAGE(UM_UPDATE_PROPERTYCTRL,UpdateDetailList)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED,OnParticleDetailChanged)
END_MESSAGE_MAP()

// CParticleSystemDlg 消息处理程序

BOOL CParticleSystemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ExpandButton.SubclassDlgItem(IDB_SYSTEM_SETTING,this);

	CRect rectDummy;
	rectDummy.SetRectEmpty();
	m_DetailList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, (UINT)-1);
	m_DetailList.EnableHeaderCtrl(FALSE);
	m_DetailList.SetVSDotNetLook();
	m_DetailList.MarkModifiedProperties();
	m_DetailList.EnableDescriptionArea();
	m_Font.CreatePointFont(93,_T("微软雅黑"));
	m_DetailList.SetFont(&m_Font);

	m_DetailList.SetCustomColors(RGB(255,255,255),RGB(0,0,0),RGB(141,178,230),RGB(0,0,0),RGB(225,238,255),RGB(0,0,0),RGB(141,178,230));

	return TRUE;
}

void CParticleSystemDlg::OnBnClickedSystemSetting()
{
	SetShowMode(!m_ExpandButton.m_bExpanded);
	((CParticleConfigDlg*)GetParent())->NotifySizeChanged();
}

void CParticleSystemDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if(m_ExpandButton.GetSafeHwnd()==NULL) return ;

	if(m_ExpandButton.GetSafeHwnd()!=NULL)
		m_ExpandButton.MoveWindow(0,2,cx,BUTTON_HEIGHT);

	// "Add Group" Button
	if(GetDlgItem(IDB_PARTICLE_ADD_GROUP)!=NULL)
	{
		CRect rcAdd;
		GetDlgItem(IDB_PARTICLE_ADD_GROUP)->GetWindowRect(rcAdd);
		ScreenToClient(rcAdd);
		int offset=(cx-2)-rcAdd.right;
		rcAdd.OffsetRect(offset,0);
		GetDlgItem(IDB_PARTICLE_ADD_GROUP)->MoveWindow(rcAdd);
	}

	CRect rcSeparate;
	GetDlgItem(IDC_SEPARATE)->GetWindowRect(rcSeparate);
	ScreenToClient(rcSeparate);

	// Button "Apply"
	CRect rcApply;
	GetDlgItem(IDB_PARTICLE_APPLY)->GetClientRect(rcApply);
	rcApply.OffsetRect((cx-2)-rcApply.right,(cy-2)-rcApply.bottom);
	if(IsExpand()) GetDlgItem(IDB_PARTICLE_APPLY)->MoveWindow(rcApply);

	m_DetailList.MoveWindow(0,rcSeparate.top,cx,rcApply.top-rcSeparate.top-2);
}

BOOL CParticleSystemDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN)
			return TRUE;
	}

/*	if(pMsg->hwnd==m_DetailList.m_hWnd)
	{
		return m_DetailList.PreTranslateMessage(pMsg);
	}*/

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CParticleSystemDlg::OnBnClickedParticleAddGroup()
{
	CString strGroupName;
	strGroupName.Format("Group %d",++m_NewGroupCount);
	CParticleGroupGridProperty* pGroupProperty=new CParticleGroupGridProperty(strGroupName);
	m_GroupPropertyVector.push_back(pGroupProperty);
	pGroupProperty->Init();
	m_DetailList.AddProperty(pGroupProperty);
}

void CParticleSystemDlg::SetShowMode(BOOL bExpand)
{
	if(bExpand)
	{
		m_ExpandButton.SetExpand(TRUE);
		SetWindowPos(NULL,0,0,m_rc.Width(),m_rc.Height(),SWP_NOMOVE);
	}
	else
	{
		m_ExpandButton.SetExpand(FALSE);
		CRect rc;
		m_ExpandButton.GetWindowRect(rc);
		ScreenToClient(rc);
		SetWindowPos(NULL,0,0,rc.bottom+1,rc.Height()+2,SWP_NOMOVE);
	}
}

void CParticleSystemDlg::LoadFromData(const sgp::ParticleSystemParam& data)
{
	m_BoundBoxComputeProperty=new CParticleCheckGridProperty("EnableBoundBoxCompute",data.m_bEnableAABBCompute);
	m_DetailList.AddProperty(m_BoundBoxComputeProperty);

	m_GroupPropertyVector.clear();
	for(uint32 i=0;i<data.m_groupCount;++i)
	{
		CString strGroupName;
		strGroupName.Format("Group %s",data.m_pGroupParam[i].m_GroupName);
		CParticleGroupGridProperty* pGroupProperty=new CParticleGroupGridProperty(strGroupName/*data.m_pGroupParam[i].m_GroupName*/);
		m_DetailList.AddProperty(pGroupProperty);
		pGroupProperty->LoadFromData(data.m_pGroupParam[i]);
		m_GroupPropertyVector.push_back(pGroupProperty);
	}
	m_NewGroupCount=data.m_groupCount;
}

BOOL CParticleSystemDlg::CheckValidate()
{
	return TRUE;
}

void CParticleSystemDlg::SaveToData(sgp::ParticleSystemParam& data)
{
	data.m_bEnableAABBCompute=m_BoundBoxComputeProperty->GetCheck();

	data.m_groupCount=m_GroupPropertyVector.size();
	if(data.m_groupCount!=0)
	{
		data.m_pGroupParam=new ParticleGroupParam[data.m_groupCount];
		for(uint32 i=0;i<data.m_groupCount;++i)
			m_GroupPropertyVector[i]->SaveToData(data.m_pGroupParam[i]);
	}
	else data.m_pGroupParam=NULL;
}

LRESULT CParticleSystemDlg::UpdateDetailList(WPARAM wParam,LPARAM lParam)
{
	m_DetailList.AdjustLayout();
	return 0;
}

BOOL CParticleSystemDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT nID=LOWORD(wParam);
	CMFCPropertyGridProperty* pSelProperty=m_DetailList.GetCurSel();
	switch(nID)
	{
	case MENU_DELETE_GROUP:
		{
			CParticleGroupGridProperty* pGroupProperty=(CParticleGroupGridProperty*)pSelProperty;
			std::vector<CParticleGroupGridProperty*>::iterator iter;
			for(iter=m_GroupPropertyVector.begin();iter!=m_GroupPropertyVector.end();++iter)
			{
				if(*iter==pGroupProperty)
				{
					m_GroupPropertyVector.erase(iter);
					break;
				}
			}
			m_DetailList.DeleteProperty(pSelProperty);
		}
		break;
	case MENU_DELETE_EMITTER:
		{
			CParticleEmitterGridProperty* pEmitterProperty=(CParticleEmitterGridProperty*)pSelProperty;
			CParticleEmittersGridProperty* pEmittersProperty=(CParticleEmittersGridProperty*)(pEmitterProperty->GetParent());
			pEmittersProperty->DeleteEmitter(pEmitterProperty);
			m_DetailList.DeleteProperty(pSelProperty);
		}
		break;
	case MENU_DELETE_MODIFIER:
		{
			CParticleModifierGridProperty* pModifierProperty=(CParticleModifierGridProperty*)pSelProperty;
			CParticleModifiersGridProperty* pModifiersProperty=(CParticleModifiersGridProperty*)(pModifierProperty->GetParent());
			pModifiersProperty->DeleteModifier(pModifierProperty);
			m_DetailList.DeleteProperty(pSelProperty);
		}
		break;
	case MENU_DELETE_ENTRY:
		{
			CParticleEntryGridProperty* pEntryProperty=(CParticleEntryGridProperty*)pSelProperty;
			CParticleEntrysGridProperty* pEntrysProperty=(CParticleEntrysGridProperty*)(pEntryProperty->GetParent());
			pEntrysProperty->DeleteEntry(pEntryProperty);
			m_DetailList.DeleteProperty(pSelProperty);
		}
		break;
	case MENU_ADD_EMITTER:
		{
			CParticleEmittersGridProperty* pEmitters=(CParticleEmittersGridProperty*)pSelProperty;
			pEmitters->AddNewEmitter();
			m_DetailList.AdjustLayout();
		}
		break;
	case MENU_ADD_MODIFIER:
		{
			CParticleModifiersGridProperty* pModifiers=(CParticleModifiersGridProperty*)pSelProperty;
			pModifiers->AddNewModifier();
			m_DetailList.AdjustLayout();
		}
		break;
	case MENU_ADD_GROUP:
		{
			CParticleGroupGridProperty* pGroupProperty=new CParticleGroupGridProperty("New Group");
			m_DetailList.AddProperty(pGroupProperty);
			pGroupProperty->Init();
			m_DetailList.SetCurSel(pGroupProperty);
			m_DetailList.AdjustLayout();
		}
		break;
	case MENU_ADD_ENTRY:
		{
			CParticleEntrysGridProperty* pEntrysProperty=(CParticleEntrysGridProperty*)pSelProperty;
			pEntrysProperty->AddNewEntry();
			m_DetailList.AdjustLayout();
		}
		break;
	case MENU_USE_SELECTED_TEXTURE:
		{
			CString texturePath;
			((CMainFrame*)AfxGetMainWnd())->GetSelectedResourceTexture(texturePath);

			CParticleMenuGridProperty* pTextureProperty=(CParticleMenuGridProperty*)pSelProperty;
			pTextureProperty->SetValue(texturePath);
			if(pTextureProperty->GetValue()!=pTextureProperty->GetOriginalValue())
				pTextureProperty->SetModifiedFlag();
		}
		break;
	case MENU_GROUP_RENAME:
		{
			CParticleGroupRenameDlg dlg;
			dlg.SetProperty(pSelProperty);
			dlg.DoModal();
		}
		break;
	case IDB_PARTICLE_ADD_GROUP:
		OnBnClickedParticleAddGroup();
		break;
	case IDB_SYSTEM_SETTING:
		OnBnClickedSystemSetting();
		break;
	case IDB_PARTICLE_APPLY:
		{
			CParticleConfigPane* pConfigPane=(CParticleConfigPane*)(GetParent()->GetParent());
			bool *bShowFlag=NULL;
			int groupCount=m_GroupPropertyVector.size();
			if(groupCount!=0)
			{
				bShowFlag=new bool[groupCount];
				for(int i=0;i<groupCount;++i)
					bShowFlag[i]=m_GroupPropertyVector[i]->m_ShowProperty->GetCheck();
			}
			pConfigPane->OnParticleApply(bShowFlag);
			if(bShowFlag!=NULL) delete [] bShowFlag; 
		}
		break;
	}
	return 0;
}

void CParticleSystemDlg::SetShowName(CString strName)
{
	SetDlgItemText(IDB_SYSTEM_SETTING,strName);
}

LRESULT CParticleSystemDlg::OnParticleDetailChanged(WPARAM wParam,LPARAM lParam)
{
	CMFCPropertyGridProperty* pProp=(CMFCPropertyGridProperty*)lParam;
	CString strAttrName=pProp->GetName();
	if(strAttrName=="Samples")
	{
		long data=pProp->GetValue().lVal;
		if(data<2) pProp->SetValue(COleVariant((long)2));
	}
	else if(strAttrName=="Period")
	{
		float data=pProp->GetValue().fltVal;
		if(data<0.00001f&&data>-0.00001f) pProp->SetValue(COleVariant(1.0f));
	}

	return 0;
}

void CParticleSystemDlg::PreCloseAction()
{
	m_DetailList.SetCurSel(m_BoundBoxComputeProperty,FALSE);
}