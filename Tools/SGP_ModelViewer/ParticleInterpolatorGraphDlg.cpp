// ParticleInterpolatorGraphDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "ParticleInterpolatorGraphDlg.h"
#include "afxdialogex.h"
#include "ModelViewerRenderInterface.h"
// CParticleInterpolatorGraphDlg 对话框

IMPLEMENT_DYNAMIC(CParticleInterpolatorGraphDlg, CDialogEx)

CParticleInterpolatorGraphDlg::CParticleInterpolatorGraphDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CParticleInterpolatorGraphDlg::IDD, pParent)
{

}

CParticleInterpolatorGraphDlg::~CParticleInterpolatorGraphDlg()
{
}

void CParticleInterpolatorGraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_GRAPH_CTRL,m_Graph);
}

BEGIN_MESSAGE_MAP(CParticleInterpolatorGraphDlg, CDialogEx)
	ON_MESSAGE(WM_KICKIDLE,OnKickIdle)
END_MESSAGE_MAP()

// CParticleInterpolatorGraphDlg 消息处理程序

LRESULT CParticleInterpolatorGraphDlg::OnKickIdle(WPARAM wParam,LPARAM lParam)
{
	ModelViewerRenderInterface::GetInstance()->Render();
	return TRUE;
}