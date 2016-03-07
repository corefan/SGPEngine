// FindResultList.cpp : 实现文件
//

#include "stdafx.h"
#include "FindResultList.h"
#include "WorldMapManager.h"

// CFindResultList

CFindResultList* CFindResultList::s_pResultList=NULL;

IMPLEMENT_DYNAMIC(CFindResultList, CListBox)

CFindResultList::CFindResultList()
{

}

CFindResultList::~CFindResultList()
{
}

BEGIN_MESSAGE_MAP(CFindResultList, CListBox)
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CFindResultList 消息处理程序

int CFindResultList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_Font.CreatePointFont(90,_T("微软雅黑"));
	SetFont(&m_Font);
	s_pResultList=this;

	return 0;
}

void CFindResultList::FindObjectsBySceneName(CString strContent)
{
	PurgeResource();

	int count=0;

	strContent.MakeLower();
	std::vector<ISGPObject*> objVector=WorldMapManager::GetInstance()->GetWorldObjects();
	for(uint32 i=0;i<objVector.size();++i)
	{
		CString sceneName=objVector[i]->m_SceneObjectName;
		sceneName.MakeLower();
		if(sceneName.Find(strContent)!=-1)
		{
			AddString(CString("    ")+objVector[i]->m_SceneObjectName);
			m_FindObjectVec.push_back(CommonObject(objVector[i]));
			++count;
		}
	}
	std::vector<ISGPLightObject*> lightVector=WorldMapManager::GetInstance()->GetLightObjects();
	for(uint32 i=0;i<lightVector.size();++i)
	{
		CString sceneName=lightVector[i]->m_SceneObjectName;
		sceneName.MakeLower();
		if(sceneName.Find(strContent)!=-1)
		{
			AddString(CString("    ")+lightVector[i]->m_SceneObjectName);
			m_FindObjectVec.push_back(CommonObject(lightVector[i]));
			++count;
		}
	}

	CString strInfo;
	strInfo.Format("Find %d objects:",count);
	InsertString(0,strInfo);
}

void CFindResultList::FindObjectsByID(uint32 id)
{
	PurgeResource();

	int count=0;

	std::vector<ISGPObject*> objVector=WorldMapManager::GetInstance()->GetWorldObjects();
	for(uint32 i=0;i<objVector.size();++i)
	{
		if(objVector[i]->m_iSceneID==id)
		{
			AddString(CString("    ")+objVector[i]->m_SceneObjectName);
			m_FindObjectVec.push_back(CommonObject(objVector[i]));
			++count;
		}
	}
	std::vector<ISGPLightObject*> lightVector=WorldMapManager::GetInstance()->GetLightObjects();
	for(uint32 i=0;i<lightVector.size();++i)
	{
		if(lightVector[i]->m_iLightID==id)
		{
			AddString(CString("    ")+lightVector[i]->m_SceneObjectName);
			m_FindObjectVec.push_back(CommonObject(lightVector[i]));
			++count;
		}
	}

	CString strInfo;
	strInfo.Format("Find %d objects:",count);
	InsertString(0,strInfo);
}

void CFindResultList::FindObjectsByMF1Name(CString strContent)
{
	PurgeResource();

	int count=0;

	strContent.MakeLower();
	std::vector<ISGPObject*> objVector=WorldMapManager::GetInstance()->GetWorldObjects();
	for(uint32 i=0;i<objVector.size();++i)
	{
		CString mf1Name=objVector[i]->m_MF1FileName;
		mf1Name.MakeLower();
		if(mf1Name.Find(strContent)!=-1)
		{
			AddString(CString("    ")+objVector[i]->m_SceneObjectName);
			m_FindObjectVec.push_back(CommonObject(objVector[i]));
			++count;
		}
	}

	CString strInfo;
	strInfo.Format("Find %d objects:",count);
	InsertString(0,strInfo);
}

void CFindResultList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int nSelectedItem=GetCurSel();
	if(nSelectedItem==LB_ERR) return ;

	CRect rc;
	GetItemRect(nSelectedItem,rc);
	if(rc.PtInRect(point)&&nSelectedItem>=1)
		SceneObjectManager::GetInstance()->FocusObject(m_FindObjectVec[nSelectedItem-1],this);

	CListBox::OnLButtonDblClk(nFlags, point);
}

void CFindResultList::PurgeResource()
{
	ResetContent();
	m_FindObjectVec.clear();
}