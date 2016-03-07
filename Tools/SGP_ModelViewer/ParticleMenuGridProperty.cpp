#include "stdafx.h"
#include "Resource.h"

#include "ParticleMenuGridProperty.h"
#include "ParticleCtrlMsgDef.h"
#include "MainFrame.h"
#include "ModelViewerConfig.h"
#include "TextureInfoDlg.h"
#include <io.h>

IMPLEMENT_DYNAMIC(CParticleMenuGridProperty,CMFCPropertyGridProperty)

CParticleMenuGridProperty::CParticleMenuGridProperty(const CString& strName, const COleVariant& varValue):\
	CMFCPropertyGridProperty(strName,varValue)
{
	//	m_dwFlags = 0x0002;
	m_bAllowEdit=FALSE;
}

void CParticleMenuGridProperty::OnRClickValue(CPoint C,BOOL B)
{
	CPoint pt;
	GetCursorPos(&pt);

	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING,MENU_USE_SELECTED_TEXTURE,"Use Selected Texture");
	CString texturePath;
	if(((CMainFrame*)AfxGetMainWnd())->GetSelectedResourceTexture(texturePath))
		menu.EnableMenuItem(MENU_USE_SELECTED_TEXTURE,MF_BYCOMMAND|MF_ENABLED);
	else
		menu.EnableMenuItem(MENU_USE_SELECTED_TEXTURE,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
	menu.TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,m_pWndList->GetParent());
}

BOOL CParticleMenuGridProperty::OnEdit(LPPOINT lptClick)
{
	return FALSE;
}

BOOL CParticleMenuGridProperty::PushChar(UINT nChar)
{
	return TRUE;
}

void CParticleMenuGridProperty::SetModifiedFlag()
{
	CMFCPropertyGridProperty::SetModifiedFlag();
}

BOOL CParticleMenuGridProperty::OnDblClk(CPoint point)
{
	CString texPath=ModelViewerConfig::GetInstance()->GetDataFileDir()+"\\"+(CString)GetValue();
	CString strExt=PathFindExtension(texPath);
	if(_access(texPath,0)!=-1&&strExt!="")
	{
		CString texName=texPath.Right(texPath.GetLength()-texPath.ReverseFind('\\')-1);
		CTextureInfoDlg dlg(texPath,texName);
		dlg.DoModal();
	}
	return TRUE;
}