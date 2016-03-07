#pragma once

#include "ParticleInterpolatorGraph.h"
// CParticleInterpolatorGraphDlg 对话框

class CParticleInterpolatorGraphDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CParticleInterpolatorGraphDlg)

public:
	CParticleInterpolatorGraphDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParticleInterpolatorGraphDlg();

// 对话框数据
	enum { IDD = IDD_PARTICLE_GRAPH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg LRESULT OnKickIdle(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	CParticleInterpolatorGraph m_Graph;
};
