
// SGP_WorldEditorView.cpp : CSGP_WorldEditorView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "WorldEditor.h"
#endif

#include "WorldEditorDoc.h"
#include "WorldEditorView.h"

#include "./Render Interface/WorldEditorRenderInterface.h"
#include "MouseMsgInterface.h"
#include "SceneObjectManager.h"
#include "MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSGP_WorldEditorView

IMPLEMENT_DYNCREATE(CSGP_WorldEditorView, CView)

BEGIN_MESSAGE_MAP(CSGP_WorldEditorView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CSGP_WorldEditorView::OnFilePrintPreview)
/*	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()*/
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

// CSGP_WorldEditorView 构造/析构

CSGP_WorldEditorView::CSGP_WorldEditorView()
{
	// TODO: 在此处添加构造代码

}

CSGP_WorldEditorView::~CSGP_WorldEditorView()
{
}

BOOL CSGP_WorldEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CSGP_WorldEditorView 绘制

void CSGP_WorldEditorView::OnDraw(CDC* /*pDC*/)
{
	CSGP_WorldEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CSGP_WorldEditorView 打印


void CSGP_WorldEditorView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CSGP_WorldEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CSGP_WorldEditorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CSGP_WorldEditorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

/*void CSGP_WorldEditorView::OnRButtonUp(UINT nFlags , CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CSGP_WorldEditorView::OnContextMenu(CWnd* pWnd, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}*/


// CSGP_WorldEditorView 诊断

#ifdef _DEBUG
void CSGP_WorldEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CSGP_WorldEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSGP_WorldEditorDoc* CSGP_WorldEditorView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSGP_WorldEditorDoc)));
	return (CSGP_WorldEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CSGP_WorldEditorView 消息处理程序

void CSGP_WorldEditorView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	WorldEditorRenderInterface::GetInstance()->ResizeWindow(cx,cy);
}

BOOL CSGP_WorldEditorView::OnEraseBkgnd(CDC* pDC)
{
	if(WorldEditorRenderInterface::GetInstance()->IsInitialized())
	{
		WorldEditorRenderInterface::GetInstance()->Render();
		return TRUE;
	}

	return CView::OnEraseBkgnd(pDC);
}

void CSGP_WorldEditorView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(nFlags&MK_MBUTTON)
	{
		float x=((float)(point.x-m_MPoint.x))*-0.108f;
		float y=((float)(point.y-m_MPoint.y))*0.108f;
		WorldEditorRenderInterface::GetInstance()->MoveCamera(x,y);
		m_MPoint=point;
	}

	if(nFlags&MK_RBUTTON)
	{
		float x=((float)(point.x-m_RPoint.x))*0.008f;
		float y=((float)(point.y-m_RPoint.y))*-0.008f;
		WorldEditorRenderInterface::GetInstance()->RotateCamera(y,x);
		m_RPoint=point;
	}

	WorldEditorRenderInterface::GetInstance()->SetCurPos(point.x,point.y);
	MouseMsgInterface::GetInstance()->MouseMove(point,nFlags);

	CView::OnMouseMove(nFlags, point);
}

BOOL CSGP_WorldEditorView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	WorldEditorRenderInterface::GetInstance()->ZoomCamera((float)(zDelta*0.01/**0.005*/));
//	WorldEditorRenderInterface::GetInstance()->SetCurPos(pt.x,pt.y);

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CSGP_WorldEditorView::OnLButtonDown(UINT nFlags, CPoint point)
{	
	MouseMsgInterface::GetInstance()->LButtonDown(point,nFlags);
	SetCapture();

	CView::OnLButtonDown(nFlags, point);
}

void CSGP_WorldEditorView::OnLButtonUp(UINT nFlags, CPoint point)
{
	MouseMsgInterface::GetInstance()->LButtonUp(point,nFlags);
	ReleaseCapture();

	CView::OnLButtonUp(nFlags, point);
}

void CSGP_WorldEditorView::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_RPoint=point;
	MouseMsgInterface::GetInstance()->RButtonDown(point,nFlags);
	WorldEditorRenderInterface::GetInstance()->SetRotateCenter();
	SetCapture();

	CView::OnRButtonDown(nFlags, point);
}

void CSGP_WorldEditorView::OnRButtonUp(UINT nFlags, CPoint point)
{
	MouseMsgInterface::GetInstance()->RButtonUp(point,nFlags);
	ReleaseCapture();
}

void CSGP_WorldEditorView::OnMButtonDown(UINT nFlags, CPoint point)
{
	m_MPoint=point;
	SetCapture();

	CView::OnMButtonDown(nFlags, point);
}

void CSGP_WorldEditorView::OnMButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
}

void CSGP_WorldEditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar==VK_DELETE)
		SceneObjectManager::GetInstance()->DeleteSelectedObjOperation();

	if(nChar==VK_ESCAPE)
		CMainFrame::GetInstance()->ExitFullScreen();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CSGP_WorldEditorView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if(MouseMsgInterface::GetInstance()->SetCursor()) return TRUE;
	else return CView::OnSetCursor(pWnd, nHitTest, message);
}
