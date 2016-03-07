#include "stdafx.h"

#include "ButtonPropertyGridProperty.h"
#include "Resource.h"
#include "MainFrame.h"
#include "ResourcePane.h"
#include "ResourceTabCtrl.h"
#include "ResourceTreeCtrl.h"

#include "../SGPLibraryCode/SGPHeader.h"//engine header
using namespace sgp;

IMPLEMENT_DYNAMIC(CButtonPropertyGridProperty,CMFCPropertyGridProperty)

CButtonPropertyGridProperty::CButtonPropertyGridProperty(const CString& strName, const COleVariant& varValue):\
	CMFCPropertyGridProperty(strName,varValue)
{
	m_dwFlags = 0x0002;
	m_bAllowEdit=FALSE;
}

CButtonPropertyGridProperty::~CButtonPropertyGridProperty()
{

}

void CButtonPropertyGridProperty::OnDrawButton(CDC* pDC,CRect rectButton)
{
	CMFCToolBarComboBoxButton button;

	pDC->FillSolidRect(rectButton,RGB(213,229,255));

	CPoint pt[3];
	pt[0].x=rectButton.left+4;
	pt[0].y=rectButton.top+4;

	pt[1].x=rectButton.left+4;
	pt[1].y=rectButton.bottom-4;

	pt[2].x=rectButton.right-2;
	pt[2].y=(rectButton.top+rectButton.bottom)/2;

	CBrush brush(RGB(0,128,0));
	CBrush* pOldBrush=(CBrush*)pDC->SelectObject(&brush);

	pDC->Polygon(pt,3);
	pDC->SelectObject(pOldBrush);
}

#include "ParticleConfigDlg.h"

void CButtonPropertyGridProperty::OnClickButton(CPoint point)
{
/*	CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd();
	CResourcePane* pResourcePane=(CResourcePane*)(&pMainFrame->m_ResourcePane);
	CResourceTabCtrl* pTabCtrl=(CResourceTabCtrl*)(&pResourcePane->m_TabCtrl);
	CResourceTreeCtrl* pTreeCtrl=pTabCtrl->m_TreeVector[pTabCtrl->GetActiveTab()];

	CString strWorkDir=pTabCtrl->m_TreeVector[0]->m_DirPath;
	strWorkDir=strWorkDir.Left(strWorkDir.GetLength()-1);

	HTREEITEM hItem=NULL;
	if((hItem=pTreeCtrl->GetSelectedItem())==NULL) return ;

	CString strItem=pTreeCtrl->GetItemText(hItem);
	int index=-1;
	if((index=strItem.ReverseFind('.'))==-1) return ;

	CString strExt=strItem.Right(strItem.GetLength()-index);
	strExt.MakeLower();
	if(strExt!=_T(".tga")&&strExt!=_T(".dds")) return ;

	CString strFilePath=pTreeCtrl->GetItemPath(pTreeCtrl->GetSelectedItem());

	char relativePath[MAX_PATH];
	if(!PathRelativePathTo(relativePath,(LPCTSTR)strWorkDir,FILE_ATTRIBUTE_DIRECTORY,(LPCTSTR)strFilePath,0))
		return ;
	CString strPath=relativePath;
	if(strPath.Left(2)==_T(".\\")) strPath=strPath.Right(strPath.GetLength()-2);

	SetValue(strPath);*/
//	CParticleConfigDlg dlg;
//	dlg.DoModal();
	CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd();
	pMainFrame->m_ParticlePane.ShowPane(TRUE,FALSE,TRUE);

	CMFCPropertyGridProperty::OnClickButton(point);
}
