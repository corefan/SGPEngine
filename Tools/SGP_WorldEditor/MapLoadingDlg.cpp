// MapLoadingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "MapLoadingDlg.h"

#include "afxdialogex.h"

#include "Render Interface\WorldEditorRenderInterface.h"
#include "..\SGPLibraryCode\SGPHeader.h"

// CMapLoadingDlg 对话框

IMPLEMENT_DYNAMIC(CMapLoadingDlg, CDialogEx)

CMapLoadingDlg::CMapLoadingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMapLoadingDlg::IDD, pParent)
{

}

CMapLoadingDlg::~CMapLoadingDlg()
{
}

void CMapLoadingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
}

BEGIN_MESSAGE_MAP(CMapLoadingDlg, CDialogEx)
	ON_MESSAGE(WM_KICKIDLE,OnKickIdle)
END_MESSAGE_MAP()

// CMapLoadingDlg 消息处理程序

BOOL CMapLoadingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetDlgItemText(IDC_INFO,_T(""));
	CSGPWorldMap* pWorldMap=WorldEditorRenderInterface::GetInstance()->GetWorldMap();
	if(pWorldMap!=NULL)
	{
		uint32 objCount=pWorldMap->m_Header.m_iSceneObjectNum+pWorldMap->m_Header.m_iLightObjectNum;
		m_Progress.SetRange(0,objCount);
	}

	return TRUE;
}

LRESULT CMapLoadingDlg::OnKickIdle(WPARAM wParam,LPARAM lParam)
{
	CSGPWorldMap* pWorldMap=WorldEditorRenderInterface::GetInstance()->GetWorldMap();
	if(pWorldMap)
	{
		int index=0;	
		ISGPWorldSystemManager* pWorldManager=WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager();

		int totalCount=(int)(pWorldMap->m_Header.m_iSceneObjectNum+pWorldMap->m_Header.m_iLightObjectNum);
		uint32 objNum=pWorldMap->m_Header.m_iSceneObjectNum;

		for(uint32 i=0;i<objNum;++i)
		{
			index++;
			ISGPObject *pObj=new ISGPObject;
			pObj->Clone(&(pWorldMap->m_pSceneObject[i]));
			SceneObjectManager::GetInstance()->EditorAddSceneObject(CommonObject(pObj));
			pWorldManager->loadObjectToWorldForEditor(pObj,1);

			UpdateCtrls(index,totalCount);
		}

		// load Lights
		uint32 lightNum=pWorldMap->m_Header.m_iLightObjectNum;
		for(uint32 i=0;i<lightNum;++i)
		{
			index++;

			ISGPLightObject* pLightObj=new ISGPLightObject;
			pLightObj->Clone(&(pWorldMap->m_pLightObject[i]));
			SceneObjectManager::GetInstance()->AddSceneObject(CommonObject(pLightObj));

			UpdateCtrls(index,totalCount);
		}
	}

	SendMessage(WM_CLOSE);

	return FALSE;
}

void CMapLoadingDlg::UpdateCtrls(int index,int total)
{
	CString str;
	str.Format("Objects Loading(%d/%d)...",index,total);
	SetDlgItemText(IDC_INFO,str);
	m_Progress.SetPos(index);
}

BOOL CMapLoadingDlg::PreTranslateMessage(MSG* pMsg)
{
	//avoid ESC/OK to exit dialog
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN) return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
