#include "stdafx.h"

#include "MenuPropertyGridProperty.h"
#include "Resource.h"
#include "DetailPropertyGridCtrl.h"
#include "MainFrame.h"
#include "ModelDetailPane.h"
#include "ModelViewerConfig.h"
#include "TextureInfoDlg.h"
#include <io.h>
#include "../SGPLibraryCode/SGPHeader.h"//engine header
using namespace sgp;

IMPLEMENT_DYNAMIC(CMenuPropertyGridProperty,CMFCPropertyGridProperty)

CMenuPropertyGridProperty::CMenuPropertyGridProperty(const CString& strName, const COleVariant& varValue):\
	CMFCPropertyGridProperty(strName,varValue)
{
//	m_dwFlags = 0x0002;
	m_bAllowEdit=FALSE;
}

void CMenuPropertyGridProperty::OnRClickValue(CPoint C,BOOL B)
{
	CPoint pt;
	GetCursorPos(&pt);
	switch(m_MenuType)
	{
	case TextureMenu:
		{
			CMenu menu;
			menu.LoadMenu(IDM_TEXTURE_SELECTION);
			CMenu* pSubMenu=menu.GetSubMenu(0);

			// check whether the selected item is a texture
			CString texturePath;
			if(((CMainFrame*)AfxGetMainWnd())->GetSelectedResourceTexture(texturePath))
				pSubMenu->EnableMenuItem(IDM_USE_SELECTED_TEXTURE,MF_BYCOMMAND|MF_ENABLED);
			else
				pSubMenu->EnableMenuItem(IDM_USE_SELECTED_TEXTURE,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,(CDetailPropertyGridCtrl*)m_pWndList);
		}
		break;
	case ParticleMenu:
		{
			CMainFrame *pMainFrame=(CMainFrame*)AfxGetMainWnd();
			CParticleConfigPane& particlePane=pMainFrame->m_ParticlePane;

			CMenu menu;
			menu.CreatePopupMenu();
			menu.AppendMenu(MF_STRING,PARTICLE_MENU_CONFIG,"Click to Configure");
			UINT uFlag;
			if(particlePane.IsVisible()) uFlag=MF_BYCOMMAND|MF_DISABLED|MF_GRAYED;
			else uFlag=MF_BYCOMMAND|MF_ENABLED;
			menu.EnableMenuItem(PARTICLE_MENU_CONFIG,uFlag);
			if(m_TotalCount>1) menu.AppendMenu(MF_SEPARATOR);
			for(int i=0;i<m_TotalCount;++i)
			{
				if(i==m_currIndex) continue;
				CString str;
				str.Format("Use Particle %d Setting",i+1);
				menu.AppendMenu(MF_STRING,PARTICLE_MENU_START+i,str);
				menu.EnableMenuItem(PARTICLE_MENU_START+i,uFlag);
			}
			
			menu.TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,(CDetailPropertyGridCtrl*)m_pWndList);
		}
	}

}

BOOL CMenuPropertyGridProperty::OnDblClk(CPoint point)
{
	if(m_MenuType==ParticleMenu)
	{
		CMainFrame *pMainFrame=(CMainFrame*)AfxGetMainWnd();
		CParticleConfigPane& particlePane=pMainFrame->m_ParticlePane;
		if(!particlePane.IsVisible())
			((CDetailPropertyGridCtrl*)m_pWndList)->OnParticleConfig();
		else
			particlePane.ShowPane(TRUE,FALSE,TRUE);
	}
	if(m_MenuType==TextureMenu)
	{
		CString texPath=ModelViewerConfig::GetInstance()->GetDataFileDir()+"\\"+((CString)GetValue());
		CString strExt=PathFindExtension(texPath);
		if(_access(texPath,0)!=-1&&strExt!="")
		{
			CString texName=texPath.Right(texPath.GetLength()-texPath.ReverseFind('\\')-1);
			CTextureInfoDlg dlg(texPath,texName);
			dlg.DoModal();
		}
	}
	return TRUE;
}