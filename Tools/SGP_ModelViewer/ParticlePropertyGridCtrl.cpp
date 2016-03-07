// ParticlePropertyGridCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "ParticlePropertyGridCtrl.h"

#include "ParticleCtrlMsgDef.h"
#include "MainFrame.h"
// CParticlePropertyGridCtrl

IMPLEMENT_DYNAMIC(CParticlePropertyGridCtrl, CMFCPropertyGridCtrl)

CParticlePropertyGridCtrl::CParticlePropertyGridCtrl()
{

}

CParticlePropertyGridCtrl::~CParticlePropertyGridCtrl()
{

}


BEGIN_MESSAGE_MAP(CParticlePropertyGridCtrl, CMFCPropertyGridCtrl)
END_MESSAGE_MAP()

// CParticlePropertyGridCtrl 消息处理程序

BOOL CParticlePropertyGridCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT nID=LOWORD(wParam);
	switch(nID)
	{
	case MENU_DELETE_EMITTER:
	case MENU_DELETE_MODIFIER:
	case MENU_DELETE_GROUP:
	case MENU_DELETE_ENTRY:
		{
			CMFCPropertyGridProperty* pProperty=GetCurSel();
			CMFCPropertyGridProperty*& pProp=pProperty;
			DeleteProperty(pProp);
		}
		break;
	case MENU_USE_SELECTED_TEXTURE:
		{
			CString texturePath;
			((CMainFrame*)AfxGetMainWnd())->GetSelectedResourceTexture(texturePath);
			GetCurSel()->SetValue(texturePath);
		}
	}
	return 0;
}

BOOL CParticlePropertyGridCtrl::PreTranslateMessage(MSG* pMsg)
{
	if(!::IsWindow(pMsg->hwnd))
		return FALSE;
	return CMFCPropertyGridCtrl::PreTranslateMessage(pMsg);
}
