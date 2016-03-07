// ResourceTabCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "ResourceTreeCtrl.h"
#include "ResourceTabCtrl.h"


// CResourceTabCtrl

IMPLEMENT_DYNAMIC(CResourceTabCtrl, CMFCTabCtrl)

CResourceTabCtrl::CResourceTabCtrl()
{

}

CResourceTabCtrl::~CResourceTabCtrl()
{
	for(DWORD i=0;i<m_TreeVector.size();++i)
		delete m_TreeVector[i];
	m_TreeVector.clear();
}

BEGIN_MESSAGE_MAP(CResourceTabCtrl, CMFCTabCtrl)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CResourceTabCtrl 消息处理程序

int CResourceTabCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCTabCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	EnableTabSwap(FALSE);

	SetImageList(IDB_RESOURCE_BMP);
	// TODO:  在此添加您专用的创建代码
	CString str=_T("WorkDir");
	m_PathVector.push_back(str);
	TCHAR buf[100];
	DWORD len=GetLogicalDriveStrings(sizeof(buf)/sizeof(TCHAR),buf); 
	DWORD i;
	for(i=0;i<len;)
	{
		TCHAR *p=buf+i;
		i+=_tcslen(p)+1;
		if(DRIVE_FIXED==GetDriveType(p))
		{
			str=p;
			m_PathVector.push_back(str);
		}
	}

	for(i=0;i<m_PathVector.size();++i)
	{
		CResourceTreeCtrl* pTreeCtrl=new CResourceTreeCtrl;
		if(m_PathVector[i]!=_T("WorkDir"))
			pTreeCtrl->SetPath(m_PathVector[i]);
		else
		{
			TCHAR path[MAX_PATH];
	//		GetModuleFileName(GetModuleHandle("ModelViewer.dll"),path,MAX_PATH);
			GetModuleFileName(AfxGetInstanceHandle(),path,MAX_PATH);
			CString strPath=path;
			while(strPath.Right(4)!=_T("\\Bin"))
				strPath = strPath.Left(strPath.ReverseFind('\\'));
			strPath = strPath.Left(strPath.ReverseFind('\\'));
			strPath+=_T("\\Data\\");
			pTreeCtrl->SetPath(strPath);
		}

		CRect rectDummy;
		rectDummy.SetRectEmpty();
		const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
		pTreeCtrl->Create(dwViewStyle,rectDummy,this,i+1);

		CString tabname=m_PathVector[i];
		if(tabname.Right(2)==_T(":\\")) tabname=tabname.Left(tabname.GetLength()-2);
		AddTab(pTreeCtrl,tabname,0);
		m_TreeVector.push_back(pTreeCtrl);
	}

	return 0;
}
