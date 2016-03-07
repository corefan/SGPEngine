// ModelViewerDoc.cpp : 实现文件
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


// CModelViewerDoc 诊断

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
// CModelViewerDoc 序列化

void CModelViewerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}
#endif


// CModelViewerDoc 命令
