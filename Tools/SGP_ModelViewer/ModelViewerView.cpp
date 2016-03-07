// ModelViewerView.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "ModelViewerView.h"

#include "ModelViewerRenderInterface.h"
// CModelViewerView

IMPLEMENT_DYNCREATE(CModelViewerView, CView)

CModelViewerView::CModelViewerView()
{

}

CModelViewerView::~CModelViewerView()
{
}

BEGIN_MESSAGE_MAP(CModelViewerView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_MBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CModelViewerView 绘图

void CModelViewerView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}

// CModelViewerView 诊断

#ifdef _DEBUG
void CModelViewerView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CModelViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CModelViewerView 消息处理程序


int CModelViewerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle(0,WS_CLIPSIBLINGS|WS_CLIPCHILDREN);
	
	return 0;
}

void CModelViewerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	ModelViewerRenderInterface::GetInstance()->ResizeWindow(cx,cy);
}


BOOL CModelViewerView::OnEraseBkgnd(CDC* pDC)
{	
	if(ModelViewerRenderInterface::GetInstance()->IsInitialized())
	{
		ModelViewerRenderInterface::GetInstance()->Render();
		return TRUE;
	}

	return CView::OnEraseBkgnd(pDC);
}



void CModelViewerView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_MPoint=point;
	CView::OnMButtonDown(nFlags, point);
}


void CModelViewerView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nFlags&MK_MBUTTON)
	{
		float x=((float)(point.x-m_MPoint.x))*-0.008f;
		float y=((float)(point.y-m_MPoint.y))*0.008f;
		ModelViewerRenderInterface::GetInstance()->MoveCamera(x,y);
		m_MPoint=point;
	}

	if(nFlags&MK_RBUTTON)
	{
		float x=((float)(point.x-m_RPoint.x))*0.008f;
		float y=((float)(point.y-m_RPoint.y))*-0.008f;
		ModelViewerRenderInterface::GetInstance()->RotateCamera(y,x);
		m_RPoint=point;
	}

	CView::OnMouseMove(nFlags, point);
}


void CModelViewerView::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_RPoint=point;
	CView::OnRButtonDown(nFlags, point);
}

BOOL CModelViewerView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	ModelViewerRenderInterface::GetInstance()->ZoomCamera((float)(zDelta*0.005));
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}
