#pragma once

#include "RoundButton.h"
#include "ParticleSystemDlg.h"
#include <vector>
// CParticleConfigDlg �Ի���

class CParticleConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CParticleConfigDlg)

public:
	CParticleConfigDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParticleConfigDlg();

// �Ի�������
	enum { IDD = IDD_PARTICLE_CONFIGURATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	CParticleSystemDlg m_SystemDlg;
	void NotifySizeChanged();
	CFont m_EditFont;
	static CFont* s_EditFont;
	static CFont* GetEditFont();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	void SetCtrlPos();

	static void SetDlgItemFloat(HWND hwnd,int ctrlID,float data);
	static BOOL GetDlgItemFloat(HWND hwnd,int ctrlID,float& data);
};
