// ModelViewerDoc.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "ModelViewerDoc.h"


// CModelViewerDoc

IMPLEMENT_DYNCREATE(CModelViewerDoc, CDocument)

CModelViewerDoc::CModelViewerDoc()
{
}

BOOL CModelViewerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CModelViewerDoc::~CModelViewerDoc()
{
}


BEGIN_MESSAGE_MAP(CModelViewerDoc, CDocument)
END_MESSAGE_MAP()


// CModelViewerDoc ���

#ifdef _DEBUG
void CModelViewerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CModelViewerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CModelViewerDoc ���л�

void CModelViewerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}
#endif


// CModelViewerDoc ����
