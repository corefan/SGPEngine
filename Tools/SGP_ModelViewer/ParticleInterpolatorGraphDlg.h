#pragma once

#include "ParticleInterpolatorGraph.h"
// CParticleInterpolatorGraphDlg �Ի���

class CParticleInterpolatorGraphDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CParticleInterpolatorGraphDlg)

public:
	CParticleInterpolatorGraphDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParticleInterpolatorGraphDlg();

// �Ի�������
	enum { IDD = IDD_PARTICLE_GRAPH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg LRESULT OnKickIdle(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	CParticleInterpolatorGraph m_Graph;
};
