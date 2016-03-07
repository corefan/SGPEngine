#pragma once

// CSettingDlg 对话框
#include "ColorButton.h"
#include "DlgScrollMacro.h"
#include "SettingParticleDlg.h"

class CSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSettingDlg();

// 对话框数据
	enum { IDD = IDD_SETTING_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSetBkgndColor();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
private:
	CColorButton m_ColorButton;
	CColorButton m_DefaultColBtn;
	CSettingParticleDlg m_ParticleDlg;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void NotifySizeChanged();
	void InitScrollBar();
	void SetScrollBar();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	int m_CommonHeight;
	afx_msg LRESULT OnNcHitTest(CPoint point);
};
