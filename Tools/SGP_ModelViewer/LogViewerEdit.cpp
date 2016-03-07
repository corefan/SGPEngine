// LogViewerEdit.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "LogViewerEdit.h"


// CLogViewerEdit

IMPLEMENT_DYNAMIC(CLogViewerEdit, CRichEditCtrl)

CLogViewerEdit::CLogViewerEdit()
{

}

CLogViewerEdit::~CLogViewerEdit()
{
}

BEGIN_MESSAGE_MAP(CLogViewerEdit, CRichEditCtrl)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CLogViewerEdit ��Ϣ�������
BOOL CLogViewerEdit::PreTranslateMessage(MSG* pMsg)
{
	//avoid ESC to exit richedit control
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN) return TRUE;
	}

	return CRichEditCtrl::PreTranslateMessage(pMsg);
}

int CLogViewerEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_Font.CreatePointFont(90,_T("΢���ź�"));
	SetFont(&m_Font);

	ModifyStyle(0,WS_CLIPCHILDREN|WS_CLIPSIBLINGS);

	return 0;
}
