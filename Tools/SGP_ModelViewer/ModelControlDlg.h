#pragma once


// CModelControlDlg 对话框
#include "AnimProgressBar.h"
#include "RoundButton.h"

#include "DlgScrollMacro.h"

class CModelControlDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CModelControlDlg)

public:
	CModelControlDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CModelControlDlg();

// 对话框数据
	enum { IDD = IDD_MODELCONTROL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
//	Declare_DlgScroll()
public:
	afx_msg void OnActionSelChanged();
	afx_msg void OnPlayAnimation();//play animation
	afx_msg void OnPlayPrevFrame();//play prev frame
	afx_msg void OnPlayNextFrame();//play next frame
	afx_msg void OnPlayPrevAnimation();
	afx_msg void OnPlayNextAnimation();
	afx_msg void OnPlayMixedAnimation();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	void ModelChanged(BOOL bDynamic);
	void ModelClosed();
	void FramePlayChanged(int nFrame);
	static CModelControlDlg* s_pModelCtrlDlg;
	static void NotifyModelChanged(BOOL bDynamic);
	static void NotifyPlayFrameChanged(int nFrame);
private:
	CComboBox m_ComboBox;
	CComboBox m_UpperAnimBox;
	CComboBox m_LowerAnimBox;
	CAnimProgressBar m_ProgressBar;
	int m_nStartFrame;
	int m_nEndFrame;
	int m_nCurrentFrame;
	float m_fPlaySpeed;
	CFont m_Font;
	BOOL m_bPlayMode;//whether in play mode
private:
	CRoundButton m_PlayBtn;
	CRoundButton m_PrevFrameBtn;
	CRoundButton m_NextFrameBtn;
	CRoundButton m_PrevAnimationBtn;
	CRoundButton m_NextAnimationBtn;
	CRoundButton m_MixedAnimationBtn;
	CEdit m_CurrFrameEdit;
	CEdit m_AnimSpeedEdit;
public:
	//set current frame
	void SetCurrentFrame(int nFrame);
	void NotifyCurrentFrame(float fPos);
	void EnableAllWindows(BOOL bEnable);
	void EnableWindowInPlayMode(BOOL bEnable);
	void EnableMixPlay(BOOL bEnable);
//	void EnableGeneralPlay(BOOL bEnable);
	void EnableWindowInMixPlayMode(BOOL bEnable);
	float GetPlaySpeed();
	
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CRect m_rc;
	void InitScrollBar();
	void SetScrollBar();

	CMFCToolTipCtrl m_ToolTip;
	bool m_bNewAnim;
	int m_nCurrActionSel;
	bool m_bHaveMixedPlay;
};
